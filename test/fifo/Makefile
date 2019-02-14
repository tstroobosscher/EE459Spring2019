###############################################################################
#
#	EE459 - fifo test
#
###############################################################################

PWD = $(shell pwd)
TEST_DIR = $(PWD)/../
TOP_DIR = $(TEST_DIR)../

CC = gcc -Wall

BINARY_FLAGS = -o

SOURCE = test.c $(TOP_DIR)/fifo.c

BINARY = fifotest

all:
	$(CC) $(SOURCE) $(BINARY_FLAGS) $(BINARY)

clean:
	rm $(BINARY)
