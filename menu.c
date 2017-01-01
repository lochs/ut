#include <ncurses.h>
#include <string.h>
#include "defs.h"
#include "menu.h"

static const char *items[4] = {
	"Type",
	"Settings",
	"Stats",
	"Exit"
};

WINDOW *menu_win;

enum state menu(void)
{
	int w, h;

	// Hide cursor, no echo, clear + refresh screen
	curs_set(0);
	noecho();
	clear();
	refresh();

	// Menu window
	getmaxyx(stdscr, w, h);
	menu_win = newwin(MENU_HEIGHT, MENU_WIDTH, (w - MENU_HEIGHT) / 2, (h - MENU_WIDTH) / 2);
	box(menu_win, 0, 0);

	refresh();

	int focus = 0, c;

	while(1) {
		for(int i = 0; i < 4; i++) {
			// Highlight selected
			int cp = (i == focus) ? 1 : 0;

			wattron(menu_win, COLOR_PAIR(cp));
			mvwprintw(menu_win, i + 1, 1, "%*s", MENU_WIDTH - 2, "");	

			// Center
			int sx = (MENU_WIDTH - strlen(items[i])) / 2;
			mvwprintw(menu_win, i + 1, sx, "%s", items[i]);

			wattroff(menu_win, COLOR_PAIR(cp));
		}		

		// Refresh window contents
		wrefresh(menu_win);

		c = getch();

		switch(c) {
		case KEY_UP:
			if(focus > 0) {
				focus--;
			} else {
				focus = 3;
			}
			break;
		case KEY_DOWN:
			if(focus < 3) {
				focus++;
			} else {
				focus = 0;
			}
			break;
		case 10:
			delwin(menu_win);
			return focus;
		}
	}

	return 0;
}
