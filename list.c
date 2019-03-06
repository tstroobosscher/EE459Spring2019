/*
 *	USC EE459 Spring 2019 Team 17 - Linked List Source
 */

#include "list.h"

void push_head(struct node **head, void *data, size_t bytes) {

  struct node *new_node = (struct node *)malloc(sizeof(struct node));

  new_node->data = malloc(bytes);
  new_node->next = *head;

  for (int i = 0; i < bytes; i++) {
    /* dereference the value of the void pointer cast to char *, copy */
    *(char *)(new_node->data + i) = *(char *)(data + i);
  }

  (*head) = new_node;
}
