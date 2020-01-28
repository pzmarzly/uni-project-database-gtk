#pragma once
#include "Editor.h"
#include "Repo.h"
#include <gtk/gtk.h>

typedef struct EditorEquipment EditorEquipment;
EditorEquipment *editor_equipment_new(Editor *editor, Repo *repo,
                                      GtkBuilder *ui);
void editor_equipment_prepare(EditorEquipment *this);
void editor_equipment_refresh(EditorEquipment *this);
