/*
 * Copyright 2010-2011 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>
#include <window.h>
#include <test.h>

extern struct histogram *browser_histogram;

/* The standard histogram for tracking game scores */

U16 score_histogram_values[] =
{
	/* These score levels are taken from Congo. */
	0x0100,
	0x0300,
	0x0500,
	0x0750,
	0x1000,
	0x1500,
	0x2000,
	0x3000,
	0x4000,
	0x5000,
	0x7000,
	0x9000,
	0xFFFF,
};


struct histogram score_histogram =
{
	.label = "SCORE LEVEL",
	.count = sizeof (score_histogram_values) /
		sizeof (U16),
	.values = score_histogram_values,
	.audits = &system_audits.hist_score[0],
	.render = "%lX-%lXM.",
	.render_max = "ABOVE %lXM.",
};


/* The standard histogram for tracking game times */

U16 game_time_histogram_values[] =
{
	60, 90, 120, 150, 180, 210, 240,
	300, 360, 480, 600, 900, 0xFFFF,
};


struct histogram game_time_histogram =
{
	.label = "TIME LEVEL",
	.count = sizeof (game_time_histogram_values) /
		sizeof (U16),
	.values = game_time_histogram_values,
	.audits = &system_audits.hist_game_time[0],
	.render = "%ld-%ld SEC.",
	.render_max = "ABOVE %ld SEC.",
};


/**
 * Add a new audit to the histogram.  Lookup VAL in the table of values and
 * see which audit should be incremented.
 */
static void histogram_add (struct histogram *hist, U16 val)
{
	U8 count = hist->count;
	audit_t *auditptr = hist->audits;
	U16 *valueptr = hist->values;

	while (count > 0)
	{
		if (val < *valueptr)
		{
			audit_increment (auditptr);
			return;
		}
		valueptr++;
		auditptr++;
		count--;
	}
}

void score_histogram_add (U16 val)
{
	histogram_add (&score_histogram, val);
}

void game_time_histogram_add (U16 val)
{
	histogram_add (&game_time_histogram, val);
}

#ifdef CONFIG_DMD_OR_ALPHA
void histogram_browser_draw_1 (void)
{
	extern U8 browser_max;
	U16 min, max;
	sprintf ("%s %d", browser_histogram->label, menu_selection+1);
	print_row_center (&font_mono5, 10);

	if (menu_selection != 0)
		min = browser_histogram->values[menu_selection-1];
	else
		min = 0;
	max = browser_histogram->values[menu_selection];

	if (menu_selection == browser_max)
		sprintf (browser_histogram->render_max, min);
	else
		sprintf (browser_histogram->render, min, max);
	font_render_string_left (&font_mono5, 0, 21, sprintf_buffer);

	sprintf ("%ld", browser_histogram->audits[menu_selection]);
	font_render_string_right (&font_mono5, 127, 21, sprintf_buffer);
	dmd_show_low ();
}

void histogram_browser_init_1 (void)
{
	extern U8 browser_max;
	browser_max = browser_histogram->count - 1;
}

#endif
