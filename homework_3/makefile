CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -std=c99
PROGS = sig_demo myar
TESTS = unittest1

all: $(PROGS)
tests: $(TESTS)

sig_demo: sig_demo.o
	$(CC) $(CFLAGS) -o sig_demo sig_demo.o

sig_demo.o: sig_demo.c
	$(CC) $(CFLAGS) -c sig_demo.c

myar: myar.o
	$(CC) $(CFLAGS) -o myar myar.o

myar.o: myar.h myar.c
	$(CC) $(CFLAGS) -c myar.c

unittest1: unittest1.c myar.o fixtures.o
	$(CC) $(CFLAGS) -o unittest1 unittest1.c myar.o

unittest2: unittest2.c myar.o fixtures.o
	$(CC) $(CFLAGS) -o unittest2 unittest2.c myar.o

fixtures.o: fixtures.c 
	$(CC) $(CFLAGS) -c fixtures.c

clean:
	rm -f *.o *~ $(PROGS) $(TESTS)
