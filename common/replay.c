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

/* At present, only debug in simulation since we run out of ROM space
   on the 6809 */
//#ifndef __m6809__
//#define REPLAY_DEBUG
//#endif

#if defined(REPLAY_DEBUG) && defined(DEBUGGER)
#define rp_debug(fmt, rest...) dbprintf(fmt, ## rest)
#else
#define rp_debug(fmt, rest...)
#endif

struct replay_info {
	/* The current configured replay levels, without boost */
	replay_score_t base_levels[NUM_REPLAY_LEVELS];

	/* The amount of boost that will be applied anytime a
	replay is achieved during a game */
	replay_score_t boost;

	/* The current active levels, including boost */
	score_t score_array[NUM_REPLAY_LEVELS];

	/* The adjustment code which set the base level when
	   auto replay is in effect */
	U8 auto_adj;
};
__nvram__ struct replay_info replay_info;


/** The replay module checksum descriptor */
const struct area_csum replay_csum_info = {
	.type = FT_REPLAY,
	.version = 1,
	.area = (U8 *)&replay_info,
	.length = sizeof (replay_info),
	.reset = replay_info_reset,
};


/** The number of replays already awarded to the current player
during this game.  This is also an index into the score array,
zero-based, which says what the next replay level to be awarded is */
__local__ U8 replay_total_this_player;

/** The total number of replay awards for all players during this game */
U8 replay_total_this_game;

/* The next replay score, used during a game - this is per-player */
#define next_replay_score replay_info.score_array[replay_total_this_player]


/* The replay_code_info structure encodes all of the compile-time
   parameters for a replay value/replay boost value in BCD form.
   It allows common code to be used on either type. */
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
	.min = U16_TO_BCD2(REPLAY_SCORE_MIN),
	.step = U16_TO_BCD2(REPLAY_SCORE_STEP),
	.max = U16_TO_BCD2(REPLAY_SCORE_MAX),
	.deflt = U16_TO_BCD2(REPLAY_SCORE_DEFAULT),
	.step10 = U16_TO_BCD2(REPLAY_SCORE_STEP) << 4,
};


/**
 * Convert a 8-bit replay adjustment code into a BCD score.
 * Use the given info structure to guide the conversion.
 * This function is used for both actual replay score values, and for
 * replay boost values; we use a different info in each case.
 */
static void default_replay_code_convert (score_t score, U8 code,
	struct replay_code_info *info)
{
	bcd_t *loc;

	score_zero (score);
	if (code == 0)
		return;
	code--;
	loc = score + REPLAY_SCORE_OFFSET;
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
	.min = U16_TO_BCD2(REPLAY_BOOST_MIN),
	.step = U16_TO_BCD2(REPLAY_BOOST_STEP),
	.max = U16_TO_BCD2(REPLAY_BOOST_MAX),
	.deflt = U16_TO_BCD2(REPLAY_BOOST_DEFAULT),
	.step10 = U16_TO_BCD2(REPLAY_BOOST_STEP) << 4,
};

void replay_code_to_boost (score_t score, U8 code)
{
	default_replay_code_convert (score, code, &replay_boost_code_info);
}
#endif


/** Draw the replay screen */
void replay_draw (void)
{
	const char *header;

	switch (system_config.replay_award)
	{
		case FREE_AWARD_CREDIT:
			header = "REPLAY AT";
			break;
		case FREE_AWARD_EB:
			header = "EXTRA BALL AT";
			break;
		case FREE_AWARD_TICKET:
			header = "TICKET AT";
			break;
		case FREE_AWARD_OFF:
		default:
			return;
	}

	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 8, header);
	sprintf_score (replay_info.score_array[in_game ? replay_total_this_player : 0]);
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
	replay_total_this_player++;
	knocker_fire ();
}


/** Check if the current score has exceeded the next replay level,
 * and a replay needs to be awarded */
void replay_check_current (void)
{
	replay_score_t *curr;

	if (unlikely (system_config.replay_award == FREE_AWARD_OFF))
		return;

	if (unlikely (replay_total_this_player >= NUM_REPLAY_LEVELS))
		return;

	curr = (replay_score_t *)(current_score + REPLAY_SCORE_OFFSET);
	if (unlikely (*curr >= next_replay_score))
	{
		replay_award ();
	}
}


/** Returns true if it is possible to give out a replay award.
 * Returns false if not for some reason. */
bool replay_can_be_awarded (void)
{
	return (system_config.replay_award != FREE_AWARD_OFF
		&& replay_total_this_player == 0);
}


/* Initialize the replay_info structure.
 * This struct is populated with data from the adjustment system,
 * which is already being checksummed to detect corruption.  This
 * function does not attempt to duplicate that, but instead converts
 * the adjustment data into more usable form.
 */
void replay_info_update (void)
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
			if (level >= system_config.replay_levels)
			{
				rp_debug ("rp #%d skip\n", level);
				continue;
			}
			replay_code = replay_info.auto_adj;
			multiplier = level + 1;
		}
		else
		{
			replay_code = system_config.replay_level[level];
			if (replay_code == 0)
				continue;
			multiplier = 1;
		}

		/* Convert and store in BCD form */
		rp_debug ("rp #%d code=%d mult=%d \n", level, replay_code, multiplier);
		pinio_nvram_unlock ();
		score_zero (replay_info.score_array[level]);
		replay_code_to_score (replay_info.score_array[level], replay_code);
		score_mul (replay_info.score_array[level], multiplier);
		/* TBD - add boost */
		pinio_nvram_lock ();

#ifdef REPLAY_DEBUG
		sprintf_score (replay_info.score_array[level]);
		dbprintf1 ();
		dbprintf ("\n");
#endif
	}
}

void replay_boost_reset (void)
{
	/* Copy base levels into score array */
}

void replay_boost_now (void)
{
	/* Increment each score array entry by the boost value */
}

void replay_info_reset (void)
{
	replay_boost_reset ();
}

CALLSET_ENTRY (replay, start_game)
{
	replay_total_this_game = 0;
}

CALLSET_ENTRY (replay, start_player)
{
	replay_total_this_player = 0;
}

CALLSET_ENTRY (replay, end_player)
{
	replay_total_this_game += replay_total_this_player;
}

CALLSET_ENTRY (replay, end_game)
{
	/* Is replay boost enabled?  If so, and one or more replays
	were awarded in this game, then increase the replay value temporarily.
	Note, replay boost is canceled when credits equal 0, a coin is inserted,
	or test mode started. */
	if (system_config.replay_boost && replay_total_this_game)
	{
		/* ? Does boost apply if a game is aborted on ball 3? */
		replay_boost_now ();
	}
	else if (get_credits () == 0)
	{
		replay_boost_reset ();
	}
}

CALLSET_ENTRY (replay, init)
{
	replay_info_update ();
}

CALLSET_ENTRY (replay, test_start, add_credits, add_partial_credits)
{
	replay_boost_reset ();
}


CALLSET_ENTRY (replay, file_register)
{
	file_register (&replay_csum_info);
}


CALLSET_ENTRY (replay, adjustment_changed)
{
	/* Handle changes to any replay-related adjustments. */

	/* Copy the new replay start value into the working area. */
	if (last_adjustment_changed == &system_config.replay_start)
	{
		pinio_nvram_unlock ();
		replay_info.auto_adj = system_config.replay_start;
		pinio_nvram_lock ();
	}

	/* Recalculate all replay levels if any replay-related adjustment
	   changes */
	if (last_adjustment_changed >= &system_config.replay_system &&
		last_adjustment_changed <= &system_config.replay_boost)
	{
		replay_info_update ();
	}
}

