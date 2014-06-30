/*
 *  Josh Deare
 *  dearej@onid.orst.edu
 *  CS311-400
 *  dearej@onid.orst.edu
 *
 */

#include<stdio.h>
#include<math.h>
#include<stdlib.h>

#define PRIME 3
#define COMPOSITE 4

struct PrimeListing {
	int count;
	int* listing;
};

struct PrimeListing primes(int scan_to)
{
	int* num_list;

	num_list = (int*)malloc(scan_to * sizeof(int));
	
	int* primes;
	primes = (int*)malloc(4 * sizeof(int));

	int num_primes = 0;
	int k = 2;
	while (k < sqrt(scan_to))
	{
		int m = k;
		while (num_list[m] == COMPOSITE && m < scan_to)
		{
			m++;
		}
		int comp_numbers = 2;
		while (comp_numbers * m < scan_to)
		{
			num_list[comp_numbers * m] = COMPOSITE;
			comp_numbers++;
		}
		if (sizeof(primes)/sizeof(int) < num_primes - 1)
		{
			primes = (int*) realloc(primes, (5 + num_primes) * sizeof(int));
		}	
		primes[num_primes] = m;
		num_primes++;
		k = m + 1;
	}
	
	for (; k < scan_to; k++)
	{
		if (num_list[k] != COMPOSITE)
		{
			if (sizeof(primes)/sizeof(int) < num_primes - 1)
			{
				primes = (int*)realloc(primes, (5 + num_primes) * sizeof(int));
			}	
			primes[num_primes] = k;
			num_primes++;
		}	
	}	

	primes = (int*)realloc(primes, sizeof(int) * num_primes);
	struct PrimeListing ret_val = {num_primes, primes};
	return ret_val;
}

int main()
{
	printf("There are 6 Primes from 0 - 15: 2, 3, 5, 7, 11, 13.\n");
	printf("Demonstration of the prime function is printed below.\n\n");

	struct PrimeListing prim = primes(15);
	printf("Primes up to 15(printed from a returned struct from primes function):\n");
	printf("Number of Primes: %d\n", prim.count);
	printf("Listing of Primes\n");
	int i;
	for(i = 0; i < prim.count; i++)
	{
		printf("%d", prim.listing[i]);
		if (i < prim.count - 1)
		{
			printf(", ");
		}
		else
		{
			printf("\n");
		}	
	}
	return 0;
}
