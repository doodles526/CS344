
CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -std=c99 
PROGS = pipeline
TEST_OUTPUT = outfile1-0 outfile1-1 outfile2-0 outfile2-1

all: $(PROGS)

pipeline.o: pipeline.c
	$(CC) $(CFLAGS) -c pipeline.c

pipeline: pipeline.o
	$(CC) $(CFLAGS) -o pipeline pipeline.o

test: pipeline
	rm -f outfile2-0 outfile1-0 outfile2-1 outfile1-1
	pipeline -f outfile2-0 < /usr/local/classes/eecs/summer2014/cs344-400/src/Homework4/Problem1/words.txt > outfile1-0
	bash /usr/local/classes/eecs/summer2014/cs344-400/src/Homework4/Problem1/pipeline.bash -f outfile2-1 < /usr/local/classes/eecs/summer2014/cs344-400/src/Homework4/Problem1/words.txt > outfile1-1
	diff outfile1-1 outfile1-0
	diff outfile2-1 outfile2-0

clean:
	rm -f $(PROGS) *.o $(TEST_OUTPUT)
