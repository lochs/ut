#include <ncurses.h>
#include <string.h>
#include "misc.h"
#include "stats.h"

int stats_show_single(struct result_s *results, int wc)
{
	double tot_time, t, wpm, wwpm = 0.0, acc = 0.0;
	int ccount = 0, len;

	clear();
	curs_set(0);
	refresh();

	// Total time
	tot_time = timediff(&results[0].stime, &results[wc - 1].etime);

	for(int i = 0; i < wc; i++) {
		t = timediff(&results[i].stime, &results[i].etime);

		len = strlen(results[i].input);

		// WPM per word
		wwpm += get_wpm(len, t);

		// Accuracy
		acc += stracc(results[i].word->str, results[i].input);

		// Increment input character count
		ccount += len;
	}
	
	wwpm /= (double)wc;
	wpm = get_wpm(ccount, tot_time);
	acc /= (double)wc;

	move(0,0);
	printw("Average WPM (per word): %.2f\n", wwpm);
	printw("WPM: %.2f\n", wpm);
	printw("Average accuracy:  %.2f\%\n", acc);
	printw("Press any key to continue...");

	getch();

	return 1;
}
