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
typedef unsigned int String;

// Number of vector-like tables.
#define TABLE_NUM 4
// Amount of rows preallocated in an empty database
#define TABLE_SIZE1 64
#define TABLE_SIZE2 1024
// Max size of a table row
// max(sizeof(Equipment), sizeof(PeriodicReservation),
//     sizeof(OneTimeReservation), sizeof(StringFragment))
#define MAX_STRUCT_SIZE 1024
// Max length of a string fragment.
#define STRING_FRAGMENT_MAX 255

// Compile with `make clean test CFLAGS=-DDEBUG_REPO`
// to test reallocations.
#ifdef DEBUG_REPO
    #undef TABLE_SIZE1
    #define TABLE_SIZE1 1
    #undef TABLE_SIZE2
    #define TABLE_SIZE2 1
    #undef STRING_FRAGMENT_MAX
    #define STRING_FRAGMENT_MAX 1
#endif

typedef enum {
    TableEquipment = 0,
    TablePeriodicReservation,
    TableOneTimeReservation,
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
    String description;
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
    Day day;
    HourAndMinute start;
    HourAndMinute end;
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
    String description;
} OneTimeReservation;

// Magic number (goes into len) to symbolize more data awaiting.
#define STRING_FRAGMENT_MORE 1024
// Magic number (goes into len) to symbolize deleted element.
#define STRING_FRAGMENT_TOMBSTONE 2048
typedef struct {
    String string;
    short len;
    char data[STRING_FRAGMENT_MAX + 1];
} StringFragment;

bool repo_get(Repo *repo, TableID table, ID id, void *dest);
void repo_set(Repo *repo, TableID table, ID id, void *src);
void repo_del(Repo *repo, TableID table, ID id);
unsigned repo_len(Repo *repo, TableID table);
