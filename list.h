/*
 *	USC EE459 Spring 2019 Team 17 - Linked List Header
 */

#ifndef LIST_H
#define LIST_H

struct node {
  void *data;

  struct node *next;
};

void push_head(struct node **head, void *data, size_t bytes);

#endif
