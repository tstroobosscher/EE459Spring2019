###############################################################################
#
#	EE459 - obd test code
#
###############################################################################

PWD = $(shell pwd)
TEST_DIR = $(PWD)/../
TOP_DIR = $(TEST_DIR)../

CC = gcc -Wall

BINARY_FLAGS = -o

SOURCE = obd.c

BINARY = obd

LINKER_FLAGS = 

all:
	$(CC) $(SOURCE) $(LINKER_FLAGS) $(BINARY_FLAGS) $(BINARY)

clean:
	rm $(BINARY)
