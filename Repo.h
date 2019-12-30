#include <stdbool.h>

typedef unsigned long long int Timestamp;
typedef unsigned short HourAndMinute;

typedef struct Repo Repo;
Timestamp repo_get_semester_start(Repo *repo);
Repo* repo_open(char *path, bool override, Timestamp semester_start);
void repo_close(Repo *repo);

// Data types
typedef unsigned int ID;
typedef unsigned int String;

#define TABLE_NUM 4
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

typedef struct {
    int len;
    char data[256];
} StringFragment;

bool repo_get(Repo *repo, TableID table, ID id, void *dest);
void repo_set(Repo *repo, TableID table, ID id, void *src);
void repo_del(Repo *repo, TableID table, ID id);
unsigned repo_len(Repo *repo, TableID table);
