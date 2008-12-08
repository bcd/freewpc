/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
#include <highscore.h>

/**
 * \file
 * \brief Maintains high scores
 *
 */


/** The grand champion score */
__nvram__ U8 gc_score[HIGH_SCORE_WIDTH];
/** The initials of the grand champion */
__nvram__ U8 gc_initials[HIGH_SCORE_NAMESZ];

__nvram__ U8 gc_csum;
struct area_csum champion_csum_info = {
	.area = (U8 *)gc_score,
	.length = HIGH_SCORE_WIDTH + HIGH_SCORE_NAMESZ,
	.csum = &gc_csum,
	.reset = high_score_reset,
#ifdef HAVE_PAGING
	.reset_page = PAGE,
#endif
};


/** The highest scores */
__nvram__ U8 highest_scores[NUM_HIGH_SCORES][HIGH_SCORE_WIDTH];
/** The initials of the highest scores */
__nvram__ U8 high_score_initials[NUM_HIGH_SCORES][HIGH_SCORE_NAMESZ];

__nvram__ U8 high_score_csum;
struct area_csum highscore_csum_info = {
	.area = (U8 *)highest_scores,
	.length = (HIGH_SCORE_WIDTH + HIGH_SCORE_NAMESZ) * NUM_HIGH_SCORES,
	.csum = &high_score_csum,
	.reset = high_score_reset,
#ifdef HAVE_PAGING
	.reset_page = PAGE,
#endif
};


/* During high score entry, indicates the table position to insert at */
U8 high_score_position;

/* Indicates the player number being checked */
U8 high_score_player;


/** The default grand champion score */
static U8 default_gc_score[HIGH_SCORE_WIDTH] =
#ifndef MACHINE_GRAND_CHAMPION_SCORE
	{ 0x05, 0x00, 0x00, 0x00, 0x00 }
#else
	MACHINE_GRAND_CHAMPION_SCORE
#endif
	;


static U8 default_gc_initials[HIGH_SCORE_NAMESZ] =
#ifndef MACHINE_GRAND_CHAMPION_INITIALS
	{ 'B', 'C', 'D' }
#else
	MACHINE_GRAND_CHAMPION_INITIALS
#endif
	;


static U8 default_highest_scores[NUM_HIGH_SCORES][HIGH_SCORE_WIDTH] = {
#ifndef MACHINE_HIGH_SCORES
	{ 0x04, 0x00, 0x00, 0x00, 0x00 },
	{ 0x03, 0x50, 0x00, 0x00, 0x00 },
	{ 0x03, 0x00, 0x00, 0x00, 0x00 },
	{ 0x02, 0x50, 0x00, 0x00, 0x00 },
#else
	MACHINE_HIGH_SCORES
#endif
};


static U8 default_high_score_initials[NUM_HIGH_SCORES][HIGH_SCORE_NAMESZ] = {
#ifndef MACHINE_HIGH_SCORE_INITIALS
	{ 'Q', 'Q', 'Q' },
	{ 'F', 'T', 'L' },
	{ 'N', 'P', 'L' },
	{ 'P', 'Y', 'L' },
#else
	MACHINE_HIGH_SCORE_INITIALS
#endif
};


/** Renders a single high score table entry.
 * If pos is zero, then no position is drawn. */
static void high_score_draw_single (U8 pos, const U8 *initials,
	const U8 *score, U8 row)
{
	if (pos != 0)
		sprintf ("%d. %c%c%c", pos, initials[0], initials[1], initials[2]);
	else
		sprintf ("%c%c%c", initials[0], initials[1], initials[2]);
	font_render_string_left (&font_fixed6, 1, row, sprintf_buffer);

	sprintf_score (score);
#ifndef MACHINE_HIGH_SCORE_FONT
#define MACHINE_HIGH_SCORE_FONT font_fixed6
#endif
	font_render_string_right (&MACHINE_HIGH_SCORE_FONT, 128, row, sprintf_buffer);
}


/** Shows the current grand champion score before the final ball. */
void grand_champion_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 8, "HIGHEST SCORE AT");
	sprintf_score (gc_score);
	font_render_string_center (&font_times8, 64, 22, sprintf_buffer);
	dmd_show_low ();
}


/** Shows all of the high scores.  Called from attract mode. */
void high_score_draw_gc (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 8, "GRAND CHAMPION");
	high_score_draw_single (0, gc_initials, gc_score, 20);
	dmd_show_low ();
}

void high_score_draw_12 (void)
{
	dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
	font_render_string_center (&font_mono5, 64, 3, "HIGHEST SCORES");
#endif
	high_score_draw_single (1, high_score_initials[0], highest_scores[0], 8);
	high_score_draw_single (2, high_score_initials[1], highest_scores[1], 20);
	dmd_sched_transition (&trans_vstripe_left2right);
	dmd_show_low ();
}

void high_score_draw_34 (void)
{
	dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
	font_render_string_center (&font_mono5, 64, 3, "HIGHEST SCORES");
#endif
	high_score_draw_single (3, high_score_initials[2], highest_scores[2], 8);
	high_score_draw_single (4, high_score_initials[3], highest_scores[3], 20);
	dmd_sched_transition (&trans_vstripe_left2right);
	dmd_show_low ();
}


/** Reset all of the high scores, including the grand champion,
 * to default values */
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


/** Awards for a high score */
void high_score_award (void)
{
	audit_increment (&system_audits.hstd_credits);
}


void hsentry_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("PLAYER %d", high_score_player);
	font_render_string_center (&font_fixed6, 64, 11, sprintf_buffer);
	font_render_string_center (&font_fixed6, 64, 22, "ENTER INITIALS");
	dmd_sched_transition (&trans_scroll_up);
	dmd_show_low ();
	task_sleep_sec (2);

	dmd_alloc_low_clean ();
	if (high_score_position == 0)
		sprintf ("GRAND CHAMPION");
	else
		sprintf ("HIGH SCORE %d", high_score_position);
	font_render_string_center (&font_var5, 64, 3, sprintf_buffer);
	dmd_sched_transition (&trans_vstripe_left2right);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


void high_score_reset_check (void)
{
}


/* Check to see if the current player score qualifies for the
high score board. */
void high_score_check (void)
{
	U8 hs;

	if (hstd_config.hstd_award == OFF)
		return;

	high_score_player = 1;
	if ((score_compare (current_score, gc_score)) > 0)
	{
		/* This is a grand champion score */
		high_score_position = 0;
		deff_start (DEFF_HSENTRY);
		return;
	}

	for (hs = 0; hs < NUM_HIGH_SCORES; hs++)
		if ((score_compare (current_score, highest_scores[hs])) > 0)
		{
			/* This is a regular high score */
			high_score_position = hs+1;
			deff_start (DEFF_HSENTRY);
			return;
		}
}


CALLSET_ENTRY (high_score, init)
{
	high_score_reset ();
}

