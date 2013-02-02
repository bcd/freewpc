/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
#include <knocker.h>
#include <coin.h>

/**
 * \file
 * \brief Maintains high scores
 *
 */

struct high_score
{
	score_t score;
	U8 initials[HIGH_SCORE_NAMESZ];
};

#define HS_COUNT (NUM_HIGH_SCORES + 1)

/** The high score table */
__nvram__ struct high_score high_score_table[HS_COUNT];


/** A checksum descriptor for the high scores/initials */
struct area_csum high_csum_info = {
	.type = FT_HIGHSCORE,
	.version = 1,
	.area = (U8 *)high_score_table,
	.length = sizeof (high_score_table),
	.reset = high_score_reset,
};

/** The high score auto-reset counters */
__permanent__ U8 hs_reset_counter1, hs_reset_counter2;

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


extern char initials_data[];

#ifdef CONFIG_DMD_OR_ALPHA

/** Renders a single high score table entry.
 * If pos is zero, then no position is drawn. */
static void high_score_draw_single (U8 pos, U8 row)
{
	struct high_score *hsp = &high_score_table[pos];
	if (pos != 0)
		sprintf ("%d. %c%c%c", pos, hsp->initials[0], hsp->initials[1], hsp->initials[2]);
	else
		sprintf ("%c%c%c", hsp->initials[0], hsp->initials[1], hsp->initials[2]);
	font_render_string_left (&font_fixed6, 1, row, sprintf_buffer);

	sprintf_score (hsp->score);
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
	sprintf_score (high_score_table[0].score);
	font_render_string_center (&font_fixed10, 64, 22, sprintf_buffer);
	dmd_show_low ();
}


/** Shows all of the high scores.  Called from attract mode. */
void high_score_draw_gc (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 8, "GRAND CHAMPION");
	high_score_draw_single (0, 20);
	dmd_show_low ();
}

void high_score_draw_12 (void)
{
	dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
	font_render_string_center (&font_mono5, 64, 3, "HIGHEST SCORES");
#endif
	high_score_draw_single (1, 8);
	high_score_draw_single (2, 20);
	dmd_sched_transition (&trans_vstripe_left2right);
	dmd_show_low ();
}

void high_score_draw_34 (void)
{
	dmd_alloc_low_clean ();
#if (MACHINE_DMD == 1)
	font_render_string_center (&font_mono5, 64, 3, "HIGHEST SCORES");
#endif
	high_score_draw_single (3, 8);
	high_score_draw_single (4, 20);
	dmd_sched_transition (&trans_vstripe_left2right);
	dmd_show_low ();
}

#endif

void high_score_check_reset (void)
{
	/* Initialize the counters that will force a reset
	 * automatically after some time */
	/* TODO - what if adjustment is OFF ? */
	hs_reset_counter1 = hstd_config.hs_reset_every;
	hs_reset_counter2 = 250;
}


/** Reset all of the high scores, including the grand champion,
 * to default values */
void high_score_reset (void)
{
	U8 place;

	dbprintf ("Resetting high score table\n");

	/* Reset the grand champion */
	memcpy (high_score_table[0].score, default_gc_score, HIGH_SCORE_WIDTH);
	memcpy (high_score_table[0].initials, default_gc_initials, HIGH_SCORE_NAMESZ);

	/* Reset the other high scores */
	for (place=0; place < NUM_HIGH_SCORES; place++)
	{
		memcpy (high_score_table[place+1].score, default_highest_scores[place],
			HIGH_SCORE_WIDTH);
		memcpy (high_score_table[place+1].initials, default_high_score_initials[place],
			HIGH_SCORE_NAMESZ);
	}

	csum_area_update (&high_csum_info);

	/* Reset when the next auto-reset will occur */
	high_score_check_reset ();
}

#ifdef CONFIG_DMD_OR_ALPHA
void hsentry_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("PLAYER %d", high_score_player);
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	font_render_string_center (&font_fixed6, 64, 22, "ENTER INITIALS");
	dmd_sched_transition (&trans_scroll_up);
	dmd_show_low ();
	task_sleep_sec (2);

#if 0
	dmd_alloc_low_clean ();
	if (high_score_position == 0)
		sprintf ("GRAND CHAMPION");
	else
		sprintf ("HIGH SCORE %d", high_score_position);
	font_render_string_center (&font_var5, 64, 3, sprintf_buffer);
	dmd_sched_transition (&trans_vstripe_left2right);
	dmd_show_low ();
	task_sleep_sec (3);
#endif
	deff_exit ();
}


void hscredits_deff (void)
{
	U8 credits;

	dmd_alloc_low_clean ();

	if (high_score_position == 0)
	{
		sprintf ("GRAND CHAMPION");
		credits = hstd_config.champion_credits;
	}
	else
	{
		sprintf ("HIGH SCORE %d", high_score_position);
		credits = hstd_config.hstd_credits[high_score_position-1];
	}
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);

	if (credits > 0)
	{
		if (credits > 1)
			sprintf ("AWARD %d CREDITS", credits);
		else
			sprintf ("AWARD %d CREDIT", credits);
		font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);
	}

	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}
#endif

/** Check if the high scores need to be reset automatically.
 * Called during game start. */
