
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

#define HIGH_SCORE_WIDTH	4
#define HIGH_SCORE_NAMESZ	3
#define NUM_HIGH_SCORES		4

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


void gc_draw (void)
{
	dmd_alloc_low_clean ();

	font_render_string_center (&font_fixed10, 64, 6, "GRAND CHAMPION");
	sprintf ("%c%c%c", gc_initials[0], gc_initials[1], gc_initials[2]);
	font_render_string_left (&font_fixed10, 2, 22, sprintf_buffer);
	sprintf ("%8b", gc_score);
	font_render_string_right (&font_fixed10, 126, 22, sprintf_buffer);
}


void high_score_amode_show (void)
{
	gc_draw ();
	// dmd_sched_transition (&trans_scroll_right);
	dmd_show_low ();
	task_sleep_sec (5);
}


void high_score_reset (void)
{
	wpc_nvram_get ();

	/* Reset the grand champion */
	memcpy (gc_score, default_gc_score, HIGH_SCORE_WIDTH);
	memcpy (gc_initials, default_gc_initials, HIGH_SCORE_NAMESZ);

	/* Reset the other high scores */
	/* TODO */
	wpc_nvram_put ();
}


void high_score_init (void)
{
	high_score_reset ();
}

