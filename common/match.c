/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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
	dmd_alloc_low_clean ();
	font_render_string_right (&font_fixed6, 126, 2, "MATCH");
	sprintf ("%2b", &match_value);
	font_render_string_right (&font_fixed6, 126, 22, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


/** Runs the match sequence if necessary */
void
match_start (void) 
{
	/* TODO - get rid of all these externs, move them into a .h */
	extern const score_t score_table[];
	U8 match_flag;
	U8 starting_match_value;

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

	/* Set the match value so that the above decision holds true.
	 * Start with a random number for the match value, and increase
	 * it until the condition is satisfied. */
	starting_match_value = match_value = random_scaled (10) * 0x10;
	match_count = 0;
	for (;;)
	{
		U8 p;
		for (p = 0; p < num_players; p++)
		{
			if (score_table[p][BYTES_PER_SCORE-1] == match_value)
			{
				/* This player's score matches, and we wanted a match.
				Use this value. */
				if (match_flag == 1)
					break;
				/* Otherwise, we don't want a match; maybe the next
				player's score won't match */
			}
			else
			{
				/* This player's score does not match, and we did not
				want to match.  Use this value. */
				if (match_flag == 0)
					break;
				/* Otherwise, we want a match; maybe the next
				player's score will match */
			}
		}

value_ok:
		break;

next:
		/* Try the next value */
		match_value += 0x10;
		if (match_value > 0x90)
		{
			match_value = 0x00;
		}
		/* Note: this could theoretically be an infinite loop, but
		we must break at some point if the scores aren't corrupted. */
	}

	/* Start the match effect, then wait until it finishes. */
	deff_start (DEFF_MATCH);
	while (deff_get_active () == DEFF_MATCH)
		task_sleep (TIME_100MS);

	/* Award any credits */
	while (match_count > 0)
	{
		match_count--;
		match_award ();
	}
}

