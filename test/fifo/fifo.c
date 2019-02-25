#ifdef DEBUG_86

void fifo_dump_buf(struct fifo_t *fifo, char *ret, int size) {
	/*
	 *	get buf values by reference, wont move the tail pointer
	 */
	memcpy(ret, fifo->f_buf, size);
}

void fifo_dump_mem(struct fifo_t *fifo, FILE *stream) {
	/* 
	 * dump the fifo struct contents 
	 */

	fprintf(stream, "fifo status: ");
	switch(fifo->f_status) {
		case FIFO_FULL	:
			fprintf(stream, "full\n");
			break;
		case FIFO_EMPTY	:
			fprintf(stream, "empty\n");
			break;
		case FIFO_READY	:
			fprintf(stream, "ready\n");
			break;
		default			:
			fprintf(stream, "unhandled status\n");
			break; 
	}

	for(int i = 0; i < ARRAY_SIZE(fifo->f_buf); i++){
		fprintf(stream, "fifo address: %02d, data: 0x%02x", i, 
			fifo->f_buf[i]);
		if(fifo->f_head == i)
			fprintf(stream, " <- head");
		if(fifo->f_tail == i)
			fprintf(stream, " <- tail");

		fprintf(stream, "\n");
	}


	return;
}

#endif