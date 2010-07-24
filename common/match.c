/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <knocker.h>
#include <coin.h>

/**
 * \file
 * \brief Implements the match function
 *
 */

/* The number of matches to be given out */
U8 match_count;

/* The selected match value */
bcd_t match_value;


/** Give out the award for a single match. */
void
match_award (void)
{
	audit_increment (&system_audits.match_credits);
	add_credit ();
	knocker_fire ();
}


/** The display effect that runs to show the match.
 * This is a default effect that could be overriden with something
 * more flashy. */
void
match_deff (void)
{
	U8 value;
	U8 n;
	U8 last_value = 0xFF;

	for (n=0; n < 20; n++)
	{
		if (n == 19)
			value = match_value;
		else
		{
			do {
				value = random_scaled (10) * 0x10;
			} while (value == last_value);
		}
<<<<<<< HEAD:common/match.c
=======

>>>>>>> b29378cfc5ca4ff1f372e27072c65e2f3bcc0d9c:common/match.c
		last_value = value;
		dmd_alloc_low_clean ();
#ifdef MACHINE_TZ
		if (n == 19 && match_count)
			sound_send (SND_JET_BUMPER_ADDED);
		else
			sound_send (SND_HITCHHIKER_COUNT);
#endif
		sprintf ("%2b", &scores[0][BYTES_PER_SCORE-1]);
		font_render_string_left (&font_mono5, 0, 0, sprintf_buffer);

		font_render_string_right (&font_fixed6, 126, 2, "MATCH");
		sprintf ("%2b", &value);
		font_render_string_right (&font_fixed6, 126, 22, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_100MS);
	}

	if (match_count)
	{
		callset_invoke (match_awarded);
	}

	task_sleep_sec (2);
	deff_exit ();
}


/**
 * Return a score that says how good a particular two-digit number
 * meets our expectation.  The higher the score, the better it is.
 * WANT_MATCH is nonzero if we want at least one player to match.
 * VAL is the two-digit number we are testing.
 */
U8
match_value_score (U8 want_match, U8 val)
{
	U8 score = 0;
	U8 p;

	/* Count how many players would match if the value
	was chosen */
	for (p = 0; p < num_players; p++)
	{
		if (scores[p][BYTES_PER_SCORE-1] == match_value)
			score++;
	}

	/* If we want to match and this would do that, return 1.
	Otherwise, return zero.  (For now, number of matches
	is not considered.)
	   Likewise, if we don't want to match, return 1 iff
	no matches happened. */
	if (want_match == 2)
	{
	}
	else if (want_match == 1)
	{
		score = (score > 0);
	}
	else
	{
		score = !(score > 0);
	}

	//dbprintf ("%s, %02X = %d\n", want_match ? "match" : "no match", val, score);
	return score;
}


/** Runs the match sequence if necessary */
void
match_start (void)
{
	U8 match_flag;
	U8 score;
	U8 best_score = 0;
	U8 best_match_value = 0;

	/* Nothing to do if match has been disabled */
	if (system_config.match_feature == 0)
		return;

	/* Randomly decide whether or not to award a match.
	 * First, calculate a random number from 0 to 99. */
	match_flag = random_scaled (100);

	/* Now, see whether that value is less than N*P, where N
	is the total number of players and P is the probability of
	a single match. */
	if (match_flag < num_players * system_config.match_feature)
	{
		/* We will match at least one player */
		match_flag = 1;
	}
	else
	{
		/* We will not match any player */
		match_flag = 0;
	}

	/* Find a value for the match that meets our goal.
	Start with a random two-digit value. */
	for (match_value = 0x00; match_value <= 0x90; match_value += 0x10)
	{
		score = match_value_score (match_flag, match_value);
		if ((score > best_score)
			|| ((score == best_score) && random () > 128))
		{
			best_score = score;
			best_match_value = match_value;
		}
	}

	match_value = best_match_value;
	dbprintf ("Chose match value %02X\n", best_match_value);

	/* See how many matches we are about to award. */
	match_count = match_value_score (2, match_value);

	/* Start the match effect, then wait until it finishes. */
	deff_start (DEFF_MATCH);
	while (deff_get_active () == DEFF_MATCH)
		task_sleep (TIME_66MS);

	/* Award any credits */
	while (match_count > 0)
	{
		match_count--;
		match_award ();
	}
}


#if 0 /* for testing match */
CALLSETX_ENTRY (match, sw_buyin_button)
{
	U8 p;
	if (!in_game && !in_test)
	{
		num_players = random_scaled (4) + 1;
		dbprintf ("num_players = %d\n", num_players);
		for (p=0; p < num_players; p++)
		{
			scores[p][BYTES_PER_SCORE-1] = random_scaled (10) * 0x10;
			dbprintf ("player %d = %02X\n", p+1, scores[p][BYTES_PER_SCORE-1]);
		}
		deff_start (DEFF_SCORES_IMPORTANT);
		task_sleep (TIME_1S);
		match_start ();
	}
}
#endif
