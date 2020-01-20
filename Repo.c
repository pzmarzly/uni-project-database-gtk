#include "Repo.h"
#include "RepoData.h"
#include "Utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Timestamp semester_start;
  Timestamp semester_end;
  // section sizes
  unsigned table_size[TABLE_NUM];
  unsigned table_used[TABLE_NUM];
} Header;

static void default_header(Header *header, Timestamp start, Timestamp end) {
  header->semester_start = start;
  header->semester_end = end;
  header->table_size[TableEquipment] = TABLE_SIZE_DEFAULT;
  header->table_size[TablePeriodicReservation] = TABLE_SIZE_DEFAULT;
  header->table_size[TableOneTimeReservation] = TABLE_SIZE_LARGE;
  header->table_size[TableStringMetadata] = TABLE_SIZE_LARGE;
  header->table_size[TableStringFragment] = TABLE_SIZE_LARGE;
  memset(header->table_used, 0, sizeof(header->table_used));
}

struct Repo {
  char *path;
  FILE *file;
  Header header;
};

static bool load_header(Repo *repo) {
  fseek(repo->file, 0, SEEK_SET);
  int read = fread(&repo->header, sizeof(Header), 1, repo->file);
  if (read != 1)
    return false;
  if (repo->header.semester_start >= repo->header.semester_end) {
    printf("Uszkodzony nagłówek\n");
    return false;
  }
  for (size_t i = 0; i < TABLE_NUM; i++) {
    if (repo->header.table_used[i] > repo->header.table_size[i]) {
      printf("Uszkodzony nagłówek (rozmiar sekcji)\n");
      return false;
    }
  }
  return true;
}

static void save_header(Repo *repo) {
  fseek(repo->file, 0, SEEK_SET);
  fwrite(&repo->header, sizeof(Header), 1, repo->file);
  fflush(repo->file);
}

char *repo_get_path(Repo *repo) { return g_strdup(repo->path); }

Timestamp repo_get_semester_start(Repo *repo) {
  return repo->header.semester_start;
}

Timestamp repo_get_semester_end(Repo *repo) {
  return repo->header.semester_end;
}

static Repo *repo_open_internal(Repo *repo, RepoType type, Timestamp start,
                                Timestamp end) {
  repo->file = fopen(repo->path, type == RepoNew ? "w+b" : "r+b");
  if (repo->file == NULL) {
    printf("Nie udało się otworzyć %s, kod %d\n", repo->path, errno);
    free(repo);
    return NULL;
  }
  if (type == RepoNew) {
    default_header(&repo->header, start, end);
    save_header(repo);
  } else {
    if (!load_header(repo)) {
      free(repo);
      return NULL;
    }
    if (type == RepoDemo) {
      repo->header.semester_start = timestamp_today();
      repo->header.semester_end = repo->header.semester_start + 3600 * 24 * 365;
      save_header(repo);
    }
  }
  return repo;
}

Repo *repo_open(char *path, RepoType type, Timestamp start, Timestamp end) {
  Repo *repo = malloc(sizeof(Repo));
  repo->path = g_strdup(path);
  return repo_open_internal(repo, type, start, end);
}

void repo_close(Repo *repo) {
  if (repo != NULL) {
    save_header(repo);
    fclose(repo->file);
    free(repo->path);
    free(repo);
  }
}

static Repo *repo_save_as(Repo *repo, char *dest) {
  save_header(repo);
  // Turn repo into old_repo.
  Repo *old_repo = malloc(sizeof(Repo));
  memcpy(old_repo, repo, sizeof(Repo));
  // Load new repo in place.
  repo->path = g_strdup(dest);
  if (!repo_open_internal(repo, RepoNew, repo->header.semester_start,
                          repo->header.semester_end)) {
                            fclose(repo->file);
  fclose(old_repo->file);
  bug("Nie udało się przepisać bazy.");
  }

  // We can easily set size of an empty repo, so we use that opportunity
  // to enlarge tables if necessary.
  for (TableID i = 0; i < TABLE_NUM; i++) {
    repo->header.table_size[i] = old_repo->header.table_size[i];
    if (old_repo->header.table_used[i] * 2 > old_repo->header.table_size[i])
      repo->header.table_size[i] *= 2;
  }

  // Copy all elements from old_repo.
  AnyTableElement tmp;
  for (int i = 0; i < TABLE_NUM; i++) {
    for (unsigned j = 0; j < old_repo->header.table_used[i]; j++) {
      repo_get(old_repo, i, j, &tmp);
      repo_set(repo, i, j, &tmp);
    }
  }
  return old_repo;
}

