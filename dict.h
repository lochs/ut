#ifndef __DICT_H
#define __DICT_H

#include "types.h"

struct word_s *dict_load(struct config_s *config);
static int dict_get(struct word_s *word, struct dict_s *dict);
static int dict_wc(FILE *dict, int wl);
static void dict_mod(struct word_s *word, struct config_s *config);
void dict_free(struct word_s *words);

#endif
