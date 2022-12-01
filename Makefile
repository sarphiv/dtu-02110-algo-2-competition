CC = gcc
LDFLAGS = -L.
LDLIBS =
VPATH = src

# CXXFLAGS = -g -Wall -Iinclude
# CXXFLAGS = -Wall -g -ggdb -fno-omit-frame-pointer -O3 -Iinclude
CXXFLAGS = -Wall -O3 -Iinclude

.PHONY: clean

main: obstacle-solver-1.o obstacle-solver-2.o obstacle-solver-3.o obstacle-solver-4.o flow-graph.o

clean:
	-$(RM) *.o main