static void enlarge_table(Repo *repo) {
  char *old_path = repo->path;
  char *temp = temp_file();
  if (temp == NULL) {
    printf("temp_file zwróciło błąd.\n");
    temp = malloc(strlen(old_path) + 2);
    strcpy(temp, old_path);
    strcat(temp, "~");
  }
  Repo *old_repo = repo_save_as(repo, temp);

  // Remove old repo.
  rename(temp, old_path);
  free(temp);
  free(repo->path);
  repo->path = g_strdup(old_path);
  repo_close(old_repo);
  return;
}

// Data types
typedef struct {
  unsigned position, size;
} Position;

static Position calc_position(Repo *repo, TableID table, ID id) {
  unsigned position = sizeof(Header);
  unsigned size = 0;
  // Cascade addition of offset.
  switch (table) {
  case TableStringFragment:
    if (!size)
      size = sizeof(StringFragment);
    position +=
        sizeof(StringMetadata) * repo->header.table_size[TableStringMetadata];
    __attribute__((fallthrough));
  case TableStringMetadata:
    if (!size)
      size = sizeof(StringMetadata);
    position += sizeof(OneTimeReservation) *
                repo->header.table_size[TableOneTimeReservation];
    __attribute__((fallthrough));
  case TableOneTimeReservation:
    if (!size)
      size = sizeof(OneTimeReservation);
    position += sizeof(PeriodicReservation) *
                repo->header.table_size[TablePeriodicReservation];
    __attribute__((fallthrough));
  case TablePeriodicReservation:
    if (!size)
      size = sizeof(PeriodicReservation);
    position += sizeof(Equipment) * repo->header.table_size[TableEquipment];
    __attribute__((fallthrough));
  case TableEquipment:
    if (!size)
      size = sizeof(Equipment);
    position += 0;
  }
  position += size * id;
  Position ret = {position, size};
  return ret;
}

void repo_get(Repo *repo, TableID table, ID id, void *dest) {
  if (id >= repo->header.table_used[table])
    error("Błąd wczytywania - indeks poza tablicą.");

  Position pos = calc_position(repo, table, id);
  fseek(repo->file, pos.position, SEEK_SET);
  int read = fread(dest, pos.size, 1, repo->file);
  if (read != 1) error("Błąd odczytu.");
}

void repo_set(Repo *repo, TableID table, ID id, void *src) {
  if (id > repo->header.table_used[table]) {
    fclose(repo->file);
    error("Błąd zapisu: dziura w tablicy.");
  }

  if (id >= repo->header.table_size[table])
    enlarge_table(repo);

  Position pos = calc_position(repo, table, id);
  fseek(repo->file, pos.position, SEEK_SET);
  int written = fwrite(src, pos.size, 1, repo->file);
  fflush(repo->file);
  if (written != 1) error("Błąd zapisu.");
  if (id == repo->header.table_used[table]) {
    repo->header.table_used[table] = id + 1;
    save_header(repo);
  }
}

void repo_del(Repo *repo, TableID table, ID id) {
  return repo_del_n(repo, table, id, 1);
}

void repo_del_n(Repo *repo, TableID table, ID id, unsigned n) {
  if (id + n > repo->header.table_used[table])
    error("Błąd usuwania - indeks poza tablicą.");

  AnyTableElement tmp;
  // Shift elements left.
  for (ID i = id; i + n < repo->header.table_used[table]; i++) {
    repo_get(repo, table, i + n, &tmp);
    repo_set(repo, table, i, &tmp);
  }
  repo->header.table_used[table] -= n;
  save_header(repo);

  // Zero out the free space.
  memset(tmp.bytes, 0, sizeof(tmp.bytes));
  ID start = repo->header.table_used[table];
  for (ID i = 0; i < n; i++) {
    repo_set(repo, table, i + start, &tmp);
  }

  // repo_set just increased repo size, so we decrease it again.
  repo->header.table_used[table] -= n;
  save_header(repo);
}

ID repo_len(Repo *repo, TableID table) {
  return repo->header.table_used[table];
}
