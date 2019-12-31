#pragma once
#include <gtk/gtk.h>
#include "Repo.h"

typedef struct EditorEquipment EditorEquipment;
EditorEquipment* editor_equipment_new(Repo *repo);
void editor_equipment_show(EditorEquipment *eq, GtkBox *target);
