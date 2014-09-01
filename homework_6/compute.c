/*
 * Josh Deare
 * CS344-400
 * Homewark 6
 * dearej@onid.orst.edu
 *
 */

#define _POSIX_SOURCE
#define _BSD_SOURCE
#include <errno.h>
#include <getopt.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

void compute_perfect(int, int, int, int);
void report_perfect(int, int);
char* marshall_perfect(int);
int* unmarshall_range(int);
char* marshall_status(char*);
char* marshall_mods_per_second(int);

// Returns valid xml for the number of
// mod operations per second
char* marshall_mods_per_second(int mps)
{
    // TODO: Implement this
}

// Returns valid xml for the
// status given in the status argument
char* marshall_status(char* status)
{
    // TODO: Implement this
    return NULL;
}

// returns an array of length 2 containing
// the range over which we are to iterate
int* unmarshall_range(int socket)
{
    // TODO: Implement this
    return NULL;
}

// Marshalls perfect number into
// proper XML
char* marshall_perfect(int perfect)
{
    // TODO: Implement this
    return NULL;
}

// Takes a perfect number and reports the perfect number to
// the manager
void report_perfect(int perfect, int sock)
{
    char* xml = marshall_perfect(perfect);
    if(write(sock, xml, sizeof(char)*strlen(xml)) != sizeof(char)*strlen(xml))
    {
        perror("Writing xml to manager");
        exit(1);
    }
}

// computes perfect numbers in a given range
void compute_perfect(int lower, int higher, int sock, int debug)
{
    int i;
    int j;
    int sum = 0;
    for(i = lower; i <= higher; i++)
    {
        for(j = 1; j < i; j++)
        {
            if(i % j == 0)
            {
                sum += j;
            }
        }
        if(sum == i)
        {
            if(debug != 1)
            {
                report_perfect(i, sock);
            }
            else
            {
                printf("Perfect Found: %d\n", i);
            }
        }
        sum = 0;
    }
}

int main(int argc, char** argv)
{
    int c;
    char* host;
    int port;
    int debug = 0;
    int debug_upto;
    int sfd = 0;
    struct sockaddr_in addr;
    int* range;
    while((c = getopt(argc, argv, "h:p:d:")) != -1)
    {
        switch(c)
        {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'd':
                debug = 1;
                debug_upto = atoi(optarg);
                break;
            case '?':
                printf("Usage: compute -h REMOTEHOSTNAME -p REMOTEPORT [-d(debug mode)]\n");
                break;
            default:
                printf("Usage: compute -h REMOTEHOSTNAME -p REMOTEPORT [-d(debug mode)]\n");
                break;
        }
    }
    
    if(debug == 1)
    {
        compute_perfect(6, debug_upto, sfd, debug);
        return 0;
    }
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
    {
        perror("Creating socket");
        exit(1);
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    if(connect(sfd, (struct sockaddr*) &addr, sizeof(addr)) == -1)
    {
        perror("Host Unavailable");
        exit(1);
    }

    range = unmarshall_range(sfd);
    compute_perfect(range[0], range[1], sfd, debug);
}
