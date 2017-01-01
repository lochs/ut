#include <ncurses.h>
#include <stdlib.h>
#include "defs.h"
#include "dict.h"
#include "menu.h"
#include "misc.h"
#include "types.h"
#include "typer.h"
#include "settings.h"

void ncurses_init();

int main()
{
	struct config_s config = {DICT_DIR, DEFAULT_DICT, DEFAULT_WC, 10};

	// srand seed
	seed();

	// Init ncurses
	ncurses_init();

	enum state state = STATE_MENU;

	while(1) {
		if(state == STATE_EXIT)
			break;

		switch(state) {
		case STATE_MENU:
			state = menu();
			break;
		case STATE_TYPE:
			state = typer(&config);	
			break;
		case STATE_SETTINGS:	
			settings(&config);
			state = STATE_MENU;
			break;
		case STATE_STATS:
			// Show stats!
			state = STATE_MENU;
			break;
		}
	}

	// End ncurses
	endwin();

	printf("Bye.\n");

	return 0;
}

void ncurses_init()
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	set_escdelay(10);
	start_color();
	init_color(COLOR_GREEN, 0, 1000, 0);
	init_color(COLOR_RED, 1000, 0, 0);
	init_color(COLOR_BLUE, 0, 0, 1000);
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_RED);
}
