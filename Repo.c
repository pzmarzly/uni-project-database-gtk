#include "Repo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "Utils.h"

typedef struct {
    Timestamp semester_start;
    // section sizes
    unsigned table_size[TABLE_NUM];
    unsigned table_used[TABLE_NUM];
} Header;

static void default_header(Header *header, Timestamp semester_start) {
    header->semester_start = semester_start;
    header->table_size[TableEquipment] = TABLE_SIZE1;
    header->table_size[TablePeriodicReservation] = TABLE_SIZE1;
    header->table_size[TableOneTimeReservation] = TABLE_SIZE2;
    header->table_size[TableStringFragment] = TABLE_SIZE2;
    memset(header->table_used, 0, sizeof(header->table_used));
}

struct Repo {
    char *path;
    FILE *file;
    Header header;
};

Timestamp repo_get_semester_start(Repo *repo) {
    return repo->header.semester_start;
}

static bool load_header(Repo *repo) {
    fseek(repo->file, 0, SEEK_SET);
    int read = fread(&repo->header, sizeof(Header), 1, repo->file);
    if (read != 1) return false;
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

static Repo *repo_open_internal(Repo *repo, bool overwrite, Timestamp semester_start) {
    repo->file = fopen(repo->path, overwrite ? "w+b" : "r+b");
    if (repo->file == NULL) {
        printf("Nie udało się otworzyć %s, kod %d\n", repo->path, errno);
        free(repo);
        return NULL;
    }
    if (overwrite) {
        default_header(&repo->header, semester_start);
        save_header(repo);
    } else {
        if (!load_header(repo)) {
            free(repo);
            return NULL;
        }
    }
    return repo;
}

Repo* repo_open(char *path, bool overwrite, Timestamp semester_start) {
    Repo *repo = malloc(sizeof(Repo));
    repo->path = g_strdup(path);
    return repo_open_internal(repo, overwrite, semester_start);
}

void repo_close(Repo *repo) {
    if (repo != NULL) {
        save_header(repo);
        fclose(repo->file);
        free(repo->path);
        free(repo);
    }
}

static void enlarge_table(Repo *repo, TableID table) {
    // Turn repo into old_repo, new repo is a temp file.
    save_header(repo);
    Repo *old_repo = malloc(sizeof(Repo));
    memcpy(old_repo, repo, sizeof(Repo));
    repo->path = malloc(strlen(old_repo->path) + 2);
    strcpy(repo->path, old_repo->path);
    strcat(repo->path, "~");
    if (!repo_open_internal(repo, true, old_repo->header.semester_start)) goto fail;

    // We can easily set size of an empty repo.
    for (TableID i = 0; i < TABLE_NUM; i++) {
        repo->header.table_size[i] = old_repo->header.table_size[i];
        if (i == table) repo->header.table_size[i] *= 2;
    }

    // Copy all elements from old_repo.
    void *tmp = malloc(MAX_STRUCT_SIZE);
    for (int i = 0; i < TABLE_NUM; i++) {
        for (unsigned j = 0; j < old_repo->header.table_used[i]; j++) {
            if (!repo_get(old_repo, i, j, tmp)) goto fail;
            repo_set(repo, i, j, tmp);
        }
    }
    free(tmp);

    // Remove old_repo.
    rename(repo->path, old_repo->path);
    free(repo->path);
    repo->path = g_strdup(old_repo->path);
    repo_close(old_repo);
    return;

    fail:
    fclose(repo->file);
    fclose(old_repo->file);
    bug("Nie udało się zmienić rozmiaru bazy.");
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
            if (!size) size = sizeof(StringFragment);
            position += sizeof(StringMetadata) * repo->header.table_size[TableStringMetadata];
            __attribute__((fallthrough));
        case TableStringMetadata:
            if (!size) size = sizeof(StringMetadata);
            position += sizeof(OneTimeReservation) * repo->header.table_size[TableOneTimeReservation];
            __attribute__((fallthrough));
        case TableOneTimeReservation:
            if (!size) size = sizeof(OneTimeReservation);
            position += sizeof(PeriodicReservation) * repo->header.table_size[TablePeriodicReservation];
            __attribute__((fallthrough));
        case TablePeriodicReservation:
            if (!size) size = sizeof(PeriodicReservation);
            position += sizeof(Equipment) * repo->header.table_size[TableEquipment];
            __attribute__((fallthrough));
        case TableEquipment:
            if (!size) size = sizeof(Equipment);
            position += 0;
    }
    position += size * id;
    Position ret = {position, size};
    return ret;
}

bool repo_get(Repo *repo, TableID table, ID id, void *dest) {
    if (id >= repo->header.table_used[table]) return false;

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
        enlarge_table(repo, table);

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
    if (id + n > repo->header.table_used[table]) return;

    void *tmp = malloc(MAX_STRUCT_SIZE);
    // Shift elements to the left.
    for (ID i = id; i + n < repo->header.table_used[table]; i++) {
        if (!repo_get(repo, table, i + n, tmp)) bug("Nie można usunąć elementu.");
        repo_set(repo, table, i, tmp);
    }
    repo->header.table_used[table] -= n;
    save_header(repo);

    // Zero out the free space.
    memset(tmp, 0, MAX_STRUCT_SIZE);
    ID start = repo->header.table_used[table];
    for (ID i = 0; i < n; i++) {
        repo_set(repo, table, i + start, tmp);
    }
    free(tmp);

    // repo_set just increased repo size, so we decrease it again.
    repo->header.table_used[table] -= n;
    save_header(repo);
}

ID repo_len(Repo *repo, TableID table) {
    return repo->header.table_used[table];
}
