#pragma once
#include "Repo.h"

typedef struct LinkedList LinkedList;

LinkedList *linked_list_new();
void linked_list_add(LinkedList *list, void *item);
ID linked_list_into_array(LinkedList *list, int element_size, void **dest);
