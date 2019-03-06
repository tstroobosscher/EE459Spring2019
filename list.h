/*
 *	USC EE459 Spring 2019 Team 17 - Linked List Header
 */

#ifndef LIST_H
#define LIST_H

#include <stddef.h>

struct node {
  void *data;

  struct node *next;
};

void list_push_head(struct node **head, void *data, size_t bytes);
void list_pust_tail(struct node **head, void *data, size_t bytes);
void list_free(struct node *ptr);
void list_dump(struct node *ptr, void (*fptr)(void *));

#endif
