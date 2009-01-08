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

/**
 * \file
 * \brief Detect and award replays.
 */

#include <freewpc.h>
#include <knocker.h>
#include <coin.h>

/** The current replay score */
__nvram__ U8 replay_score[BYTES_PER_SCORE];

/** The checksum for the current replay score */
__nvram__ U8 replay_csum;


/** The default replay score.  This is only used if the machine does not
define a method for taking replay scores from the standard adjustment. */
const score_t default_replay_score = { 0x00, 0x50, 0x00, 0x00, 0x00 };


/** The replay checksum descriptor */
const struct area_csum replay_csum_info = {
	.area = replay_score,
	.length = BYTES_PER_SCORE,
	.csum = &replay_csum,
	.reset = replay_reset,
#ifdef HAVE_PAGING
	.reset_page = PAGE,
#endif
};


/** The number of replays awarded to the current player */
__local__ U8 replay_award_count;


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
	sprintf_score (replay_score);
	font_render_string_center (&font_times8, 64, 22, sprintf_buffer);
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
		&& score_compare (replay_score, current_score) <= 0)
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
	wpc_nvram_get ();
#ifdef MACHINE_REPLAY_CODE_TO_SCORE
	score_zero (replay_score);
	MACHINE_REPLAY_CODE_TO_SCORE (replay_score, replay_code);
#else
	memcpy (replay_score, default_replay_score, sizeof (score_t));
#endif
	wpc_nvram_put ();
	csum_area_update (&replay_csum_info);
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
	replay_reset ();
}


CALLSET_ENTRY (replay, amode_start)
{
	/* Initialize the replay score from the menu adjustment. */
	replay_reset ();
}

