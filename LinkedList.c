#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  void *data;
  struct Node *next;
} Node;

struct LinkedList {
  Node *start;
  Node *end;
  int len;
};

LinkedList *linked_list_new() {
  LinkedList *ret = malloc(sizeof(LinkedList));
  ret->start = NULL;
  ret->end = NULL;
  ret->len = 0;
  return ret;
}

void linked_list_add(LinkedList *list, void *item) {
  Node *node = malloc(sizeof(Node));
  node->data = item;
  node->next = NULL;
  if (list->len == 0) {
    list->start = node;
    list->end = node;
  } else {
    list->end->next = node;
    list->end = node;
  }
  list->len++;
}

ID linked_list_into_array(LinkedList *list, int element_size, void **dest) {
  void *arr = malloc(list->len * element_size);
  ID i = 0;
  Node *node = list->start;
  while (node != NULL) {
    memcpy(arr + i * element_size, node->data, element_size);
    i++;

    Node *old = node;
    node = node->next;
    free(old);
  }
  free(list);

  *dest = arr;
  return i;
}
