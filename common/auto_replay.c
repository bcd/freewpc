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
#include <replay.h>

/* When auto replay is in effect, the adjustment value that denotes
   its starting value (similar to system_config.replay_start, except
	that it varies). */
__nvram__ U8 auto_replay_adj;

__nvram__ U16 auto_replay_award_count;

/** The number of recent games played */
__nvram__ U16 auto_replay_game_count;

__nvram__ U8 auto_replay_histogram[MAX_REPLAY_SCORES];


/** Reset the tracking of recent scores for calculating the auto replay. */
void auto_replay_reset (void)
{
	auto_replay_award_count = 0;
	auto_replay_game_count = 0;
	auto_replay_adj = system_config.replay_start;
}


/**
 * Adjust the current replay levels based on recent scores.
 *
 * We have auto_replay_award_count / auto_replay_game_count which is the actual
 * percentage of replays given in the recent period, with the current replay score.
 */
static void auto_replay_adjust (void)
{
	/* Recompute auto_replay_adj */

	/* Install the new adjustment */
}


CALLSET_ENTRY (auto_replay, end_player)
{
	U8 i;
	U8 code;
	score_t score;

	if (system_config.replay_system != REPLAY_AUTO)
		return;

	/* Track the total number of replays this player earned during the game. */
	auto_replay_award_count += replay_total_this_player;

	/* Chalk the replay score histogram.  For each possible replay score N,
	increment the stat for N if the player's score exceeded it. */
	for (i=0, code = REPLAY_SCORE_MIN; i < MAX_REPLAY_SCORES; i++, code++)
	{
		replay_code_to_score (score, code);
		if (score_compare (score, current_score) <= 0)
		{
			auto_replay_histogram[i]++;
		}
	}

	/* Increment the number of games played */
	auto_replay_game_count++;
}


CALLSET_ENTRY (auto_replay, end_game)
{
	if (system_config.replay_system != REPLAY_AUTO)
		return;

	/* Add the scores from this game to the score histograms */

	/* After so many games, auto-adjust the replay score if
	necessary */
	if (auto_replay_game_count >= AUTO_REPLAY_ADJUST_RATE)
	{
		auto_replay_adjust ();
		auto_replay_game_count = 0;
	}
}


CALLSET_ENTRY (auto_replay, factory_reset)
{
	auto_replay_reset ();
}

