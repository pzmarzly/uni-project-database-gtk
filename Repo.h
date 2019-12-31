#pragma once
#include <stdbool.h>

typedef unsigned long long int Timestamp;
typedef unsigned short HourAndMinute;

typedef struct Repo Repo;
Timestamp repo_get_semester_start(Repo *repo);
Repo* repo_open(char *path, bool overwrite, Timestamp semester_start);
void repo_close(Repo *repo);

// Data types
typedef unsigned int ID;

// Number of vector-like tables.
#define TABLE_NUM 5
// Amount of rows preallocated in an empty database
#define TABLE_SIZE1 64
#define TABLE_SIZE2 1024
// Max size of a table row
// max(sizeof(Equipment), sizeof(PeriodicReservation),
//     sizeof(OneTimeReservation), sizeof(StringMetadata),
//     sizeof(StringFragment))
#define MAX_STRUCT_SIZE 256
// Max length of a string fragment.
#define STRING_FRAGMENT_MAX 255

// Compile with `make clean test CFLAGS=-DDEBUG_REPO=1`
// to test reallocations.
#ifdef DEBUG_REPO
    #undef TABLE_SIZE1
    #define TABLE_SIZE1 DEBUG_REPO
    #undef TABLE_SIZE2
    #define TABLE_SIZE2 DEBUG_REPO
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

typedef enum {
    DefinedBeforeSemester = 0,
    DefinedDuringSemester,
    CanceledDuringSemester,
} PeriodicReservationType;

typedef struct {
    PeriodicReservationType type;
    Day day;
    HourAndMinute start;
    HourAndMinute end;
    Timestamp active_since;
    Timestamp active_until;
} PeriodicReservation;

typedef enum {
    Reservation = 0,
    ShadowingFree,
    ShadowingReservation,
} OneTimeReservationType;

typedef struct {
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

bool repo_get(Repo *repo, TableID table, ID id, void *dest);
void repo_set(Repo *repo, TableID table, ID id, void *src);
void repo_del(Repo *repo, TableID table, ID id);
void repo_del_n(Repo *repo, TableID table, ID id, unsigned n);
ID repo_len(Repo *repo, TableID table);
