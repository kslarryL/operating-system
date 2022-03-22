## This is a simple Makefile with lots of comments
## Check Unix Programming Tools handout for more info.

# Define what compiler to use and the flags.
CC=gcc
CXX=CC
CCFLAGS= -g -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Werror -lrt -lpthread

all: candykids

# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)
	$(CC) stats.c -c $(CCFLAGS)
	$(CC) bbuff.c -c $(CCFLAGS)
	$(CC) candykids.c -c $(CCFLAGS)


candykids: candykids.o bbuff.o stats.o
	$(CC) -o candykids candykids.o bbuff.o stats.o $(CCFLAGS)

clean:
	rm -f core *.o candykids bbuff stats
