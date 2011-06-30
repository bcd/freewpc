/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

/**
 * An array that describes the sort order of all scores.
 * Each entry refers to a particular player, just like in the scores array.
 * The value here is the rank; i.e. 0x01 for first place, and so on.
 */
U8 score_ranks[MAX_PLAYERS];

/*
 * The previous rank for the current player up
 */
U8 prev_rank;


/**
 * For debugging.
 */
void score_rank_dump (void)
{
#ifdef DEBUG_SCORE_RANK
	U8 n;
	for (n=0; n < MAX_PLAYERS; n++)
	{
		dbprintf ("%d: %10b %d\n", n, scores+n, score_ranks[n]);
	}
#endif
}


/**
 * Recompute score ranks.
 */
void score_rank_update (void)
{
	U8 i, j, highest;

	/* Recompute score_ranks.
	I is the rank we are trying to find, starting from first place
	down to last place.
	J is the player we are currently examining.
	*/
	memset (score_ranks, 0, sizeof (score_ranks));
	for (i=1; i <= num_players; i++)
	{
		/* The ith position is the largest score that we
		haven't already assigned a rank to */
		highest = -1;
		for (j=0; j < num_players; j++)
		{
			if (score_ranks[j])
				continue;
			if (highest == -1)
				highest = j;
			else if (score_compare (scores[j], scores[highest]) > 0)
				highest = j;
		}
		score_ranks[highest] = i;
	}
}


/**
 * Ensure that rankings are recalculated and correct at the start
 * of each ball, and at the end of the game.
 */
CALLSET_ENTRY (score_rank, start_ball, end_game)
{
	if (num_players > 1)
	{
		score_rank_update ();
		if (in_game)
			prev_rank = score_ranks[player_up-1];
		score_rank_dump ();
	}
}


/**
 * Periodically, update the rankings while a game is in progress.
 * Check for cases where it is impossible for the rankings to have
 * changed since the last call.
 */
CALLSET_ENTRY (score_rank, idle_every_second)
{
	/* Don't update unless we're in a game */
	if (!in_live_game)
		return;

	/* Don't update if we haven't reached valid playfield.
	Scores shouldn't be changing (much) at this point. */
	if (!valid_playfield)
		return;

	/* Don't update if it's a 1-player game.  Rankings are
	kind of pointless in this case. */
	if (num_players == 1)
		return;

	/* Don't update if the current player was already the leader.
	This assumes that points cannot be deducted, which is true for now. */
	prev_rank = score_ranks[player_up-1];
	if (prev_rank == 1)
		return;

	/* OK, something might have changed.  Recompute and if his rank
	changed, announce it */
	score_rank_update ();
	score_rank_dump ();
	if (score_ranks[player_up-1] != prev_rank)
	{
		dbprintf ("Player %d is now in %d place\n", player_up,
			score_ranks[player_up-1]);
		callset_invoke (rank_change);
	}
}

