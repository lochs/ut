#ifndef __MISC_H
#define __MISC_H

#include <time.h>

int seed(void);
int trim_ws(char *str, int len);
double timediff(struct timespec *stime, struct timespec *etime);
double get_wpm(int ccount, double time);
double stracc(char *source, char *input);

#endif
