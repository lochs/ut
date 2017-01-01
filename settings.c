#include <form.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "settings.h"

FORM *form;
FIELD *fields[4];

static void settings_init(struct config_s *config)
{
	char wc[5], ucp[4];

	// Int to string conversions
	sprintf(wc, "%d", config->wc);
	sprintf(ucp, "%d", config->ucp);

	// Cursor visible
	curs_set(1);

	fields[0] = new_field(1, 4, 1, 0, 0, 0);
	fields[1] = new_field(1, 20, 3, 0, 0, 0);
	fields[2] = new_field(1, 3, 5, 0, 0, 0);
	fields[3] = NULL;

	field_opts_off(fields[0], O_AUTOSKIP);
	field_opts_off(fields[1], O_AUTOSKIP);
	field_opts_off(fields[2], O_AUTOSKIP);

	field_opts_on(fields[0], O_BLANK);
	field_opts_on(fields[1], O_BLANK);
	field_opts_on(fields[2], O_BLANK);

	set_field_back(fields[0], A_UNDERLINE);
	set_field_back(fields[1], A_UNDERLINE);
	set_field_back(fields[2], A_UNDERLINE);

	form = new_form(fields);
	post_form(form);

	mvprintw(0, 0, "Number of words");
	mvprintw(2, 0, "Dictionary file");
	mvprintw(4, 0, "Uppercase weight %");

	refresh();

	set_field_buffer(fields[0], 0, wc);
	set_field_buffer(fields[1], 0, config->df);
	set_field_buffer(fields[2], 0, ucp);

	form_driver(form, REQ_FIRST_FIELD);
}

int settings(struct config_s *config)
{
	char *wc, *df, *ucp;
	int dlen;

	settings_init(config);

	while(1) {
		if(!keypress(getch()))
			break;
	}

	// Sync
	form_driver(form, REQ_VALIDATION);
	
	// Process data
	wc = field_buffer(fields[0], 0);
	df = field_buffer(fields[1], 0);
	ucp = field_buffer(fields[2], 0);

	config->wc = atoi(wc);
	config->ucp = atoi(ucp);

	dlen = trim_ws(df, 20);
	strncpy(config->df, df, dlen + 1);

	settings_deinit();

	return STATE_MENU;
}

static void settings_deinit(void)
{
	unpost_form(form);
	free_form(form);
	free_field(fields[0]);
	free_field(fields[1]);
}

static int keypress(int c)
{
	FIELD *f = current_field(form);

	switch(c) {
	case 27:			// Enter and esc exit	
	case 10:
		return 0;
	case KEY_DOWN:
	case '\t':
		form_driver(form, REQ_NEXT_FIELD);
		form_driver(form, REQ_END_LINE);
		break;
	case KEY_UP:
		form_driver(form, REQ_PREV_FIELD);
		form_driver(form, REQ_END_LINE);
		break;
	case KEY_LEFT:
		form_driver(form, REQ_PREV_CHAR);
		break;
	case KEY_RIGHT:
		form_driver(form, REQ_NEXT_CHAR);
		break;
	case KEY_BACKSPACE:
	case 127:
		form_driver(form, REQ_DEL_PREV);
		break;
	case KEY_DC:
		form_driver(form, REQ_DEL_CHAR);
		break;
	default:
		form_driver(form, c);
		break;
	}

	refresh();

	return 1;
}
