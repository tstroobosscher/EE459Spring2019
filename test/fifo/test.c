#include <stdlib.h>
#include <stdio.h>

#include "../../fifo.h"
#include "../../utils.h"

int main() {

	struct fifo_t *fifo = malloc(sizeof(struct fifo_t));

	fifo_init(fifo);

	char dat[] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B,
		0x0C, 0x0D, 0x0E, 0x0F,
	};

	if(fifo_writen(fifo, dat, ARRAY_SIZE(dat)) < 0) {
		printf("fifo_writen: failure\n");
		return -1;
	}

	fifo_dump_buf(fifo);

	free(fifo);
	return 0;
}
