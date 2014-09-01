#Josh Deare
#CS344-400
#dearej@onid.orst.edu
#HW6

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -std=c99

PROGS = compute

all: $(PROGS)

compute.o: compute.c
	$(CC) $(CFLAGS) -c compute.c

compute: compute.o
	$(CC) $(CFLAGS) -o compute compute.o

clean:
	rm -f $(PROGS) *.o
