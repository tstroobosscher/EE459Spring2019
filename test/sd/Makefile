###############################################################################
#
#	EE459 - sd memory test
#
###############################################################################

PWD = $(shell pwd)
TEST_DIR = $(PWD)/../
TOP_DIR = $(TEST_DIR)../

CC = gcc -Wall

BINARY_FLAGS = -o

SOURCE = mem_read.c

BINARY = mem_read

all:
	$(CC) $(SOURCE) $(BINARY_FLAGS) $(BINARY)

clean:
	rm $(BINARY)
