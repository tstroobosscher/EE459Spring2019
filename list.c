/*
 *	USC EE459 Spring 2019 Team 17 - Linked List Source
 */

#include <stddef.h>
#include <stdlib.h>

#include "list.h"

void list_push_head(struct node **head, void *data, size_t bytes) {

  struct node *new_node = (struct node *)malloc(sizeof(struct node));

  new_node->data = malloc(bytes);
  new_node->next = *head;

  for (int i = 0; i < bytes; i++) {
    /* dereference the value of the void pointer cast to char *, copy */
    *(char *)(new_node->data + i) = *(char *)(data + i);
  }

  (*head) = new_node;
}

void list_push_tail(struct node **head, void *data, size_t bytes) {

  struct node *new_node = (struct node *)malloc(sizeof(struct node));
  struct node *ptr = (*head);

  new_node->data = malloc(bytes);
  new_node->next = NULL;

  /* copy over the bytes to the new struct on the heap */
  for (int i = 0; i < bytes; i++) {
    /* dereference the value of the void pointer cast to char *, copy */
    *(char *)(new_node->data + i) = *(char *)(data + i);
  }

  /* if it's the first one */
  if(!ptr) {
  	(*head) = new_node;
  	return;
  }

  /* find the tail ptr */
  while(ptr->next) 
  	ptr = ptr->next;

  ptr->next = new_node;
  return;
}

void list_free(struct node *ptr) {
	while(ptr) {
		free(ptr);
		ptr = ptr->next;
	}
}

void list_dump(struct node *ptr, void (*fptr)(void *)) {
  while (ptr) {
    (*fptr)(ptr->data);
    ptr = ptr->next;
  }
}
