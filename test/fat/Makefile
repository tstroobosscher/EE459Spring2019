###############################################################################
#
#	EE459 - fat test code
#
###############################################################################

PWD = $(shell pwd)
TEST_DIR = $(PWD)/../
TOP_DIR = $(TEST_DIR)../

CC = gcc -Wall

BINARY_FLAGS = -o

SOURCE = readfat.c $(TOP_DIR)/fat.c $(TOP_DIR)/utils.c

BINARY = readfat

all:
	$(CC) $(SOURCE) $(BINARY_FLAGS) $(BINARY)

clean:
	rm $(BINARY)
