#pragma once
#include <stdbool.h>

typedef unsigned long long int Timestamp;
typedef unsigned short HourAndMinutes;
// HourAndMinutes value that signalises an error.
#define HM_INVALID 3000
typedef unsigned int ID;
// UINT_MAX
#define INVALID_ID 4294967295

typedef enum {
  RepoOpen = 0,
  RepoNew,
  RepoDemo,
} RepoType;

typedef struct Repo Repo;
char *repo_get_path(Repo *repo);
Timestamp repo_get_semester_start(Repo *repo);
Timestamp repo_get_semester_end(Repo *repo);

Repo *repo_open(char *path, RepoType type, Timestamp start, Timestamp end);
void repo_close(Repo *repo);

// Number of vector-like tables.
#define TABLE_NUM 5
// Amount of rows preallocated in an empty database.
#define TABLE_SIZE_DEFAULT 64
#define TABLE_SIZE_LARGE 1024
// Max size of a table row.
// max(sizeof(Equipment), sizeof(PeriodicReservation),
//     sizeof(OneTimeReservation), sizeof(StringMetadata),
//     sizeof(StringFragment))
#define MAX_STRUCT_SIZE 256
typedef struct {
  char bytes[MAX_STRUCT_SIZE];
} AnyTableElement;
// Max length of a string fragment.
#define STRING_FRAGMENT_MAX 255

// Compile with `make clean && make test DEBUG_REPO=1`
// to test reallocations.
#ifdef DEBUG_REPO
#undef TABLE_SIZE_DEFAULT
#define TABLE_SIZE_DEFAULT DEBUG_REPO
#undef TABLE_SIZE_LARGE
#define TABLE_SIZE_LARGE DEBUG_REPO
#undef STRING_FRAGMENT_MAX
#define STRING_FRAGMENT_MAX DEBUG_REPO
#endif

typedef enum {
  TableEquipment = 0,
  TablePeriodicReservation,
  TableOneTimeReservation,
  TableStringMetadata,
  TableStringFragment,
} TableID;

typedef enum {
  Projector = 0,
  Laptop,
  Whiteboard,
  Other,
} EquipmentType;

typedef struct {
  EquipmentType type;
  char name[32];
  ID description;
} Equipment;

typedef enum {
  Monday = 0,
  Tuesday,
  Wednesday,
  Thursday,
  Friday,
  Saturday,
  Sunday,
} Day;

typedef struct {
  ID item;
  Day day;
  HourAndMinutes start;
  HourAndMinutes end;
  Timestamp active_since;
  Timestamp active_until;
  ID description;
} PeriodicReservation;

typedef enum {
  Reservation = 0,
  Cancellation,
} OneTimeReservationType;

typedef struct {
  ID item;
  OneTimeReservationType type;
  Timestamp start;
  Timestamp end;
  ID description;
} OneTimeReservation;

typedef struct {
  ID start;
  unsigned len;
} StringMetadata;

typedef struct {
  char data[STRING_FRAGMENT_MAX];
} StringFragment;

void repo_get(Repo *repo, TableID table, ID id, void *dest);
void repo_set(Repo *repo, TableID table, ID id, void *src);
void repo_raw_del(Repo *repo, TableID table, ID id);
void repo_raw_del_n(Repo *repo, TableID table, ID id, unsigned n);
ID repo_len(Repo *repo, TableID table);

// Deletion wrappers - required because objects have relations
// that need to be deleted or fixed (if IDs change).
void repo_equipment_del(Repo *repo, ID id);
void repo_one_time_del(Repo *repo, ID id);
void repo_periodic_del(Repo *repo, ID id);
