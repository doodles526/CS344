#define _POSIX_SOURCE
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char** argv)
{
	char* outfile;
	int c;
	
	int rev_to_sort[2];
	int sort_to_uniq[2];
	int uniq_to_tee[2];
	int tee_to_wc[2];
	
	if(argc < 2)
	{
		printf("Usage: pipeline [-f outfile]\n");
		return 0;
	}
	
	while((c = getopt(argc, argv, "f:")) != -1)
	{
		switch(c)
		{
			case 'f':
				outfile = optarg;
				break;
			case '?':
				printf("Usage: pipeline [-f outfile]\n");
				break;
			default:
				printf("Usage: pipeline [-f outfile]\n");
				break;
		}
	}

	//create pipe tee->wc
	//needs to stay open through execution of tee fork
	if(pipe(tee_to_wc) == -1)
	{
		perror("Creating pipe tee_to_wc");
		exit(1);
	}

	// fork and start wc
	switch(fork())
	{
		case -1:
			perror("Fork wc");
			exit(1);

		case 0: // kill read pipe
			if(close(tee_to_wc[1]) == -1)
			{
				perror("Close Write tee pipe");
				exit(1);
			}
			
			if(tee_to_wc[0] != STDIN_FILENO)
			{
				if(dup2(tee_to_wc[0], STDIN_FILENO) == -1)
				{
					perror("Assigning stdin to tee pipe");
					exit(1);
				}
				if(close(tee_to_wc[0]) == -1)
				{
					perror("Closing write pipe uniq");
					exit(1);
				}
			}

			execlp("wc", "wc", (char*)NULL);
			perror("Didnt call rev");
			exit(1);
		
		default:
			break;
	}








	//create pipe uniq->tee
	//needs to stay open through execution of uniq fork
	if(pipe(uniq_to_tee) == -1)
	{
		perror("creating uniq_to_tee pipe");
		exit(1);
	}


	// fork and start tee 
	switch(fork())
	{
		case -1:
			perror("Fork tee");
			exit(1);

		case 0: // kill read pipe
			if(close(uniq_to_tee[1]) == -1)
			{
				perror("Close Write uniq pipe");
				exit(1);
			}
			
			if(close(tee_to_wc[0]) == -1)
			{
				perror("Close read wc pipe");
				exit(1);
			}

			if(uniq_to_tee[0] != STDIN_FILENO)
			{
				if(dup2(uniq_to_tee[0], STDIN_FILENO) == -1)
				{
					perror("Assigning stdout to tee pipe");
					exit(1);
				}
				if(close(uniq_to_tee[0]) == -1)
				{
					perror("Closing write pipe uniq");
					exit(1);
				}
			}

			if(tee_to_wc[1] != STDOUT_FILENO)
			{
				if(dup2(tee_to_wc[1], STDOUT_FILENO) == -1)
				{
					perror("Assigning STDOUT to tee");
					exit(1);
				}
				if(close(tee_to_wc[1]) == -1)
				{
					perror("closing uniq_to_tee[1]");
					exit(1);
				}
			}

			execlp("tee", "tee", outfile, (char*)NULL);
			perror("Didnt call rev");
			exit(1);
		
		default:
			break;
	}

	// close tee->wc
	if(close(tee_to_wc[0]) == -1)
	{
		perror("closing pipe tee_to_wc");
		exit(1);
	}
	if(close(tee_to_wc[1]) == -1)
	{
		perror("closing pipe tee to wc");
		exit(1);
	}








	//create pipe uniq->tee
	//needs to stay open through exec of uniq
	if(pipe(sort_to_uniq) == -1)
	{
		perror("creating sort_to_uniq");
		exit(1);
	}

	// fork and start uniq
	switch(fork())
	{
		case -1:
			perror("Fork uniq");
			exit(1);

		case 0: // kill read pipe
			if(close(sort_to_uniq[1]) == -1)
			{
				perror("Close Write sort pipe");
				exit(1);
			}
			
			if(close(uniq_to_tee[0]) == -1)
			{
				perror("Close read tee pipe");
				exit(1);
			}

			if(sort_to_uniq[0] != STDIN_FILENO)
			{
				if(dup2(sort_to_uniq[0], STDIN_FILENO) == -1)
				{
					perror("Assigning stdout to rev pipe");
					exit(1);
				}
				if(close(sort_to_uniq[0]) == -1)
				{
					perror("Closing write pipe");
					exit(1);
				}
			}

			if(uniq_to_tee[1] != STDOUT_FILENO)
			{
				if(dup2(uniq_to_tee[1], STDOUT_FILENO) == -1)
				{
					perror("Assigning STDOUT to tee");
					exit(1);
				}
				if(close(uniq_to_tee[1]) == -1)
				{
					perror("closing uniq_to_tee[1]");
					exit(1);
				}
			}

			execlp("uniq", "uniq", "-c", (char*)NULL);
			perror("Didnt call rev");
			exit(1);
		
		default:
			break;
	}

	//close uniq->tee
	if(close(uniq_to_tee[0]) == -1)
	{
		perror("closing uniq_to_tee");
		exit(1);
	}
	if(close(uniq_to_tee[1]) == -1)
	{
		perror("closing uniq_to_tee");
		exit(1);
	}








	//create pipe rev-> sort 
	if(pipe(rev_to_sort) == -1)
	{
		perror("creating sort_to_uniq");
		exit(1);
	}

	// fork and start sort 
	switch(fork())
	{
		case -1:
			perror("Fork cat");
			exit(1);

		case 0: // kill write pipe
			if(close(rev_to_sort[1]) == -1)
			{
				perror("Close read rev pipe");
				exit(1);
			}
			
			//close read pipe
			if(close(sort_to_uniq[0]) == -1)
			{
				perror("Close read uniq pipe");
				exit(1);
			}

			// assign pipe in from rev to STDIN
			if(rev_to_sort[0] != STDIN_FILENO)
			{
				if(dup2(rev_to_sort[0], STDIN_FILENO) == -1)
				{
					perror("Assigning stdin to rev-sort pipe");
					exit(1);
				}
				if(close(rev_to_sort[0]) == -1)
				{
					perror("Closing read rev pipe");
					exit(1);
				}
			}

			if(sort_to_uniq[1] != STDOUT_FILENO)
			{
				if(dup2(sort_to_uniq[1], STDOUT_FILENO) == -1)
				{
					perror("Assigning stdout to sort-uniq pipe");
					exit(1);
				}
				if(close(sort_to_uniq[1]) == -1)
				{
					perror("Closing write to sort-unique");
					exit(1);
				}
			}

			execlp("sort", "sort", (char*)NULL);
			perror("Didnt call rev");
			exit(1);

		default:
			break;
	}

	//close sort->uniq
	if(close(sort_to_uniq[0]) == -1)
	{
		perror("closing sort_to_uniq");
		exit(1);
	}
	if(close(sort_to_uniq[1]) == -1)
	{
		perror("closing sort_to_uniq");
		exit(1);
	}







	// execute the first in chain last -rev
	switch(fork())
	{
		case -1:
			perror("Fork cat");
			exit(1);

		case 0: // kill read pipe
			if(close(rev_to_sort[0]) == -1)
			{
				perror("Close Write rev pipe");
				exit(1);
			}

			if(rev_to_sort[1] != STDOUT_FILENO)
			{
				if(dup2(rev_to_sort[1], STDOUT_FILENO) == -1)
				{
					perror("Assigning stdout to rev pipe");
					exit(1);
				}
				if(close(rev_to_sort[1]) == -1)
				{
					perror("Closing write pipe");
					exit(1);
				}
			}

			execlp("rev", "rev", (char*)NULL);
			perror("Didnt call rev");
			exit(1);
		default:
			break;
	}
	
	//close rev_to_sort pipe
	if(close(rev_to_sort[0]) == -1)
	{
		perror("close rev_to_sort 0");
		exit(1);
	}
	if(close(rev_to_sort[1]) == -1)
	{
		perror("close rev_to_sort 1");
		exit(1);
	}


	if(wait(NULL) == -1)
	{
		perror("wait 1");
		exit(1);
	}
	if(wait(NULL) == -1)
	{
		perror("wait 1");
		exit(1);
	}
	if(wait(NULL) == -1)
	{
		perror("wait 1");
		exit(1);
	}
	if(wait(NULL) == -1)
	{
		perror("wait 1");
		exit(1);
	}
	if(wait(NULL) == -1)
	{
		perror("wait 1");
		exit(1);
	}
	return 0;
}
