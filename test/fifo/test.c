#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../fifo.h"
#include "../../utils.h"

extern FILE *stdout;

struct fifo_t fifo;

int cmp_fifo_buf(struct fifo_t *fifo, char *exp, int size) {
	/*
	 *	dump buf values and compare to expected buf, ret 0 if same
	 */

	char *ret = malloc(size);
	fifo_dump_buf(fifo, ret, size);
	if(memcmp(ret, exp, size))
		return -1;

	free(ret);
	return 0;
}

int main() {

	struct fifo_t *fifo_ptr = &fifo;

	char ret[FIFO_SIZE];
	char dat[FIFO_SIZE] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B,
		0x0C, 0x0D, 0x0E, 0x0F,
	};

	///////////////////////////////////////////////////////////////////////////
	//
	//	Test 1: head and tail should be 0, status should be empty
	//
	///////////////////////////////////////////////////////////////////////////

	/* stimulus */
	initialize_fifo(fifo_ptr);

	if((fifo_ptr->f_status == FIFO_EMPTY) && 
		(fifo_ptr->f_head == 0) && 
		(fifo_ptr->f_tail == 0))
		printf("Test 1: Success, Fifo initialized empty, head and tail set to "
			"zero\n");
	else
		printf("Test 1: failure, Fifo status: %d\n", fifo_ptr->f_status);

	///////////////////////////////////////////////////////////////////////////
	//
	//	Test 2: fifo should have all the index set to their natural values,
	//	and should be full, head and tail at 0
	//
	///////////////////////////////////////////////////////////////////////////

	/* stimulus */
	if(fifo_writen(fifo_ptr, dat, FIFO_SIZE) < 0) {
		printf("fifo_writen: failure\n");
		return -1;
	}

	if((!cmp_fifo_buf(fifo_ptr, dat, FIFO_SIZE)) && 
		(fifo_ptr->f_status == FIFO_FULL) &&
		(fifo_ptr->f_head == 0) && 
		(fifo_ptr->f_tail == 0))
		printf("Test 2: Success, Fifo values all correct\n");
	else
		printf("Test 2: Failure, Fifo values incorrect\n");

	///////////////////////////////////////////////////////////////////////////
	//
	//	Test 3: fifo should have all the correct values but should be empty now
	//
	///////////////////////////////////////////////////////////////////////////

	/* stimulus */
	if(fifo_readn(fifo_ptr, ret, FIFO_SIZE) < 0) {
		printf("fifo_readn: failure\n");
		return -1;
	}

	if((!cmp_fifo_buf(fifo_ptr, dat, FIFO_SIZE)) && 
		(fifo_ptr->f_status == FIFO_EMPTY) &&
		(fifo_ptr->f_head == 0) && 
		(fifo_ptr->f_tail == 0))
		printf("Test 3: Success, Fifo values all correct\n");
	else
		printf("Test 3: Failure, Fifo values incorrect\n");

	fifo_dump_mem(fifo_ptr, stdout);

	return 0;
}
