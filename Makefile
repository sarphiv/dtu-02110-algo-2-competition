CC = gcc -g
CXXFLAGS = -g -Wall -O3 -Iinclude
CFLAGS = -g -Wall
LDFLAGS = -L. -g
LDLIBS = 
# CC = gcc
# CXXFLAGS = -Wall -O3 -Iinclude
# CFLAGS = -Wall
# LDFLAGS = -L.
# LDLIBS = 

.PHONY: clean

main: obstacle-solver-1.o obstacle-solver-2.o obstacle-solver-3.o obstacle-solver-4.o graph-builder.o flow-graph.o

clean:
	-$(RM) *.o main