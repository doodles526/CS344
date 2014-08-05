#define _POSIX_SOURCE
#define _GNU_SOURCE
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>

// longest word in english is 45 characters
// so we account for the \n termination at the end
#define MAX_WORD 46

void parser(int**, int);
int index_of_smallest(char**, int);
void merger(int**, int);

// parses words from stdin via scanf
// and outputs them roundrobin to all the pipes
void parser(int** pipes, int num_sorts)
{
	char word[MAX_WORD+1];
	int len_scan;
	int cur_pipe = 0;
	int i;
	// read words from stdin, and read the length of the scan
	while(scanf("%45[A-Za-z]%n%*[^A-Za-z]", word, &len_scan) != -1)
	{
		// convert to lower
		for(i = 0; i < len_scan; i++)
		{
			word[i] = tolower(word[i]);
		}
		// append newline
		word[len_scan] = '\n';
		// write our word to the current pipe
		if(write(pipes[cur_pipe][1], word, len_scan+1) != len_scan+1)
		{
			perror("Writing to pipe!");
			exit(1);
		}
		// increment the current pipe
		// the modulus will reset us at 0 
		// once we have covered all the pipes
		cur_pipe = (cur_pipe + 1) % num_sorts;
	}
}

int index_of_smallest(char** words, int num_words)
{
	char *temp = (char*)malloc(sizeof(char) * (MAX_WORD + 1));
	int ind = -1;
	int i = 0;

	// Find the first occurence of a non-null in words
	while(i < num_words && words[i][0] == '\0')
	{
//		printf("Index:%d is NULL\n", i);
		i++;
	}

	// Make sure there's at least 1 non-null word
	if(i >= num_words)
	{
		return ind;
	}

	temp = strndup(words[i], MAX_WORD+1);
	ind = i;
	// parse the rest of the list looking for comparisons
	for(; i < num_words; i++)
	{
		// ensure we don't process any NULL chars
		while(i < num_words && words[i][0] == '\0')
		{
			i++;
		}

		// In the case that we have to the end of
		// our words without finding a valid word
		// return the index
		if(i >= num_words)
		{
			return ind;
		}

		// compare the current lowest with the ith word
		if(strcmp(words[i], temp) < 0)
		{
			temp = strndup(words[i], MAX_WORD+1);
			ind = i;
		}
	}
	return ind;
}

// merges all the pipes and spits out some cool stuffs
void merger(int** pipes, int num_sorts)
{
	FILE ** files = (FILE**)malloc(sizeof(FILE*) * num_sorts);
	char** word_buffer = (char**)calloc(num_sorts, sizeof(char*));
	char* current_buffer = (char*)calloc(MAX_WORD + 1, sizeof(char));
	char line[43];
	int ind;
	int num_occurence = 1;
	int i;



	for(i = 0; i < num_sorts; i++)
	{
		word_buffer[i] = (char*)calloc((MAX_WORD + 1), sizeof(char));
	}


	// Setup the pipe read files for fgets later
	for(i = 0; i < num_sorts; i++)
	{
		if((files[i] = fdopen(pipes[i][0], "r")) == NULL)
		{
			perror("Couldn't open pipe file");
			exit(1);
		}
	}

	// initial population of word_buffer
	for(i = 0; i < num_sorts; i++)
	{
		// the reason for the double assignment is to ensure
		// word_buffer[i] is NULL on the EOF
		//if(fgets(word_buffer[i], MAX_WORD, files[i]) == NULL) 
		//{
		//	perror("opening read file for pipe");
		//	exit(1);
		//}
		if(fgets(line, sizeof(line), files[i]) == NULL)
		{
			word_buffer[i] = (char*)calloc(1, sizeof(char));
			word_buffer[i][0] = '\0';
			fclose(files[i]);
		}
		else
		{
			word_buffer[i] = strndup(line, MAX_WORD+1);
		}
	}

	//initialize the current_buffer for the while loop
	if((ind = index_of_smallest(word_buffer, num_sorts)) != -1)
	{
		current_buffer = strndup(word_buffer[ind], MAX_WORD+1);
		// pull down the next value
		if(fgets(line, sizeof(line), files[ind]) == NULL)
		{
			word_buffer[ind] = (char*)malloc(sizeof(char));
			word_buffer[ind][0] = '\0';
			fclose(files[ind]);
		}
		else
		{
			word_buffer[ind] = strndup(line, MAX_WORD+1);
		}
	}

	/* LOOK AT THE PRE_PULL AND HOW IT'R RELATED TO BUFFERS*/

	while((ind = index_of_smallest(word_buffer, num_sorts)) != -1)
	{
		// If we have seen this before
		if(strcmp(current_buffer, word_buffer[ind]) == 0)
		{
			// increment the num_occurence
			num_occurence++;
		}
		//otherwise print and update the buffer
		else
		{
			printf("%7d %s", num_occurence, current_buffer);
			current_buffer = strndup(word_buffer[ind], MAX_WORD+1);
			num_occurence = 1;
			fflush(stdout);
		}
		//Finally, replace the word in the array with the next
		//off the pipe
		if(fgets(line, sizeof(line), files[ind]) == NULL)
		{
			word_buffer[ind] = (char*)malloc(sizeof(char));
			word_buffer[ind][0] = '\0';
			fclose(files[ind]);
		}
		else
		{
			word_buffer[ind] = strndup(line, MAX_WORD+1);
		}
	}
	printf("%7d %s", num_occurence, current_buffer);
	fflush(stdout);
}

