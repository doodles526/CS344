#define _POSIX_SOURCE
#define _BSD_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

extern const char *const sys_siglist[];

void sigusr1_handl(int signal)
{
	printf("SIGUSR1 has been caught\n");
}

void sigusr2_handl(int signal)
{
	printf("SIGUSR2 has been caught\n");
}

void sigint_handl(int signal)
{
	printf("SIGINT has been caught, terminating program\n");
	// Exit code for termination with SIGINT, referenced from:
	// http://www.tldp.org/LDP/abs/html/exitcodes.html
	exit(130);
}

int main()
{
	// define sigaction structs
	struct sigaction sigusr1_a;
	struct sigaction sigusr2_a;
	struct sigaction sigint_a;

	// define the oldaction structs
	struct sigaction sigusr1_a_old;
	struct sigaction sigusr2_a_old;
	struct sigaction sigint_a_old;

	// assign the handles function
	sigusr1_a.sa_handler = sigusr1_handl;
	sigusr2_a.sa_handler = sigusr2_handl;
	sigint_a.sa_handler = sigint_handl;

	// ignoring all flags
	sigemptyset(&sigusr1_a.sa_mask);
	sigemptyset(&sigusr1_a.sa_mask);
	sigemptyset(&sigusr1_a.sa_mask);
	
	// enable the alternate handlers
	sigaction(SIGUSR1, &sigusr1_a, &sigusr1_a_old);
	sigaction(SIGUSR2, &sigusr2_a, &sigusr2_a_old);
	sigaction(SIGINT, &sigint_a, &sigint_a_old);
	
	// get this processes PID
	int my_pid = getpid();
	
	// send SIGUSR1 signal to this process
	kill(my_pid, SIGUSR1);
	
	// Wait a bit until signaling again
	int unslept = sleep(1);
	
	// ensure we actually slept
	while(unslept != 0)
	{
		unslept = sleep(unslept);
	}

	//send SIGUSR2 signal to this process
	kill(my_pid, SIGUSR2);

	// wait a bit until signaling again
	unslept = sleep(1);

	while (unslept != 0)
	{
		unslept = sleep(unslept);
	}
	// send SIGINT signal to this process
	kill(my_pid, SIGINT);

	return 0;
}
