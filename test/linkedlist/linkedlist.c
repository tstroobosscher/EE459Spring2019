/*
 *	USC EE459 Spring 2019 Team 17 - Linked List Test code
 */

#include <stdlib.h>
#include <stdio.h>

struct node {
	void  *data;

	struct node *next;
};

void push_head(struct node **head, void *data, size_t bytes) {

	struct node *new_node = (struct node *) malloc(sizeof(struct node));

	new_node->data = malloc(bytes);
	new_node->next = *head;

	for(int i = 0; i < bytes; i++) {
		/* dereference the value of the void pointer cast to char *, copy */
		*(char *)(new_node->data + i) = *(char *)(data + i);
	}

	(*head) = new_node;

}

void dump_list(struct node *ptr, void (*fptr)(void *)) {
	while(ptr) {
		(*fptr)(ptr->data);
		ptr = ptr->next;
	}
}

void print_int(void *t) {
	printf("%d\n", *(int *) t);
}

int main() {

	struct node *start = NULL;

	int size = sizeof(int);

	int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};

	for(int i = (sizeof(arr)/sizeof(int) - 1); i >= 0; i--) {
		push_head(&start, &arr[i], size);
	}

	printf("linked list created\n");
	dump_list(start, print_int);

}
