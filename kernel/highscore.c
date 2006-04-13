
/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

__nvram__ U8 gc_score[HIGH_SCORE_WIDTH];
__nvram__ U8 gc_initials[HIGH_SCORE_NAMESZ];

__nvram__ U8 highest_scores[NUM_HIGH_SCORES][HIGH_SCORE_WIDTH];
__nvram__ U8 high_score_initials[NUM_HIGH_SCORES][HIGH_SCORE_NAMESZ];

static U8 default_gc_score[HIGH_SCORE_WIDTH] = { 0x10, 0x00, 0x00, 0x00 };
static U8 default_gc_initials[HIGH_SCORE_NAMESZ] = { 'B', 'C', 'D' };

static U8 default_highest_scores[NUM_HIGH_SCORES][HIGH_SCORE_WIDTH] = {
	{ 0x08, 0x00, 0x00, 0x00 },
	{ 0x07, 0x00, 0x00, 0x00 },
	{ 0x06, 0x00, 0x00, 0x00 },
	{ 0x05, 0x00, 0x00, 0x00 },
};
static U8 default_high_score_initials[NUM_HIGH_SCORES][HIGH_SCORE_NAMESZ] = {
	{ 'Q', 'Q', 'Q' },
	{ 'F', 'T', 'L' },
	{ 'N', 'P', 'L' },
	{ 'P', 'Y', 'L' },
};


static void high_score_draw_single (int pos, const U8 *initials, 
	const U8 *score, int row)
{
	if (pos != 0)
		sprintf ("%d. %c%c%c", pos, initials[0], initials[1], initials[2]);
	else
		sprintf ("%c%c%c", initials[0], initials[1], initials[2]);
	font_render_string_left (&font_fixed6, 2, row, sprintf_buffer);

	sprintf ("%8b", score);
	font_render_string_right (&font_fixed6, 126, row, sprintf_buffer);
}


void high_score_amode_show (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 8, "GRAND CHAMPION");
	high_score_draw_single (0, gc_initials, gc_score, 20);
	dmd_show_low ();
	task_sleep_sec (5);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "HIGHEST SCORES");
	high_score_draw_single (1, high_score_initials[0], highest_scores[0], 8);
	high_score_draw_single (2, high_score_initials[1], highest_scores[1], 20);
	dmd_show_low ();
	task_sleep_sec (4);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "HIGHEST SCORES");
	high_score_draw_single (3, high_score_initials[2], highest_scores[2], 8);
	high_score_draw_single (4, high_score_initials[3], highest_scores[3], 20);
	dmd_show_low ();
	task_sleep_sec (4);
}


void high_score_reset (void)
{
	wpc_nvram_get ();

	/* Reset the grand champion */
	memcpy (gc_score, default_gc_score, HIGH_SCORE_WIDTH);
	memcpy (gc_initials, default_gc_initials, HIGH_SCORE_NAMESZ);

	/* Reset the other high scores */
	memcpy (highest_scores, default_highest_scores,
		HIGH_SCORE_WIDTH * NUM_HIGH_SCORES);
	memcpy (high_score_initials, default_high_score_initials,
		HIGH_SCORE_NAMESZ * NUM_HIGH_SCORES);
	wpc_nvram_put ();
}


void high_score_init (void)
{
	high_score_reset ();
}

