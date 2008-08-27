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
 * \brief Routines for doing math on scores
 */

#include <freewpc.h>
#include <bcd.h>

extern const score_t score_table[];

/** The array of player scores */
__permanent__ score_t scores[MAX_PLAYERS];

/** A pointer to the score for the current player up */
U8 *current_score;

/** The all-inclusive score multiplier */
U8 global_score_multiplier;


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
	/* Advance to just past the end of each score */
	s1 += BYTES_PER_SCORE;
	s2 += BYTES_PER_SCORE;

	/* Add one byte at a time, however many times it takes */
#if (BYTES_PER_SCORE >= 6)
	bcd_add8 (s1, s2, (BYTES_PER_SCORE == 6) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 5)
	bcd_add8 (s1, s2, (BYTES_PER_SCORE == 5) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 4)
	bcd_add8 (s1, s2, (BYTES_PER_SCORE == 4) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 3)
	bcd_add8 (s1, s2, (BYTES_PER_SCORE == 3) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 2)
	bcd_add8 (s1, s2, (BYTES_PER_SCORE == 2) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 1)
	bcd_add8 (s1, s2, (BYTES_PER_SCORE == 1) ? 0 : 1);
#endif
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


/** Adds to the current score.  The input score is given as a BCD-string. */
void score_add_current (const bcd_t *s)
{
	if (!in_live_game)
	{
		nonfatal (ERR_SCORE_NOT_IN_GAME);
		return;
	}

	score_add (current_score, s);
	score_update_request ();
	replay_check_current ();
}


/** Like score_add_byte, but modifies the current player's score */
void score_add_byte_current (U8 offset, bcd_t val)
{
	if (!in_live_game)
	{
		nonfatal (ERR_SCORE_NOT_IN_GAME);
		return;
	}

	score_add_byte (current_score, offset, val);
	score_update_request ();
	replay_check_current ();
}


/** Adds to the current score, multiplied by some amount.
 * The multiplier is subject to further multiplication by the
 * global score multiplier. */
void score_multiple (score_id_t id, U8 multiplier)
{
	if (!in_live_game)
		return;

	multiplier *= global_score_multiplier;

	/* Some things to consider:
	 * 1. Multiplication is expensive on BCD values.
	 * 2. Multiplied scores will not be common.
	 * 3. Even if the multiplier > 1, it will often be small.
	 *
	 * Considering all of that, it makes sense to ignore
	 * multiplication entirely and just do repeated additions. */
	while (multiplier > 0)
	{
		score_add_current (score_table[id]);
		multiplier--;
	}
	score_update_request ();
}


/** Adds to the current score.  The input score is given as a score ID. */
void score (score_id_t id)
{
	score_multiple (id, 1);
}


void score_sub (score_t s1, const score_t s2)
{
	/* Advance to just past the end of each score */
	s1 += BYTES_PER_SCORE;
	s2 += BYTES_PER_SCORE;

	/* Subtract one byte at a time, however many times it takes */
#if (BYTES_PER_SCORE >= 6)
	bcd_sub8 (s1, s2, (BYTES_PER_SCORE == 6) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 5)
	bcd_sub8 (s1, s2, (BYTES_PER_SCORE == 5) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 4)
	bcd_sub8 (s1, s2, (BYTES_PER_SCORE == 4) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 3)
	bcd_sub8 (s1, s2, (BYTES_PER_SCORE == 3) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 2)
	bcd_sub8 (s1, s2, (BYTES_PER_SCORE == 2) ? 0 : 1);
#endif
#if (BYTES_PER_SCORE >= 1)
	bcd_sub8 (s1, s2, (BYTES_PER_SCORE == 1) ? 0 : 1);
#endif
}


/** Multiply a score (in place) by the given value.
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


/** Reset all scores to zero */
void scores_reset (void)
{
	score_update_start ();
	memset ((U8 *)scores, 0, sizeof (scores));
	current_score = &scores[0][0];
}


CALLSET_ENTRY (score, start_ball)
{
	global_score_multiplier = 1;
}

