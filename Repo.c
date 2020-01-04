#include "Repo.h"
#include "Utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Timestamp semester_start;
  bool semester_active;
  // section sizes
  unsigned table_size[TABLE_NUM];
  unsigned table_used[TABLE_NUM];
} Header;

static void default_header(Header *header) {
  GDateTime *time = g_date_time_new_now_utc();
  int year, month, day;
  g_date_time_get_ymd(time, &year, &month, &day);
  g_date_time_unref(time);

  GTimeZone *tz = g_time_zone_new_utc();
  time = g_date_time_new(tz, year, month, day, 0, 0, 0);
  header->semester_start = g_date_time_to_unix(time);

  header->semester_active = false;
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
  for (size_t i = 0; i < TABLE_NUM; i++) {
    if (repo->header.table_used[i] > repo->header.table_size[i]) {
      printf("Uszkodzony nagłówek\n");
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

void repo_set_semester_start(Repo *repo, Timestamp semester_start) {
  repo->header.semester_start = semester_start;
  save_header(repo);
}

bool repo_get_semester_active(Repo *repo) {
  return repo->header.semester_active;
}

void repo_set_semester_active(Repo *repo, bool semester_active) {
  repo->header.semester_active = semester_active;
  save_header(repo);
}

static Repo *repo_open_internal(Repo *repo, bool overwrite) {
  repo->file = fopen(repo->path, overwrite ? "w+b" : "r+b");
  if (repo->file == NULL) {
    printf("Nie udało się otworzyć %s, kod %d\n", repo->path, errno);
    free(repo);
    return NULL;
  }
  if (overwrite) {
    default_header(&repo->header);
    save_header(repo);
  } else {
    if (!load_header(repo)) {
      free(repo);
      return NULL;
    }
  }
  return repo;
}

Repo *repo_open(char *path, bool overwrite) {
  Repo *repo = malloc(sizeof(Repo));
  repo->path = g_strdup(path);
  return repo_open_internal(repo, overwrite);
}

void repo_close(Repo *repo) {
  if (repo != NULL) {
    save_header(repo);
    fclose(repo->file);
    free(repo->path);
    free(repo);
  }
}

Repo *repo_save_as(Repo *repo, char *dest) {
  save_header(repo);
  // Turn repo into old_repo.
  Repo *old_repo = malloc(sizeof(Repo));
  memcpy(old_repo, repo, sizeof(Repo));
  // Load new repo in place.
  repo->path = g_strdup(dest);
  if (!repo_open_internal(repo, true))
    goto fail;
  // Copy metadata.
  repo->header.semester_start = old_repo->header.semester_start;
  repo->header.semester_active = old_repo->header.semester_active;

  // We can easily set size of an empty repo, so we use that moment
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
      if (!repo_get(old_repo, i, j, &tmp))
        goto fail;
      repo_set(repo, i, j, &tmp);
    }
  }
  return old_repo;

fail:
  fclose(repo->file);
  fclose(old_repo->file);
  bug("Nie udało się przepisać bazy.");
  return NULL;
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

bool repo_get(Repo *repo, TableID table, ID id, void *dest) {
  if (id >= repo->header.table_used[table])
    return false;

  Position pos = calc_position(repo, table, id);
  fseek(repo->file, pos.position, SEEK_SET);
  int read = fread(dest, pos.size, 1, repo->file);
  return read == 1;
}

void repo_set(Repo *repo, TableID table, ID id, void *src) {
  if (id > repo->header.table_used[table]) {
    fclose(repo->file);
    bug("Dziura w tablicy.");
  }

  if (id >= repo->header.table_size[table])
    enlarge_table(repo);

  Position pos = calc_position(repo, table, id);
  fseek(repo->file, pos.position, SEEK_SET);
  fwrite(src, pos.size, 1, repo->file);
  fflush(repo->file);
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
    return;

  AnyTableElement tmp;
  // Shift elements left.
  for (ID i = id; i + n < repo->header.table_used[table]; i++) {
    if (!repo_get(repo, table, i + n, &tmp))
      bug("Nie można usunąć elementu.");
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
