#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "dict.h"
#include "types.h"

struct word_s *dict_load(struct config_s *config)
{
	// Allocate words
	struct word_s *words = malloc((config->wc + 1) * sizeof(struct word_s));
	if(!words)
		return NULL;

	// Null out the data structure
	// In case of an emergency, we can safely free everything
	memset(&(*words), 0, (config->wc + 1) * sizeof(struct word_s));

	struct dict_s dict;

	// Open dictionary file
	char fname[41];
	snprintf(fname, 41, "%s/%s", config->ddir, config->df);
	dict.fh = fopen(fname, "r");
	if(!dict.fh) {
		free(words);
		return NULL;
	}

	// Get dictionary word count
	// For now, word length comes from a def.
	// Perhaps change this in the future (to be included in the dictionary file?)
	dict.wl = DICT_WORDLEN;
	dict.wc = dict_wc(dict.fh, dict.wl);

	// Get words
	for(int i = 0; i < config->wc; i++) {
		if(!dict_get(&words[i], &dict)) {
			dict_free(words);
			
			return NULL;
		}
		dict_mod(&words[i], config);
	}

	// Close dictionary file
	fclose(dict.fh);

	return words;
}

static int dict_get(struct word_s *word, struct dict_s *dict)
{
	// Temporary in-buffer
	char tmp[MAX_WORDLEN];

	// Get a random word
	fseek(dict->fh, (rand() % dict->wc) * (dict->wl + 1), SEEK_SET);
	fgets(tmp, dict->wl + 1, dict->fh);	

	// Strip whitespace
	for(int i = 0; tmp[i]; i++) {
		if(tmp[i] == ' ') {
			tmp[i] = 0;
		}
	}

	// Store length
	word->len = strlen(tmp);

	// Allocate space
	word->str = malloc(word->len + 1);
	if(!word->str)
		return 0;

	// Copy string over
	strcpy(word->str, tmp);

	return 1;
}

static int dict_wc(FILE *dict, int wl)
{
	fseek(dict, 0, SEEK_END);
	int fs = ftell(dict);
	fseek(dict, 0, SEEK_SET);
			
	/* Offset by one due to the newline character */
	return (fs / (wl + 1));
}

static void dict_mod(struct word_s *word, struct config_s *config)
{
	// String modification
	// Uppercase
	if(config->ucp) {
		if((rand() % 100) < config->ucp)
			word->str[0] = toupper(word->str[0]);
	}
}

void dict_free(struct word_s *words)
{
	// Free allocated strings
	for(struct word_s *word = words; word->str; word++)
		free(word->str);

	// Free the word pointer
	free(words);
}
