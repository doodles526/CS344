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
	int i = 2;
	printf("Before Loop");
	while (sqrt(i) < scan_to)
	{
		if (i != COMPOSITE)
		{
			int h;
			for (h = 2*i; h < scan_to; h*=2)
			{
				num_list[h] = COMPOSITE;
			}
			num_list[i] = PRIME;
			
			if (sizeof(primes)/sizeof(int) < num_primes)
			{
				primes = realloc(primes, sizeof(int) * (num_primes + 4));
			}

			primes[num_primes] = i;
			num_primes++;
		}
		else 
		{
			i++;
		}
	}
	primes = realloc(primes, sizeof(int) * num_primes);
	struct PrimeListing ret_val = {num_primes, primes};
	return ret_val;
}

int main()
{
	printf("first thing");
	struct PrimeListing prim = primes(15);
	printf("Number of Primes: %d\n", prim.count);
	printf("Listing of Primes\n");
	int i;
	for(i = 0; i < sizeof(prim.listing)/sizeof(int); i++)
	{
		printf("%d", prim.listing[i]);
		if (i < (sizeof(prim.listing)/sizeof(int) - 1))
		{
			printf(", ");
		}
	}
}
