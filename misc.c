#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "defs.h"
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

double timediff(struct timespec *stime, struct timespec *etime) 
{
	double start = (stime->tv_sec * 1000) + (stime->tv_nsec / 1000000.0);	
	double end = (etime->tv_sec * 1000) + (etime->tv_nsec / 1000000.0);
	
	return (double)((end - start) / 1000.0);
}

double get_wpm(int ccount, double time)
{
	return (ccount / WLEN) / (time / 60.0);
}

double stracc(char *source, char *input)
{
	int len = strlen(source), corr = 0;
	for(int i = 0; i < len && input[i]; i++) {
		if(source[i] == input[i])
			corr++;
	}

	return (double)corr / (double)len * 100.0;
}
