#pragma once
#include "Repo.h"
#include <gtk/gtk.h>

GtkWidget *equipment_icon(EquipmentType type, unsigned size);
char *removal_and_editing_text(TableID type);
