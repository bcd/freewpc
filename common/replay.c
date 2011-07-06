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
#include <bcd_string.h>

/** The current replay scores */
__nvram__ U8 replay_score_array[NUM_REPLAY_LEVELS][BYTES_PER_SCORE];

/** The replay module NVRAM descriptor */
const struct area_csum replay_csum_info = {
	.type = FT_REPLAY,
	.version = 1,
	.area = (U8 *)replay_score_array,
	.length = sizeof (replay_score_array),
	.reset = replay_reset,
};


/** The number of replays already awarded to the current player
during this game.  This is also an index into the replay_score_array,
zero-based, which says what the next replay level to be awarded is */
__local__ U8 replay_award_count;

/** The total number of replay awards for all players during this game */
U8 replay_total_this_game;


/* The next replay score, used during a game - this is per-player */
#define next_replay_score replay_score_array[replay_award_count]



/*
 * Code 0 means OFF.
 * Code 1 is the minimum value.
 */

struct replay_code_info
{
	U16 min;
	U16 step;
	U16 max;
	U16 deflt;
	U16 step10;
};


/* Describe valid values for replay values */
struct replay_code_info replay_score_code_info = {
	.min = REPLAY_SCORE_MIN,
	.step = REPLAY_SCORE_STEP,
	.max = REPLAY_SCORE_MAX,
	.deflt = REPLAY_SCORE_DEFAULT,
	.step10 = REPLAY_SCORE_STEP << 4,
};


/**
 * Convert a 8-bit replay adjustment code into a BCD score.
 * Use the given info structure to guide the conversion.
 * This function is used for both actual replay score values, and for
 * replay boost values; we use a different info in each case.
 */
void default_replay_code_convert (score_t score, U8 code,
	struct replay_code_info *info)
{
	bcd_t *loc;

	score_zero (score);
	if (code == 0)
		return;
	code--;
#ifdef REPLAY_MILLIONS
	loc = score;
#else
	loc = score + 1;
#endif
	bcd_string_add (loc, (U8 *)&info->min, sizeof (U16));
	while (code >= 10)
	{
		bcd_string_add (loc, (U8 *)&info->step10, sizeof (U16));
		code -= 10;
	}
	while (code > 0)
	{
		bcd_string_add (loc, (U8 *)&info->step, sizeof (U16));
		code--;
	}
}


/**
 * Convert an 8-bit replay score adjustment into BCD form.
 * The old MACHINE_REPLAY_CODE_TO_SCORE hook is still supported for now,
 * but should not be used for new games.
 */
void replay_code_to_score (score_t score, U8 code)
{
#ifdef MACHINE_REPLAY_CODE_TO_SCORE
	MACHINE_REPLAY_CODE_TO_SCORE (score, code);
#else
	default_replay_code_convert (score, code, &replay_score_code_info);
#endif
}


/**
 * Convert an 8-bit, non-boolean replay boost adjustment into BCD.
 */

#ifndef CONFIG_REPLAY_BOOST_BOOLEAN
struct replay_code_info replay_boost_code_info = {
	.min = REPLAY_BOOST_MIN,
	.step = REPLAY_BOOST_STEP,
	.max = REPLAY_BOOST_MAX,
	.deflt = REPLAY_BOOST_DEFAULT,
	.step10 = REPLAY_BOOST_STEP << 4,
};

void replay_code_to_boost (score_t score, U8 code)
{
	default_replay_code_convert (score, code, &replay_boost_code_info);
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
	/* TODO - during a game, if there are multiple replay levels, we should show
	the player what the next replay is at, not the first one */
	sprintf_score (replay_score_array[0]);
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
	if (system_config.replay_award != FREE_AWARD_OFF &&
		replay_award_count < NUM_REPLAY_LEVELS &&
		score_compare (next_replay_score, current_score) <= 0)
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


/** Reset the replay scores to their initial values. */
void replay_reset (void)
{
	U8 replay_code;
	U8 level;
	U8 multiplier;

	/* Repeat for each of the possible replay levels. */
	for (level = 0; level < NUM_REPLAY_LEVELS; level++)
	{
		/* Get the correct adjustment code and multiplier.
		 * This depends on the replay system in effect (auto or fixed). */
		if (system_config.replay_system == REPLAY_AUTO)
		{
			if (system_config.replay_levels >= level)
				continue;
			/* TODO - this clears out any auto reflexing */
			replay_code = system_config.replay_start;
			multiplier = level;
		}
		else
		{
			replay_code = system_config.replay_level[level];
			if (replay_code == 0)
				continue;
			multiplier = 1;
		}

		/* Convert and store in BCD form */
		pinio_nvram_unlock ();
		score_zero (replay_score_array[level]);
		replay_code_to_score (replay_score_array[level], replay_code);
		score_mul (replay_score_array[level], multiplier);
		pinio_nvram_lock ();
	}
}

CALLSET_ENTRY (replay, start_game)
{
	replay_total_this_game = 0;
}

CALLSET_ENTRY (replay, start_player)
{
	replay_award_count = 0;
}

CALLSET_ENTRY (replay, end_player)
{
	replay_total_this_game += replay_award_count;
}

CALLSET_ENTRY (replay, end_game)
{
	/* Is replay boost enabled?  If so, and one or more replays
	were awarded in this game, then increase the replay value temporarily.
	Note, replay boost is canceled when credits equal 0, a coin is inserted,
	or test mode started. */
	if (system_config.replay_boost && replay_total_this_game)
	{
	}
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



