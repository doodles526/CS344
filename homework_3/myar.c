#include "myar.h"

char* arstruct_to_char_array(struct ar_hdr *ar_st)
{
	char *ar_char = (char*)malloc(sizeof(char) * 60);

	// copy the contents of our struct to an array
	memcpy(ar_char, ar_st, sizeof(char) * 60);

	// replace all NULL values with spaces
	for(int i = 0; i < 60; i++)
	{
		if(ar_char[i] == '\0')
		{
			ar_char[i] = ' ';
		}
	}

	// return the char array
	return ar_char;
}

struct ar_hdr* char_array_to_arstruct(char* ar_arr)
{
	//for int i = 0
	return (struct ar_hdr*)1;
}
