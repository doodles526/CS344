#
#  $RCSfile$
#  $Revision$
#  $Author$
#  $Date$
#  $Log$
#

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall
PROGS=check_vowel helloworld


all: $(PROGS)


check_vowel: check_vowel.o
	$(CC) $(CFLAGS) -o check_vowel check_vowel.o

check_vowel.o: check_vowel.c
	$(CC) $(CFLAGS) -c check_vowel.c

helloworld: helloworld.o
	$(CC) $(CFLAGS) -o helloworld helloworld.o

helloworld.o: helloworld.c
	$(CC) $(CFLAGS) -c helloworld.c


clean:
	rm -f $(PROGS) *.o *~

