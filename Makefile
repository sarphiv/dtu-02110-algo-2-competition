CC = gcc
CPPFLAGS = -Iinclude
CFLAGS = -Wall -O3
LDFLAGS = -L.
LDLIBS = 

.PHONY: clean

main: obstacle-solver-1.o obstacle-solver-2.o obstacle-solver-3.o obstacle-solver-4.o graph-builder.o flow-graph.o

clean:
	-$(RM) *.o main