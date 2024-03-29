/*
 *Josh Deare
 *CS344-400
 *dearej@onid.orst.edu
 *hw5
 *
 */

#define _POSIX_SOURCE
#define _BSD_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>

#define MSG_LEN 300
#define TWIN_STR 15

#define TestBit(x,i) ((x[i>>3] & (1<<(i&7)))!=0)
#define SetBit(x,i) x[i>>3]|=(1<<(i&7));
#define ClearBit(x,i) x[i>>3]&=(1<<(i&7))^0xFF;

/*
#define TestBit(x,i) ((x[i>>3] & (1<<(7-(i&7))))!=0)
#define SetBit(x,i) x[i>>3]|=(1<<(7-(i&7)));
#define ClearBit(x,i) x[i>>3]&=(1<<(7-(i&7)))^0xFF;
*/
unsigned int odd_num_to_bit_array(unsigned int);
void* printer(void*);
unsigned int bit_array_to_odd_num(unsigned int);
unsigned int get_index_offset(unsigned int, unsigned int);
void* worker(void*);
void master(unsigned char*, int, unsigned int, int**, int*, int);

/*
 * Because we know all even digits 
 * except 2 are not prime, we can cut
 * out memory footprint in half.
 * In order to do so, we need to make a mapping of 
 * odd numbers to our bit-array
 *
 * Note: this approach allows also allows us not need
 * the use of locks on our bit array if we slightly increase
 * the size of our halved array
 */
unsigned int odd_num_to_bit_array(unsigned int n)
{
	return (n-1)/2;
}

unsigned int bit_array_to_odd_num(unsigned int n)
{
	return (n * 2) + 1;
}

struct printer_args
{
	int** pipes;
	int num_markers;
	int quiet;
};

void* printer(void* arg)
{
	struct printer_args* pargs = (struct printer_args*)arg;
	int** pipes = pargs->pipes;
	int num_markers = pargs->num_markers;
	int quiet = pargs->quiet;

	int i;
	char* buffer1 = (char*)malloc(sizeof(char) * TWIN_STR);
	char* buffer2 = (char*)malloc(sizeof(char) * TWIN_STR);
	int num_read;
	if(quiet)
	{
		for(i = 0; i < num_markers; i++)
		{
			while((num_read = read(pipes[i][0], buffer1, TWIN_STR)) == TWIN_STR)
			{
				//EOF checker
				if((num_read = read(pipes[i][0], buffer2, TWIN_STR)) == 0)
				{
					break;
				}
				else if(num_read == -1)
				{
					perror("Error reading pipe fr prn");
					exit(1);
				}
			}/*
			if(num_read == -1)
			{
				perror("Error reading pipe fr prn");
				exit(1);
			}*/
			if(close(pipes[i][0]) == -1)
			{
				perror("Error closing pipe fr prn");
				exit(1);
			}
		}
	}
	else
	{
		for(i = 0; i < num_markers; i++)
		{
			while((num_read = read(pipes[i][0], buffer1, TWIN_STR)) == TWIN_STR)
			{
				if((num_read = read(pipes[i][0], buffer2, TWIN_STR)) == TWIN_STR)
				{
				printf("%u, %u\n",
					(unsigned int)strtoul(buffer1, NULL, 10),
					(unsigned int)strtoul(buffer2, NULL, 10));
				}
				// EOF
				else if(num_read == 0)
				{
					break;
				}
				else
				{
					perror("Error reading pipe fr prn");
					exit(1);
				}
			}
			if(num_read == -1)
			{
				perror("Error reading pipe fr prn");
				exit(1);
			}
			if(close(pipes[i][0]) == -1)
			{
				perror("Error closing pipe fr prn");
				exit(1);
			}
		}
	}
	fflush(stdout);
	return NULL;
}

unsigned int get_index_offset(unsigned int base, unsigned int factor)
{
	return (factor - 1) - (base - 1) % factor;
}

