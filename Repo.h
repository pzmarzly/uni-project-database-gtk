#include <stdbool.h>

typedef unsigned long long int Timestamp;
typedef unsigned short HourAndMinute;

typedef struct Repo Repo;
Timestamp repo_get_semester_start(Repo *repo);
Repo* repo_open(char *path, bool create, Timestamp semester_start);
void repo_close(Repo *repo);

// Data types
typedef unsigned int EquipmentID;
typedef unsigned int PeriodicReservationID;
typedef unsigned int OneTimeReservationID;
typedef unsigned int StringID;

typedef enum {
    Projector = 0,
    Laptop,
    Whiteboard,
    Other,
} EquipmentType;

typedef struct {
    EquipmentType type;
    char name[32];
    StringID description;
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
    StringID description;
} OneTimeReservation;

bool equipment_load(Repo *repo, EquipmentID id, Equipment *dest);
void equipment_save(Repo *repo, EquipmentID id, Equipment *src);
void equipment_remove(Repo *repo, EquipmentID id);
unsigned equipment_len(Repo *repo);

bool periodic_reservation_load(Repo *repo, PeriodicReservationID id, PeriodicReservation *dest);
void periodic_reservation_save(Repo *repo, PeriodicReservationID id, PeriodicReservation *src);
void periodic_reservation_remove(Repo *repo, PeriodicReservationID id);
unsigned periodic_reservation_len(Repo *repo);

bool one_time_reservation_load(Repo *repo, OneTimeReservationID id, OneTimeReservation *dest);
void one_time_reservation_save(Repo *repo, OneTimeReservationID id, OneTimeReservation *src);
void one_time_reservation_remove(Repo *repo, OneTimeReservationID id);
unsigned one_time_reservation_len(Repo *repo);

bool string_load(Repo *repo, StringID id, char **dest);
StringID string_save(Repo *repo, char **src);
void string_remove(Repo *repo, StringID id);
