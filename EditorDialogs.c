#include "EditorDialogs.h"
#include "RepoData.h"

PreparedEditDialog editor_edit_dialog_prepare(TableID type) {
  return dialog_edit(editing_window_title(type));
}

bool editor_removal_dialog(TableID type, char *element_name) {
  return dialog_remove(removal_window_title(type), element_name);
}
