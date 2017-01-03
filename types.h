#ifndef __TYPES_H
#define __TYPES_H

#include <stdio.h>
#include <time.h>
#include "defs.h"

struct config_s {
	char df[21];		// Dictionary file name
	char ddir[21];		// Dictionary directory
	int wc;			// Word count
	int ucp;		// Upper-case percentage (if any)
};

struct dict_s {
	FILE *fh;		// Dictionary file handle
	int wc;			// Total word count in dictionary file
	int wl;			// Dictionary file word length
};

struct word_s {
	char *str;		// Word string
	int len;		// String length
};

struct result_s {
	struct word_s *word;		// Source word
	char input[DICT_WORDLEN + 1];	// Input word
	struct timespec stime;		// Input start time
	struct timespec etime;		// Input end time
};

enum state {
	STATE_TYPE,
	STATE_SETTINGS,
	STATE_STATS,
	STATE_EXIT,
	STATE_MENU
};

#endif
