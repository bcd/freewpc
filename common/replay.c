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

/**
 * \file
 * \brief Detect and award replays.
 */

#include <freewpc.h>
#include <replay.h>
#include <knocker.h>
#include <coin.h>
#include <eb.h>

/** The current replay scores */
__nvram__ U8 replay_score_array[NUM_REPLAY_LEVELS][BYTES_PER_SCORE];

/** The default replay score.  This is only used if the machine does not
define a method for taking replay scores from the standard adjustment. */
const score_t default_replay_score = { 0x00, 0x50, 0x00, 0x00, 0x00 };


/** The replay checksum descriptor */
const struct area_csum replay_csum_info = {
	.type = FT_REPLAY,
	.version = 1,
	.area = replay_score_array,
	.length = BYTES_PER_SCORE,
	.reset = replay_reset,
};


/** The number of replays already awarded to the current player
during this game.  This is also an index into the replay_score_array,
zero-based, which says what the next replay level to be awarded is */
__local__ U8 replay_award_count;

/* The first replay score, used in attract mode */
#define first_replay_score replay_score_array[0]

/* The next replay score, used during a game - this is per-player */
#define next_replay_score replay_score_array[replay_award_count]


#ifdef MACHINE_REPLAY_CODE_INCREMENT
void replay_code_convert (score_t score, U8 code)
{
#if (MACHINE_REPLAY_CODE_INCREMENT == REPLAY_CODE_MILLION)
	score_add_byte (score, 4, code);
#endif
}
#endif


/** Draw the replay screen, as used in attract mode */
void replay_draw (void)
{
	dmd_alloc_low_clean ();
	switch (system_config.replay_award)
	{
		case FREE_AWARD_CREDIT:
			font_render_string_center (&font_fixed6, 64, 8, "REPLAY AT");
			break;
		case FREE_AWARD_EB:
			font_render_string_center (&font_fixed6, 64, 8, "EXTRA BALL AT");
			break;
		case FREE_AWARD_TICKET:
			font_render_string_center (&font_fixed6, 64, 8, "TICKET AT");
			break;
		case FREE_AWARD_OFF:
		default:
			return;
	}
	sprintf_score (first_replay_score);
	font_render_string_center (&font_fixed10, 64, 22, sprintf_buffer);
	dmd_show_low ();
}


/** Award a single replay to the player up */
void replay_award (void)
{
	callset_invoke (replay);
	switch (system_config.replay_award)
	{
		case FREE_AWARD_CREDIT:
			add_credit ();
			break;

		case FREE_AWARD_EB:
			increment_extra_balls ();
			break;

		case FREE_AWARD_OFF:
			break;
	}
#ifdef DEFF_REPLAY
	deff_start (DEFF_REPLAY);
#endif
#ifdef LEFF_REPLAY
	leff_start (LEFF_REPLAY);
#endif

	audit_increment (&system_audits.replays);
	replay_award_count++;
	knocker_fire ();
}


/** Check if the current score has exceeded the next replay level,
 * and a replay needs to be awarded */
void replay_check_current (void)
{
	if (system_config.replay_award != FREE_AWARD_OFF
		&& replay_award_count == 0
		&& score_compare (next_replay_score, current_score) <= 0)
	{
		replay_award ();
	}
}


/** Returns true if it is possible to give out a replay award.
 * Returns false if not for some reason. */
bool replay_can_be_awarded (void)
{
	return (system_config.replay_award != FREE_AWARD_OFF
		&& replay_award_count == 0);
}


/** Reset the replay score to its default value. */
void replay_reset (void)
{
	U8 replay_code;

	replay_code = system_config.replay_start;
#ifdef MACHINE_REPLAY_CODE_TO_SCORE
	score_zero (first_replay_score);
	MACHINE_REPLAY_CODE_TO_SCORE (first_replay_score, replay_code);
#else
	memcpy (first_replay_score, default_replay_score, sizeof (score_t));
#endif
}


CALLSET_ENTRY (replay, start_player)
{
	replay_award_count = 0;
}


CALLSET_ENTRY (replay, end_game)
{
	/* Add the scores from this game to the score histograms */

	/* After so many games, auto-adjust the replay score if
	necessary */
}


CALLSET_ENTRY (replay, init)
{
	/* Initialize the replay score from the menu adjustment. */
	csum_area_reset (&replay_csum_info);
}


CALLSET_ENTRY (replay, amode_start)
{
	/* Reinitialize the replay levels from the adjustments. */
	csum_area_reset (&replay_csum_info);
}


CALLSET_ENTRY (replay, file_register)
{
	file_register (&replay_csum_info);
}

