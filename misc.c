#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "misc.h"

int seed(void)
{
	FILE *fh = fopen("/dev/urandom", "r");
	if(!fh)
		return 0;

	unsigned int n;
	fread(&n, sizeof(unsigned int), 1, fh);

	srand(n ^ getpid());
	
	fclose(fh);

	return 1;
}

int trim_ws(char *str, int len)
{
	int i;
	for(i = 0; i < len; i++) {
		if(str[i] == ' ') {
			str[i] = 0;
			break;
		}
	}

	str[len] = 0;

	return i;
}
