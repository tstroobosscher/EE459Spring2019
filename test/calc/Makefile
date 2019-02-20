###############################################################################
#
#	EE459 - calc test code
#
###############################################################################

PWD = $(shell pwd)
TEST_DIR = $(PWD)/../
TOP_DIR = $(TEST_DIR)../

CC = gcc -Wall

BINARY_FLAGS = -o

SOURCE = calc.c $(TOP_DIR)/fat.c

BINARY = calc

all:
	$(CC) $(SOURCE) $(BINARY_FLAGS) $(BINARY)

clean:
	rm $(BINARY)
