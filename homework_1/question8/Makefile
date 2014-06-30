#
#  Josh Deare
#  CS311-400
#  dearej@onid.orst.edu
#  Homework 1
#

CC = gcc
DEBUG = -g
CFLAGS = $(DEBUG) -Wall -lm
PROGS = primes

ALL:  $(PROGS)

primes: primes.o
	$(CC) $(CFLAGS) -o primes primes.o

primes.o: primes.c
	$(CC) $(CFLAGS) -c primes.c

clean:
	rm -f $(PROGS) *.o *~ 
