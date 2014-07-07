/*
 * Josh Deare
 * dearej@onid.orst.edu
 * CS311-400
 * Homework 2
 */

#include <stdio.h> 
#include <getopt.h> 
#include <sys/utsname.h> 
#include <time.h> 
#include <sys/stat.h> 

int main(int argc, char * argv[])
{
	// initialize variables
	struct utsname uname_pointer;
	time_t time_raw_format; 
	struct stat s;
	int option;
	
	// This requires arguments
	// so if none are given then print usage string
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s [-h] hostname [-t] time [-f file_name] filesize\n", argv[0]);
		return 1;
	}

	// Parse all arguments with getopt
	while((option = getopt(argc, argv, "htf:")) != -1)
	{
		// send each argument through the switch
		// and perform the appropriate action
		switch(option)
		{
			case 'h':
				uname(&uname_pointer);
				printf("Hostname = %s \n", uname_pointer.nodename);
				break;
			case 't':
				time(&time_raw_format);
				printf("The current local time: %s", ctime(&time_raw_format));
				break;

			case 'f':
				if (stat(optarg, &s) == 0)
				{ 
					printf("size of file '%s' is %d bytes\n" , optarg, (int) s.st_size); 
				}
				else
				{ 
					printf("file '%s' not found\n", optarg);
				} 
				break;
			// if there is an invalid argument
			// then we print the usage string
			default:
				fprintf(stderr, "Usage: %s [-h] hostname [-t] time [-f file_name] filesize\n", argv[0]);
				return 1;
		}
	}
	return 0;
}
