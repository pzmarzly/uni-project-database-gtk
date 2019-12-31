#include "RepoData.h"
#include "Utils.h"

GtkWidget* equipment_icon(EquipmentType type, unsigned size) {
    char *path = basedir();
    if (type == Projector) {
        strcat(path, "/icons/image-projector.png");
    } else if (type == Laptop) {
        strcat(path, "/icons/laptop.png");
    } else if (type == Whiteboard) {
        strcat(path, "/icons/white-board.png");
    } else {
        strcat(path, "/icons/more.png");
    }
    GError *error = NULL;
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_size(path, size, size, &error);
    free(path);
    // If pixbuf is NULL, gtk_image_new_from_pixbuf will handle it correctly.
    return gtk_image_new_from_pixbuf(pixbuf);
}
