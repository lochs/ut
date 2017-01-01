#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "types.h"

static void settings_init(struct config_s *config);
int settings(struct config_s *config);
static void settings_deinit(void);
static int keypress(int c);

#endif
