#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../fifo.h"
#include "../../utils.h"

int test_fifo_buf(struct fifo_t *fifo, char *exp, int size) {

	char *ret = malloc(size);

	free(ret);
	return 0;
}

int main() {

	struct fifo_t *fifo_ptr = malloc(sizeof(struct fifo_t));

	char dat[] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B,
		0x0C, 0x0D, 0x0E, 0x0F,
	};

	char *ret = malloc(sizeof(dat));

	initialize_fifo(fifo_ptr);

	/*************************************************************************\
	 *
	 *	head and tail should be 0, status should be empty
	 *
	\*************************************************************************/

	if(fifo_ptr->fifo_status == FIFO_EMPTY)
		printf("Test 1: Success, Fifo initialized empty\n");
	else
		printf("Test 1: failure, Fifo status: %d\n", fifo_ptr->fifo_status);

	if(fifo_writen(fifo_ptr, dat, ARRAY_SIZE(dat)) < 0) {
		printf("fifo_writen: failure\n");
		return -1;
	}

	fifo_dump_buf(fifo_ptr);

	free(fifo_ptr);
	free(ret);
	return 0;
}