int main(int argc, char** argv)
{
	int num_sorts = 0;
	int c;
	int i;
	int h;
	int **parser_to_sorters;
	int **sorters_to_merger;

	if(argc < 2)
	{
		num_sorts = 1;
	}
	
	while((c = getopt(argc, argv, "n:")) != -1)
	{
		switch(c)
		{
			case 'n':
				num_sorts = atoi(optarg);
				if(num_sorts > 50)
				{
					num_sorts = 50;
				}
				break;
			case '?':
				printf("Usage: pipeline [-f outfile]\n");
				return 1;
			default:
				printf("Usage: pipeline [-f outfile]\n");
				return 1;
		}
	}
	if(num_sorts == 0)
	{
		num_sorts = 1;
	}
	// create all 2nd level pipe arrays
	parser_to_sorters = (int**)malloc(sizeof(int*) * num_sorts);
	sorters_to_merger = (int**)malloc(sizeof(int*) * num_sorts);

	// create all pipe arrays
	for(i = 0; i < num_sorts; i++)
	{
		parser_to_sorters[i] = (int*)malloc(sizeof(int) * 2);
		sorters_to_merger[i] = (int*)malloc(sizeof(int) * 2);
	
	}

	for(i = 0; i < num_sorts; i++)
	{
		// open the sorter receive pipe
		if(pipe(parser_to_sorters[i]) == -1)
		{
			perror("open parser_to_sorter pipe");
			exit(1);
		}
		//open merger receive pipe
		if(pipe(sorters_to_merger[i]) == -1)
		{
			perror("open sorters_to_merger pipe");
			exit(1);
		}

		switch(fork())
		{
			case -1:
				printf("BLAH");
				perror("forking sort");
				exit(1);
			case 0:
				// close pipe write to parser
				
				for(h = 0; h <= i; h++)
				{
					if(close(parser_to_sorters[h][1]) == -1)
					{
						perror("closing write to parser");
						exit(1);
					}
				}
				
				for(h = 0; h <= i; h++)
				{
					// close read from merger
					if(close(sorters_to_merger[h][0]) == -1)
					{
						perror("closing read from merger");
						exit(1);
					}
				}

				// link STDIN to the read end of pipe
				if(parser_to_sorters[i][0] != STDIN_FILENO)
				{
					if(dup2(parser_to_sorters[i][0], STDIN_FILENO) == -1)
					{
						perror("dup2 parser_to_sorter");
						exit(1);
					}
					if(close(parser_to_sorters[i][0]) == -1)
					{
						perror("close parser_to_sorter read");
						exit(1);
					}
				}

				// link STDOUT to the write end of pipe
				if(sorters_to_merger[i][1] != STDOUT_FILENO)
				{
					if(dup2(sorters_to_merger[i][1], STDOUT_FILENO) == -1)
					{
						perror("dup2 sorters_to_merger stdout");
						exit(1);
					}
					if(close(sorters_to_merger[i][1]) == -1)
					{
						perror("close sorters_to_merger write");
						exit(1);
					}
				}

				// start sort
				execlp("sort", "sort", (char*)NULL);
				perror("Didn't call sort");
				exit(1);
			default:
				break;
		}
		// We need to close the unused pipes
		// Which would be the read from parser_to_sorters
		// and the Write to sorters_to_merger
		if(close(parser_to_sorters[i][0]) == -1)
		{
			perror("closing parser_to_sorter read parent");
			exit(1);
		}
		if(close(sorters_to_merger[i][1]) == -1)
		{
			perror("closing sorters_to_merger write parent");
			exit(1);
		}

	}
	
	//fork the merge
	switch(fork())
	{
		case -1:
			printf("BLAH2");
			perror("forking merge");
			exit(1);
		case 0:
			// close everything related to the parser
			for(i = 0; i < num_sorts; i++)
			{
				if(close(parser_to_sorters[i][1]) == -1)
				{
					perror("close parter_to_sorter write");
					exit(1);
				}
			}
			// merger handles all it's own errors
			merger(sorters_to_merger, num_sorts);
			_exit(0);
			break;
		default:
			for(i = 0; i < num_sorts; i++)
			{
				if(close(sorters_to_merger[i][0]) == -1)
				{
					perror("closing sorters_to_merger read pnt");
					exit(1);
				}
			}
			break;
	}

	parser(parser_to_sorters, num_sorts);
	// close all parser_to_sorter write pipes
	for(i = 0; i < num_sorts; i++)
	{
		if(close(parser_to_sorters[i][1]) == -1)
		{
			perror("final parent pipe closing");
			exit(1);
		}
	}
	// wait for num_sorts+1, the +1 is for the merger child
	for(i = 0; i < num_sorts + 1; i++)
	{
		if(wait(NULL) == -1)
		{
			perror("reaping child");
			exit(1);
		}

	}
	return 0;
}
