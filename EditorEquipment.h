#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorEquipment EditorEquipment;
EditorEquipment *editor_equipment_new(Repo *repo, GtkBuilder *ui);
void editor_equipment_show(EditorEquipment *this);
