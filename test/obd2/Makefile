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

all:
	$(CC) $(SOURCE) $(BINARY_FLAGS) $(BINARY)

clean:
	rm $(BINARY)
