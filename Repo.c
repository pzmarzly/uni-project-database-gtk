#include "Repo.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

typedef struct {
    Timestamp semester_start;
    // section sizes
    unsigned equipment_size;
    unsigned equipment_used;
    unsigned periodic_reservation_size;
    unsigned periodic_reservation_used;
    unsigned one_time_reservation_size;
    unsigned one_time_reservation_used;
    unsigned string_size;
    unsigned string_used;
} Header;

static void default_header(Header *header, Timestamp semester_start) {
    header->semester_start = semester_start;
    header->equipment_size =
        header->periodic_reservation_size = 64;
    header->one_time_reservation_size =
        header->string_size = 1024;
    header->equipment_used =
        header->periodic_reservation_size =
        header->one_time_reservation_size =
        header->string_used = 0;
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
static void resize_repo(
    Repo *repo,
    unsigned equipment,
    unsigned periodic_reservation,
    unsigned one_time_reservation,
    unsigned string
) {
    // TODO: resize
}
#pragma GCC diagnostic pop

// Data types

typedef struct {
    int len;
    char *data;
} String;

// Generic data ops
bool equipment_load(Repo *repo, EquipmentID id, Equipment *dest) {
    if (id >= repo->header.equipment_used) return false;
    fseek(repo->file, sizeof(Header) + id * sizeof(Equipment), SEEK_SET);
    int read = fread(dest, sizeof(Equipment), 1, repo->file);
    return read == 1;
}
void equipment_save(Repo *repo, EquipmentID id, Equipment *src) {
    if (id >= repo->header.equipment_size) {
        resize_repo(
            repo,
            repo->header.equipment_size * 2,
            repo->header.periodic_reservation_size,
            repo->header.one_time_reservation_size,
            repo->header.string_size
        );
    }
    fseek(repo->file, sizeof(Header) + id * sizeof(Equipment), SEEK_SET);
    fwrite(src, sizeof(Equipment), 1, repo->file);
    if (id >= repo->header.equipment_used)
        repo->header.equipment_used = id;
}
void equipment_remove(Repo *repo, EquipmentID id) {
    if (id >= repo->header.equipment_used) return;
    for (EquipmentID i = id; i + 1 < repo->header.equipment_used; i++) {
        Equipment item;
        equipment_load(repo, i + 1, &item);
        equipment_save(repo, i, &item);
    }
    repo->header.equipment_used--;
}
unsigned equipment_len(Repo *repo) {
    return repo->header.equipment_used;
}

bool periodic_reservation_load(Repo *repo, PeriodicReservationID id, PeriodicReservation *dest) {
    if (id >= repo->header.periodic_reservation_used) return false;
    fseek(repo->file, sizeof(Header) + id * sizeof(PeriodicReservation), SEEK_SET);
    int read = fread(dest, sizeof(PeriodicReservation), 1, repo->file);
    return read == 1;
}
void periodic_reservation_save(Repo *repo, PeriodicReservationID id, PeriodicReservation *src) {
    if (id >= repo->header.periodic_reservation_size) {
        resize_repo(
            repo,
            repo->header.equipment_size,
            repo->header.periodic_reservation_size * 2,
            repo->header.one_time_reservation_size,
            repo->header.string_size
        );
    }
    fseek(repo->file, sizeof(Header) + id * sizeof(PeriodicReservation), SEEK_SET);
    fwrite(src, sizeof(PeriodicReservation), 1, repo->file);
    if (id >= repo->header.periodic_reservation_used)
        repo->header.periodic_reservation_used = id;
}
void periodic_reservation_remove(Repo *repo, PeriodicReservationID id) {
    if (id >= repo->header.periodic_reservation_used) return;
    for (PeriodicReservationID i = id; i + 1 < repo->header.periodic_reservation_used; i++) {
        PeriodicReservation item;
        periodic_reservation_load(repo, i + 1, &item);
        periodic_reservation_save(repo, i, &item);
    }
    repo->header.periodic_reservation_used--;
}
unsigned periodic_reservation_len(Repo *repo) {
    return repo->header.periodic_reservation_used;
}

bool one_time_reservation_load(Repo *repo, OneTimeReservationID id, OneTimeReservation *dest) {
    if (id >= repo->header.one_time_reservation_used) return false;
    fseek(repo->file, sizeof(Header) + id * sizeof(OneTimeReservation), SEEK_SET);
    int read = fread(dest, sizeof(OneTimeReservation), 1, repo->file);
    return read == 1;
}
void one_time_reservation_save(Repo *repo, OneTimeReservationID id, OneTimeReservation *src) {
    if (id >= repo->header.one_time_reservation_size) {
        resize_repo(
            repo,
            repo->header.equipment_size,
            repo->header.periodic_reservation_size,
            repo->header.one_time_reservation_size * 2,
            repo->header.string_size
        );
    }
    fseek(repo->file, sizeof(Header) + id * sizeof(OneTimeReservation), SEEK_SET);
    fwrite(src, sizeof(OneTimeReservation), 1, repo->file);
    if (id >= repo->header.one_time_reservation_used)
        repo->header.one_time_reservation_used = id;
}
void one_time_reservation_remove(Repo *repo, OneTimeReservationID id) {
    if (id >= repo->header.one_time_reservation_used) return;
    for (OneTimeReservationID i = id; i  + 1 < repo->header.one_time_reservation_used; i++) {
        OneTimeReservation item;
        one_time_reservation_load(repo, i + 1, &item);
        one_time_reservation_save(repo, i, &item);
    }
    repo->header.one_time_reservation_used--;
}
unsigned one_time_reservation_len(Repo *repo) {
    return repo->header.one_time_reservation_used;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool string_load(Repo *repo, StringID id, char **dest) {
    // TODO: VL items
    return false;
}
StringID string_save(Repo *repo, char **src) {
    return 0;
}
void string_remove(Repo *repo, StringID id) {
}
unsigned string_len(Repo *repo) {
    return repo->header.string_used;
}
#pragma GCC diagnostic pop
