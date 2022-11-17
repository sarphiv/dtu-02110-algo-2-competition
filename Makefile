CC = gcc -g
CXXFLAGS = -g -Wall -Iinclude
CFLAGS = -g -Wall
LDFLAGS = -L. -g
LDLIBS = 

.PHONY: clean

main: obstacle-solver-1.o obstacle-solver-2.o obstacle-solver-3.o obstacle-solver-4.o graph-builder.o flow-graph.o

clean:
	-$(RM) *.o main