struct worker_args 
{
	unsigned char* bm_base;
	unsigned int base;
	unsigned int bits;
	unsigned int num_analyze;
	int* pipe_from_master;
	int* pipe_to_printer;
};

void* worker(void* arg)
{
	struct worker_args* wargs = (struct worker_args*)arg;
	unsigned char* bm_base = wargs->bm_base;
	unsigned int base = wargs->base;
	unsigned int bits = wargs->bits;
	unsigned int num_analyze = wargs->num_analyze;
	int* pipe_from_master = wargs->pipe_from_master;
	int* pipe_to_printer = wargs->pipe_to_printer;

	int num_read;
	int i;
	char* prime_buf = (char*)malloc(sizeof(char) * TWIN_STR);
	unsigned int prime;
	unsigned int offset;
	// initialize the shmem segment we are responsible for to 1(prime)
	for(i = 0; i < bits; i++)
	{
		SetBit(bm_base, i);	
	}
	
	while((num_read = read(pipe_from_master[0], prime_buf, TWIN_STR)) > 0)
	{
		prime = (unsigned int)strtoul(prime_buf, NULL, 10);
		
		// determine the first offset
		offset = get_index_offset(base, prime);
		if((offset + base)% 2 == 0 )
		{
			offset += prime;
		}
		
		// mark all multiples of the prime from the offset
		// But since all the primes(except 2 which we will not process)
		// are even, we will simply skip the even multiples of the prime
		for(i = offset; i < num_analyze; i += (2*prime))
		{
			if(base%2 == 0)
			{
				ClearBit(bm_base, odd_num_to_bit_array(i));
			}
			else
			{
				ClearBit(bm_base, i/2);
			}
		}
	}
	if(num_read == -1)
	{
		perror("worker reading from pipe");
		_exit(1);
	}

	if(close(pipe_from_master[0]) == -1)
	{
		perror("closing read");
		_exit(1);
	}
	
	offset = get_index_offset(base, 6);
	//fprintf(stdout, "%d\n",offset);
	if(base % 2 == 0)
	{

		for(i = offset; i < num_analyze; i+=6)
		{
			if(TestBit(bm_base, odd_num_to_bit_array(i-1)) == 1 
			   && TestBit(bm_base, (i/2)) == 1)
			{
				sprintf(prime_buf, "%15u", base+(i-1));
				if(write(pipe_to_printer[1], prime_buf, TWIN_STR) == -1)
				{
					printf("ISSUE1\n");
					fflush(stdout);
					perror("writing to pipe");
					exit(1);
				}
				sprintf(prime_buf, "%15u", base+(i+1));
				if(write(pipe_to_printer[1], prime_buf, TWIN_STR) == -1)
				{
					perror("writing to pipe");
					exit(1);
				}
		//		fprintf(stdout, "%u, %u\n", base+(i-1),
		//					    base+(i+1));
			
			}
		}
		/*
		for(i = 0; i < bits; i++)
		{
			if(TestBit(bm_base, i) == 1)
			{
			//		fprintf(stdout, "%u\n", base + bit_array_to_odd_num(i));
				sprintf(prime_buf, "%15u", base+bit_array_to_odd_num(i));
				if(write(pipe_to_printer[1], prime_buf, TWIN_STR) == -1)
				{
					perror("writing to pipe");
					exit(1);
				}
			}
		}*/
	}
	else
	{
		for(i = offset; i < num_analyze; i+=6)
		{
			if(TestBit(bm_base, ((i+1)/2)) == 1
			   && TestBit(bm_base, odd_num_to_bit_array(i)) == 1)
			{
				
				sprintf(prime_buf, "%15u", base+(i-1));
				if(write(pipe_to_printer[1], prime_buf, TWIN_STR) == -1)
				{
					fflush(stdout);
					perror("writing to pipe");
					exit(1);
				}
				sprintf(prime_buf, "%15u", base+(i+1));
				if(write(pipe_to_printer[1], prime_buf, TWIN_STR) == -1)
				{
					perror("writing to pipe");
					exit(1);
				}
		//		fprintf(stdout, "%u, %u\n", base+(i-1),
		//					    base+(i+1));
			}
		}
		/*	
		for(i = 0; i < bits; i++)
		{
			if(TestBit(bm_base, i) == 1)
			{
			//		fprintf(stdout, "%u\n", base + i*2);
				sprintf(prime_buf, "%15u", base+i*2);
				if(write(pipe_to_printer[1], prime_buf, TWIN_STR) == -1)
				{
					perror("writing to pipe");
					exit(1);
				}
			}
		}*/
	}
	if(close(pipe_to_printer[1]) == -1)
	{
		perror("closing write");
		_exit(1);
	}
	return NULL;
}

