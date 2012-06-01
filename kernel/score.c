/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Routines for doing math on scores
 */

#include <freewpc.h>
#include <bcd_string.h>
#include <replay.h>

/** The array of player scores */
__permanent__ score_t scores[MAX_PLAYERS];

/** A pointer to the score for the current player up */
U8 *current_score;

/** The all-inclusive score multiplier */
U8 global_score_multiplier;

/** The last score award, with multipliers applied */
score_t last_score;

/** The multiplier attached to the last score award.  This does not
factor in the global score multiplier. */
U8 last_multiplier;

/** The score award that is being displayed.  This is a latched
 * version of last_score. */
score_t deff_score;

/** The last multiplier as displayed */
U8 deff_multiplier;

/** Nonzero if the current score has changed and needs to be redrawn */
bool score_update_needed;


/** Clears a score */
void score_zero (score_t s)
{
	memset (s, 0, sizeof (score_t));
}


/** Copy from one score to another */
void score_copy (score_t dst, const score_t src)
{
	memcpy (dst, src, sizeof (score_t));
}


/** Adds one binary-coded decimal score to another. */
void score_add (score_t s1, const score_t s2)
{
	bcd_string_add (s1, s2, BYTES_PER_SCORE);
}


/** Increments a binary-coded decimal score by another value
 * in which only one byte is nonzero (e.g. 40K = 04 00 00).
 * 'offset' identifies the position of the nonzero byte, with
 * zero always meaning the 'ones' byte, and higher values
 * referring to larger place values.  'val' gives the byte
 * value. */
void score_add_byte (score_t s1, U8 offset, bcd_t val)
{
	score_t s2;

	memset (s2, 0, sizeof (score_t));
	s2[BYTES_PER_SCORE - offset] = val;
	score_add (s1, s2);
}



/** Subtracts one binary-coded decimal score from another. */
void score_sub (score_t s1, const score_t s2)
{
	bcd_string_sub (s1, s2, BYTES_PER_SCORE);
}


/** Multiply a score (in place) by the given integer.
 * Zero is not supported, as it should never be called this
 * way. */
void score_mul (score_t s, U8 multiplier)
{
	/* If multiplier is 1, nothing needs to be done. */
	if (multiplier > 1)
	{
		/* Otherwise, we need to perform 'multiplier-1'
		 * additions of the value into itself.  This is
		 * not the most elegant way, but multiplications
		 * are not common, and the multipliers are often
		 * going to be small. */
		score_t copy;
		score_copy (copy, s);

		do {
			score_add (s, copy);
		} while (--multiplier > 1);
	}
}


/** Compares two scores.  Returns -1, 0, or 1 accordingly, like memcmp. */
I8 score_compare (const score_t s1, const score_t s2)
{
	register U8 len = sizeof (score_t);

	while (len > 0)
	{
		if (*s1 > *s2)
			return (1);
		else if (*s1 < *s2)
			return (-1);
		else
		{
			s1++;
			s2++;
			len--;
		}
	}
	return (0);
}


/** Adds to the current score.  The input score is given as a BCD-string. */
static void score_award (const bcd_t *s)
{
	if (in_tilt || in_test)
		return;
	if (!in_game)
	{
		nonfatal (ERR_SCORE_NOT_IN_GAME);
		return;
	}

	score_add (current_score, s);
	score_update_request ();
	replay_check_current ();
}


/** Like score_add_byte, but modifies the current player's score.
 * This function is analogous to score_award(). */
void score_award_compact (U8 offset, bcd_t val)
{
	U8 mult;

	if (in_tilt || in_test)
		return;
	if (!in_game)
	{
		nonfatal (ERR_SCORE_NOT_IN_GAME);
		return;
	}

	mult = global_score_multiplier;
	do {
		score_add_byte (current_score, offset, val);
		mult--;
	} while (mult != 0);
	score_update_request ();
	replay_check_current ();
}


/** The first of three general scoring APIs.  score_long_multiplied
adds to the current player's score without applying *any*
multipliers. */
void score_long_unmultiplied (const score_t score)
{
	score_copy (last_score, score);
	last_multiplier = 1;
	score_award (last_score);
}

/** Like above, but the global score multiplier is in effect and
an additional multiplier can be specified too. */
void score_long_multiple (const score_t score, U8 multiplier)
{
	score_copy (last_score, score);
	last_multiplier = multiplier;
	score_mul (last_score, multiplier);
	score_mul (last_score, global_score_multiplier);
	score_award (last_score);
}

/** Like score_long_multiple, without an additional multiplier.
The global multiplier still applies. */
void score_long (const score_t score)
{
	score_long_multiple (score, 1);
}


/** Adds to the current score, multiplied by some amount.
 * The multiplier is subject to further multiplication by the
 * global score multiplier. */
void score_multiple (score_id_t id, U8 multiplier)
{
	score_long_multiple (score_table[id], multiplier);
}


/** Like score_long, except the input score is given as a score ID
instead of a long BCD string. */
void score (score_id_t id)
{
	score_long_multiple (score_table[id], 1);
}

/** Called by the display effect module when starting an effect which
wants to display the last score (it has the D_SCORE flag).  The score
is saved to separate storage so that future scores do not clobber it. */
void score_deff_set (void)
{
	score_copy (deff_score, last_score);
	deff_multiplier = last_multiplier;
}

/** Called by a display effect to get the last score */
bcd_t *score_deff_get (void)
{
	return deff_score;
}


/** Reset all scores to zero */
void scores_reset (void)
{
	score_update_start ();
	memset ((U8 *)scores, 0, sizeof (scores));
	current_score = &scores[0][0];
}

void score_multiplier_set (U8 m)
{
	if (m == 0)
	{
		nonfatal (ERR_ZERO_SCORE_MULT);
		m = 1;
	}
	global_score_multiplier = m;
}

CALLSET_ENTRY (score, start_ball)
{
	score_multiplier_set (1);
	score_update_request ();
}

CALLSET_ENTRY (score, factory_reset)
{
	scores_reset ();
}

CALLSET_ENTRY (score, init)
{
	current_score = &scores[0][0];
}

