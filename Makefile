CC = gcc -g
CXXFLAGS = -g -Wall -Iinclude
CFLAGS = -g -Wall
LDFLAGS = -g -L.
LDLIBS =
# CC = gcc
# CXXFLAGS = -Wall -O3 -Iinclude
# CFLAGS = -Wall
# LDFLAGS = -L.
# LDLIBS = 

.PHONY: clean

main: obstacle-solver-1.o obstacle-solver-2.o obstacle-solver-3.o obstacle-solver-4.o flow-graph.o

clean:
	-$(RM) *.o main
