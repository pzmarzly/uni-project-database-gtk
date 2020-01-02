#pragma once
#include <gtk/gtk.h>
#include "Repo.h"

GtkWidget* equipment_icon(EquipmentType type, unsigned size);
char* removal_and_editing_text(TableID type);
