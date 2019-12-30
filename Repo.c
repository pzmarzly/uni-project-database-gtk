#include "Repo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef struct {
    Timestamp semester_start;
    // section sizes
    unsigned table_size[TABLE_NUM];
    unsigned table_used[TABLE_NUM];
} Header;

static void default_header(Header *header, Timestamp semester_start) {
    header->semester_start = semester_start;
    header->table_size[TableEquipment] = 64;
    header->table_size[TablePeriodicReservation] = 64;
    header->table_size[TableOneTimeReservation] = 1024;
    header->table_size[TableStringFragment] = 1024;
    memset(header->table_used, 0, sizeof(header->table_used));
}

struct Repo {
    FILE *file;
    Header header;
};

Timestamp repo_get_semester_start(Repo *repo) {
    return repo->header.semester_start;
}

static void load_header(Repo *repo) {
    fseek(repo->file, 0, SEEK_SET);
    fread(&repo->header, sizeof(Header), 1, repo->file);
}

static void save_header(Repo *repo) {
    fseek(repo->file, 0, SEEK_SET);
    fwrite(&repo->header, sizeof(Header), 1, repo->file);
    fflush(repo->file);
}

Repo* repo_open(char *path, bool create, Timestamp semester_start) {
    Repo *repo = malloc(sizeof(Repo));
    repo->file = fopen(path, create ? "wb" : "r+b");
    if (repo->file == NULL) {
        printf("Nie udało się otworzyć %s, kod %d\n", path, errno);
        return NULL;
    }
    if (create) {
        default_header(&repo->header, semester_start);
        save_header(repo);
    } else {
        load_header(repo);
    }
    return repo;
}

void repo_close(Repo *repo) {
    if (repo != NULL) {
        fclose(repo->file);
        free(repo);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void enlarge_table(Repo *repo, TableID table) {
    // TODO: resize
}
#pragma GCC diagnostic pop

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
            if (!size) size = sizeof(TableStringFragment);
            position += sizeof(OneTimeReservation) * repo->header.table_used[TableOneTimeReservation];
            __attribute__((fallthrough));
        case TableOneTimeReservation:
            if (!size) size = sizeof(OneTimeReservation);
            position += sizeof(PeriodicReservation) * repo->header.table_used[TablePeriodicReservation];
            __attribute__((fallthrough));
        case TablePeriodicReservation:
            if (!size) size = sizeof(PeriodicReservation);
            position += sizeof(Equipment) * repo->header.table_used[TableEquipment];
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
        printf("BUG: Dziura w tablicy.\n");
        return;
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
    if (id >= repo->header.table_used[table]) return;
    // Shift elements to the left.
    for (ID i = id; i + 1 < repo->header.table_used[table]; i++) {
        void *item;
        repo_get(repo, table, i + 1, &item);
        repo_set(repo, table, i, &item);
    }
    repo->header.table_used[table]--;
    save_header(repo);
}

unsigned repo_len(Repo *repo, TableID table) {
    return repo->header.table_used[table];
}