void master(unsigned char* bitmap, int num_bits, unsigned int top_num, int** master_to_workers, int* pipe_to_printer, int num_workers)
{
	int i;
	int j;
	unsigned int max_send = (unsigned int)floor(sqrt(top_num));
	char* buffer = (char*)malloc(sizeof(char) * TWIN_STR);

	// initialize shmem segment for master
	for(i = 0; i < num_bits; i++)
	{
		SetBit(bitmap, i);
	}

	// Find/mark all primes and send all under sqrt(top_num)
	// to the workers to mark stuffs
	for(i = 1; i < num_bits; i++)
	{
		if(TestBit(bitmap, i) == 1)
		{
			for(j = bit_array_to_odd_num(i) * 3; 
				j < bit_array_to_odd_num(num_bits);
			    	j += bit_array_to_odd_num(i)*2)
			{
				ClearBit(bitmap, odd_num_to_bit_array(j));
			}
			if(bit_array_to_odd_num(i) <= max_send)
			{
				sprintf(buffer, "%15u", bit_array_to_odd_num(i));
				for(j = 0; j < num_workers; j++)
				{
					if(write(master_to_workers[j][1], buffer, TWIN_STR) == -1)
					{
						perror("Error writing to pipe");
						exit(1);
					}
				}
			}
		}
	}
	for(i = 0; i < num_workers; i++)
	{
		if(close(master_to_workers[i][1]) == -1)
		{
			perror("closing mtw write");
			exit(1);
		}
	}
	for(i = 1; i < num_bits - 1; i++)
	{
		if((TestBit(bitmap, i) == 1) && (TestBit(bitmap, (i+1)) == 1))
		{
			if(sprintf(buffer, "%15u", bit_array_to_odd_num(i)) == -1)
			{
				perror("sprinting");
				exit(1);
			}
			if(write(pipe_to_printer[1], buffer, TWIN_STR) == -1)
			{
				perror("writing to pipe_printer");
				exit(1);
			}
			if(sprintf(buffer, "%15u", bit_array_to_odd_num(i+1)) == -1)
			{
				perror("sprinting");
				exit(1);
			}
			if(write(pipe_to_printer[1], buffer, TWIN_STR) == -1)
			{
				perror("writing to pipe_printer");
				exit(1);
			}

		}
	}
	if(close(pipe_to_printer[1]) == -1)
	{
		perror("closing ptp write");
		exit(1);
	}
}