void high_score_reset_check (void)
{
	dbprintf ("High score reset check: %02X %02X\n",
		hs_reset_counter1, hs_reset_counter2);

	/* If the counters are invalid, factory reset them */
	if (hs_reset_counter1 == 0 || hs_reset_counter2 == 0)
		high_score_check_reset ();

	/* Decrement the counters.  If we hit zero,
	 * restore to maximum and actually reset the high
	 * scores */
	if (--hs_reset_counter2 == 0)
	{
		hs_reset_counter2 = 250;
		if (--hs_reset_counter1 == 0)
		{
			hs_reset_counter1 = hstd_config.hs_reset_every;
			csum_area_reset (&high_csum_info);
		}
	}
}


/**
 * Free a high score entry.
 *
 * If this is not the last entry in the table, it is pushed
 * down recursively.
 */
void high_score_free (U8 position)
{
	if (position < HS_COUNT-1)
		high_score_free (position+1);

	memcpy (&high_score_table[position+1], &high_score_table[position],
		sizeof (struct high_score));
}


/**
 * Check if player PLAYER has qualified for the high score board
 * and add his score to the table if so.
 */
void high_score_check_player (U8 player)
{
	U8 hs;

	for (hs = 0; hs < HS_COUNT; hs++)
	{
		struct high_score *hsp = &high_score_table[hs];
		if ((score_compare (scores[player], hsp->score)) > 0)
		{
			/* The score qualifies for this position.  Push all
			 * scores down and then insert the player score here.
			 * Set the initials to the player number */
			dbprintf ("High score %d achieved by player %d\n",
				hs, player+1);
			pinio_nvram_unlock ();
			high_score_free (hs);
			memcpy (hsp->score, scores[player], sizeof (score_t));
			hsp->initials[0] = player;
			csum_area_update (&high_csum_info);
			pinio_nvram_lock ();
			return;
		}
	}
}


/** Award COUNT credits for achieving a high score */
void high_score_award_credits (U8 *adjptr)
{
	U8 count = *adjptr;
	dbprintf ("Award %d highscore credits\n", count);
	while (count > 0)
	{
		audit_increment (&system_audits.hstd_credits);
		add_credit ();
		knocker_fire ();
		task_sleep_sec (1);
		count--;
	}
}


/** See if the given position in the high score table was modified and
 * needs initials entered.  POSITION is 1-4 for the regular spots
 * and 0 for the grand champion. */
void high_score_enter_initials (U8 position)
{
	struct high_score *hsp = &high_score_table[position];
	if (hsp->initials[0] <= MAX_PLAYERS)
	{
		dbprintf ("High score %d needs initials\n", position);
		/* Announce that player # has qualified */
		high_score_player = hsp->initials[0]+1;

		/* Blank the high score initials until determined */
		pinio_nvram_unlock ();
		memset (hsp->initials, ' ', HIGH_SCORE_NAMESZ);
		pinio_nvram_lock ();
		csum_area_update (&high_csum_info);

		/* Read the player's initials */
		high_score_position = position;
		deff_start_sync (DEFF_HSENTRY);
#ifdef LEFF_HIGH_SCORE
		leff_start (LEFF_HIGH_SCORE);
#endif
		SECTION_VOIDCALL (__common__, initials_enter);

		/* Save the initials to table entry */
		pinio_nvram_unlock ();
		memcpy (hsp->initials, initials_data, HIGH_SCORE_NAMESZ);
		csum_area_update (&high_csum_info);
		pinio_nvram_lock ();

		/* Award credits */
		deff_start (DEFF_HSCREDITS);
		if (position == 0)
		{
			high_score_award_credits (&hstd_config.champion_credits);
		}
		else
		{
			high_score_award_credits (&hstd_config.hstd_credits[position-1]);
		}
		task_sleep (TIME_1500MS);
	}
}



/** Check to see if the current player score qualifies for the
high score board. */
void high_score_check (void)
{
	U8 player;

	/* Don't record high scores if disabled by adjustment */
	if (hstd_config.highest_scores == OFF)
		return;

	/* Give games a chance to disable high scores in other ways */
	if (!callset_invoke_boolean (allow_high_scores))
		return;

	dbprintf ("Checking for high scores\n");

	/* Scan all players, in order from first to last, and see if they
	 * qualify for the high score board.  For each player that
	 * qualifies, insert the score and set the first initial to
	 * the player number, which is a nonprintable character. */
	for (player = 0; player < num_players; player++)
		high_score_check_player (player);

	/* Now that all high scores have been entered, scan the table
	 * one more table to see which initials need to be entered */
	high_score_enter_initials (4);
	high_score_enter_initials (3);
	high_score_enter_initials (2);
	high_score_enter_initials (1);
	high_score_enter_initials (0);
#ifdef LEFF_HIGH_SCORE
	leff_stop (LEFF_HIGH_SCORE);
#endif
}


CALLSET_ENTRY (high_score, file_register)
{
	file_register (&high_csum_info);
}


CALLSET_ENTRY (high_score, sw_buyin_button)
{
#if 0
	high_score_table[4].initials[0] = 0;
	high_score_table[3].initials[0] = 1;
	high_score_enter_initials (4);
	high_score_enter_initials (3);
#endif
}

