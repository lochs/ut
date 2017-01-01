#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include "defs.h"
#include "dict.h"
#include "typer.h"

static WINDOW *source_win, *input_win;
static struct word_s *words;
static struct result_s *results;

static int typer_init(struct config_s *config)
{
	// Allocate result buffer
	results = malloc(sizeof(struct result_s) * config->wc);
	if(!results)
		return 0;

	// Load source words
	words = dict_load(config);
	if(!words)
		return 0;

	// Clear screen
	clear();

	// Initialize windows
	// Calculate centering, offset for borders
	int width = TYPER_SWIN_WIDTH + 2, height = TYPER_SWIN_HEIGHT + 2;
	int sx = (COLS - width) / 2, sy = (LINES - height) / 2;

	source_win = newwin(height, width, sy, sx);

	// Input window dimensions hard-coded
	input_win = newwin(3, width, sy + height, sx);

	// Cursor visible
	curs_set(1);

	// Borders
	box(source_win, 0, 0);
	box(input_win, 0, 0);

	// Special keys
	keypad(input_win, TRUE);

	scrollok(source_win, TRUE);
	wsetscrreg(source_win, 1, TYPER_SWIN_HEIGHT);

	// Initial refresh
	refresh();
	wrefresh(source_win);
	wrefresh(input_win);

	return 1;
}

int typer(struct config_s *config)
{
	// Init (allocate results, load words, init windows) 
	if(!typer_init(config)) {
		typer_deinit();

		return STATE_MENU;
	}	

	// Print out words
	struct word_s *line = words;
	for(int x = 1, y = 1; line->len && y <= TYPER_SWIN_HEIGHT; y++)
		line += typer_printline(line, y, TYPER_SWIN_WIDTH);

	wrefresh(source_win);

	// Cursor to input window
	wmove(input_win, 1, 1);
	wrefresh(input_win);

	// Typing loop
	struct word_s *word = words;
	struct result_s *result = results;
	for(int sx = 1, sy = 1; word->len; word++, result++) {
		// Boundary check
		if(sx + word->len > TYPER_SWIN_WIDTH) {
			sx = 1;
			// Scroll if need be
			if(line->len) {
				scroll(source_win);
				line += typer_printline(line, TYPER_SWIN_HEIGHT, TYPER_SWIN_WIDTH);

				box(source_win, 0, 0);
				wrefresh(source_win);
				wmove(input_win, 1, 1);
			} else {
				sy++;
			}
		}

		int act = typer_io(word, result, sx, sy);
		if(act < 0) {
			typer_deinit();
			
			return (act == -1) ? STATE_TYPE : STATE_MENU;	
		}
		// Color last source word according to input
		int cp = (strcmp(word->str, result->input) == 0) ? COLOR_GOOD : COLOR_BAD;

		wattron(source_win, COLOR_PAIR(cp));
		mvwaddstr(source_win, sy, sx, word->str);
		wattroff(source_win, COLOR_PAIR(cp));

		// Advance cursor position, take trailing space into account
		sx += word->len + 1;	
	}

	typer_deinit(); 

	return STATE_MENU;
}

static int typer_printline(struct word_s *word, int y, int ll)
{
	int count = 0;

	// Print the word line
	for(int x = 1; word->len; word++) {
		if(x + word->len > TYPER_SWIN_WIDTH)
			break;

		mvwaddstr(source_win, y, x, word->str);	
		x += word->len + 1;
		count++;
	}

	return count;
}

static void typer_deinit(void)
{
	// Free allocations
	if(words)
		dict_free(words);

	if(results)	
		free(results);

	// Delete windows
	delwin(source_win);
	delwin(input_win);

	refresh();
}

static int typer_io(struct word_s *source, struct result_s *result, int x, int y)
{
	// Input buffer index, input character
	int index = 0, c = 0;

	// Input start time
	struct timespec stime;	

	// Timer run flag
	bool trun = FALSE;

	// Main loop
	while(1) {
		// Source window word coloring
		// Active = white on gray, Bad = white on red
		int pos = (index > source->len) ? source->len : index; 
		int attr = (strncmp(source->str, result->input, pos) != 0 || index > source->len) ? COLOR_PAIR(COLOR_BAD) : A_STANDOUT;

		wattron(source_win, attr);
		mvwaddstr(source_win, y, x, source->str);
		wattroff(source_win, attr);

		// Refresh source window after word coloring
		wrefresh(source_win);

		// Move cursor, Get a character
		wmove(input_win, 1, index + 1);
		c = wgetch(input_win);	

		// Start time not set yet?
		if(!trun) {
			clock_gettime(CLOCK_MONOTONIC, &stime);
			trun = TRUE;
		}
	
		// Handle backspace, get input 
		if(c == 127 || c == KEY_BACKSPACE) {
			if(index > 0) {
				mvwaddch(input_win, 1, index, ' ');
				wmove(input_win, 1, --index);
				wrefresh(input_win);
			}

			continue;
		} else if(isprint(c) && index < DICT_WORDLEN - 1) {
			// Return on space clear input box
			if(isspace(c)) {
				for(int i = 0; i < index; i++) {
					mvwaddch(input_win, 1, i + 1, ' ');
					wmove(input_win, 1, 1);
				}	
				
				wrefresh(input_win);

				// Terminate input string
				result->input[index] = '\0';

				// Get input end time
				result->t = typer_get_time(&stime);

				return 0;
			}

			waddch(input_win, c);
			result->input[index] = (char)c;
			index++;
		} else {
			int act =  typer_ctrl(c);
			if(act < 0)
				return act;
		}	
	}

	// Should not get here
	return 0;
}

static double typer_get_time(struct timespec *stime) 
{
	struct timespec etime;
	clock_gettime(CLOCK_MONOTONIC, &etime);

	double start = (stime->tv_sec * 1000) + (stime->tv_nsec / 1000000.0);	
	double end = (etime.tv_sec * 1000) + (etime.tv_nsec / 1000000.0);
	
	return (double)((end - start) / 1000.0);
}

static int typer_ctrl(int c)
{
	switch(c) {
	case KEY_F(1):
		return -1;
	case 27:
		return -2;
	default:
		return 0;
	}

	return 0;
}