int main(int argc, char** argv)
{
	int c;
	int i;

	unsigned char* bitmap;
	unsigned char* offset_bitmap;
	int bitmap_size = 0;

	double num_master_analyze;
	double num_children_analyze;
	double num_per_child;
	double num_last;
	
	unsigned int* adjusted_size;
	unsigned int* adjusted_analyze;
	unsigned int* bits_per_segment;

	int** markers_to_printer;
	int** master_to_workers;

	struct worker_args* wargs_list;
	struct printer_args pargs;

	int quiet = 0;
	unsigned int max_size = UINT_MAX;
	int num_conc = 1;

	pthread_t* thread;

	unsigned int current_upper = 0;
	unsigned int current_base = 0;


	while((c = getopt(argc, argv, "qmc:")) != -1)
	{
		switch(c)
		{
			case 'q':
				quiet = 1;
				break;

			case 'm':
				max_size = (unsigned int)strtoul(optarg, NULL, 10);
				break;

			case 'c':
				num_conc = atoi(optarg);
				break;

			case '?':
				printf("Usage: ");
				break;
			default:
				printf("USage:");
				break;

		}
	}

	thread = (pthread_t*)malloc(sizeof(pthread_t) * (num_conc+1));


	wargs_list = (struct worker_args*)malloc(sizeof(struct worker_args) * num_conc);
	//pargs = (struct printer_args)malloc(sizeof(struct printer_args));

	adjusted_size = (unsigned int*)malloc(sizeof(unsigned int) * (num_conc + 1));
	adjusted_analyze = (unsigned int*)malloc(sizeof(unsigned int) * (num_conc + 1));
	bits_per_segment = (unsigned int*)malloc(sizeof(unsigned int) * (num_conc + 1));

	//printf("max_size: %d\n", max_size);
	//printf("num_conc: %d\n", num_conc);
	num_master_analyze = floor(sqrt(max_size));
	num_children_analyze = max_size - num_master_analyze;
	num_per_child = ceil(num_children_analyze/num_conc);
	num_last = num_children_analyze - (num_per_child * (num_conc - 1));	

	while(num_last < 3)
	{
		num_conc--;
		num_master_analyze = floor(sqrt(max_size));
		num_children_analyze = max_size - num_master_analyze;
		num_per_child = ceil(num_children_analyze/num_conc);
		num_last = num_children_analyze - (num_per_child * (num_conc - 1));	
	}

	// initialize adjusted_size
	adjusted_analyze[0] = num_master_analyze;
	adjusted_analyze[num_conc] = num_last;
	for(i = 1; i < num_conc; i++)
	{
		adjusted_analyze[i] = num_per_child;
	}


	// ensure all the end points are not of 6n+1 or 6n-1
	// since each segment finds twin primes independently of all
	// other segments
	for(i = 0; i < num_conc; i++)
	{
		current_upper = current_upper + adjusted_analyze[i];
		if(current_upper % 6 == 0)
		{
			adjusted_analyze[i] = adjusted_analyze[i] + 1;
			adjusted_analyze[i+1] = adjusted_analyze[i+1] - 1;
			current_upper = current_upper + 1;
		}
		else if(current_upper % 6 == 5)
		{
			adjusted_analyze[i] = adjusted_analyze[i] + 2;
			adjusted_analyze[i+1] = adjusted_analyze[i+1] - 2;
			current_upper = current_upper + 2;
		}
		// determine how many bits per segment we need
		if(current_base % 2 == 0)
		{
			if(current_upper % 2 == 0)
			{
				bits_per_segment[i] = odd_num_to_bit_array(current_upper-1) - 
						      odd_num_to_bit_array(current_base+1);
			}
			else
			{
				bits_per_segment[i] = odd_num_to_bit_array(current_upper) -
						      odd_num_to_bit_array(current_base+1);
			}
		}
		else if(current_upper % 2 == 0)
		{
			bits_per_segment[i] = odd_num_to_bit_array(current_upper - 1) -
					      odd_num_to_bit_array(current_base);
		}
		else
		{
			bits_per_segment[i] = odd_num_to_bit_array(current_upper) - 
					      odd_num_to_bit_array(current_base);
		}
		bits_per_segment[i] = bits_per_segment[i] + 1;
		current_base = current_upper + 1;
	}
	
	// get the last segment's num_bits
	current_upper = current_upper + adjusted_analyze[i];
	if(current_base % 2 == 0)
	{
		if(current_upper % 2 == 0)
		{
			bits_per_segment[i] = 1+odd_num_to_bit_array(current_upper-1) - 
					      odd_num_to_bit_array(current_base+1);
		}
		else
		{
			bits_per_segment[i] = 1+odd_num_to_bit_array(current_upper) -
					      odd_num_to_bit_array(current_base+1);
		}
	}
	else if(current_upper % 2 == 0)
	{
		bits_per_segment[i] = 1+odd_num_to_bit_array(current_upper - 1) -
				      odd_num_to_bit_array(current_base);
	}
	else
	{
		bits_per_segment[i] = 1+odd_num_to_bit_array(current_upper) - 
				      odd_num_to_bit_array(current_base);
	}


	// determine the size of each block
	for(i = 0; i < num_conc + 1; i++)
	{
		// the additional / 2 is there because we are halving
		// our space complexity as we don't need to consider
		// even numbers
		adjusted_size[i] = ceil(((float)bits_per_segment[i]/8));
		bitmap_size = bitmap_size + adjusted_size[i];
	}
	/*
	for(i = 0; i < num_conc + 1; i++)
	{
		printf("Num analyzed by %d, %d\n", i, adjusted_analyze[i]);
		printf("Num chars: %d\n", adjusted_size[i]);
		printf("Bits in segment: %d\n", bits_per_segment[i]);
	}
	*/

	bitmap = (unsigned char*)malloc(sizeof(unsigned char) * bitmap_size);
	offset_bitmap = bitmap + adjusted_size[0];
	current_base = adjusted_analyze[0];

	markers_to_printer = (int**)malloc(sizeof(int*) * (num_conc+1));
	master_to_workers = (int**)malloc(sizeof(int*) * num_conc);

	// initialize master_to_workers
	for(i = 0; i < num_conc; i++)
	{
		//printf("current_base for %d, %u\n", i+1, (current_base));
		//printf("current_num_offset %d, %d\n", i+1, (int)(offset_bitmap - bitmap));
		master_to_workers[i] = (int*)malloc(sizeof(int) * 2);
		markers_to_printer[i + 1] = (int*)malloc(sizeof(int) * 2);
		if(pipe(master_to_workers[i]) == -1)
		{
			perror("Couldn't open pipe");
			exit(1);
		}
		
		if(pipe(markers_to_printer[i + 1]) == -1)
		{
			perror("Coudln't open pipe");
			exit(1);
		}
		wargs_list[i].bm_base = offset_bitmap;
		wargs_list[i].base = current_base+1;
		wargs_list[i].bits = bits_per_segment[i+1];
		wargs_list[i].num_analyze = adjusted_analyze[i+1];
		wargs_list[i].pipe_from_master = master_to_workers[i];
		wargs_list[i].pipe_to_printer = markers_to_printer[i+1];
		
		if(0 != pthread_create(&thread[i],
					NULL,
					worker,
					(void*)&wargs_list[i]))
		{
			perror("Making wargs threads");
			exit(1);
		}
				/*worker(offset_bitmap,
				       current_base,
				       bits_per_segment[i+1],
				       adjusted_analyze[i+1],
				       master_to_workers[i],
				       markers_to_printer[i + 1]);
					*/
		current_base = current_base + adjusted_analyze[i+1];
		offset_bitmap = offset_bitmap + adjusted_size[i+1];
	}

	// make sure we open the master pipe!!!
	markers_to_printer[0] = (int*)malloc(sizeof(int) * 2);
	if(pipe(markers_to_printer[0]) == -1)
	{
		perror("making pipe");
		exit(1);
	}

	pargs.pipes = markers_to_printer;
	pargs.num_markers = num_conc + 1;
	pargs.quiet = quiet;

	if(0 != pthread_create(&thread[num_conc],
				NULL,
				printer,
				(void*)&pargs))
	{
		perror("spawning printer thread");
		exit(1);
	}

	master(bitmap, bits_per_segment[0], 
			max_size, master_to_workers, 
			markers_to_printer[0], num_conc);

	// join alll threads
	for(i = 0; i < num_conc+1; i++)
	{
		pthread_join(thread[i], NULL);
	}

	return 0;
}
