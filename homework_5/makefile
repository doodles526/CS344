#Josh Deare
#CS344-400
#dearej@onid.orst.edu
#hw5
CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -std=c99 -lm 
PROGS = prime_mproc prime_pthread

all: $(PROGS)

prime_mproc.o: prime_mproc.c
	$(CC) $(CFLAGS) -lrt -c prime_mproc.c

prime_mproc: prime_mproc.o
	$(CC) $(CFLAGS) -lrt -o prime_mproc prime_mproc.o

prime_pthread.o: prime_pthread.c
	$(CC) $(CFLAGS) -pthread -c prime_pthread.c

prime_pthread: prime_pthread.o
	$(CC) $(CFLAGS) -pthread -o prime_pthread prime_pthread.o

clean:
	rm -f $(PROGS) *.o $(TEST_OUTPUT)
