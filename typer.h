#ifndef __TYPER_H
#define __TYPER_H

#include "types.h"

static int typer_init(struct config_s *config);
int typer(struct config_s *config);
static void typer_free(void);
static void typer_deinit(void);
static int typer_printline(struct word_s *word, int y, int ll);
static int typer_io(struct word_s *source, struct result_s *result, int x, int y);
static int typer_ctrl(int c);

#endif
