CC = gcc
CPPFLAGS = 
CFLAGS = -Wall -O3
LDFLAGS = -L.
LDLIBS = 

.PHONY: clean

main: common.o max-flow.o obstacle-1.o obstacle-4.o

clean:
	-$(RM) *.o main