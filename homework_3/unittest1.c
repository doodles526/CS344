#include "myar.h"
#include <assert.h>
#include "fixtures.c"
int test_arstruct_to_char_array()
{
	
	char *test = arstruct_to_char_array(&ar_st);

	assert(! memcmp(ar_arr, test, sizeof(char) * 60));

	return 0;

}

int main()
{
	return test_arstruct_to_char_array();
}
