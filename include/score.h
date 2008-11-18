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

#ifndef _SCORE_H
#define _SCORE_H

/** Define the number of digits contained in standard-length scores. */
#ifndef MACHINE_SCORE_DIGITS
#define MACHINE_SCORE_DIGITS 10
#endif

/** BYTES_PER_SCORE rounds the number of digits to the next highest
 * multiple of 2, since scores are kept in packed BCD */
#define BYTES_PER_SCORE ((MACHINE_SCORE_DIGITS+1)/2)

/** The standard score type, kept in packed BCD, 2 digits per byte */
typedef bcd_t score_t[BYTES_PER_SCORE];

typedef U8 score_id_t;


/** A flag that is nonzero when the score screen needs to be updated */
extern bool score_update_needed;

/** The array of player scores */
extern score_t scores[];

/** A pointer to the current score in the scores[] array */
extern U8 *current_score;



extern inline void score_update_start (void)
{
	score_update_needed = FALSE;
}

extern inline bool score_update_required (void)
{
	return (score_update_needed);
}

extern inline void score_update_request (void)
{
	score_update_needed = TRUE;
}


__effect__ void scores_draw (void);
__effect__ void scores_draw_ball (void);
__effect__ void scores_draw_current (U8 skip_player);

void score_zero (score_t s);
void score_add (score_t s1, const score_t s2);
void score_add_byte (score_t s1, U8 offset, bcd_t val);
void score_add_current (const bcd_t *s);
void score_add_byte_current (U8 offset, bcd_t val);
void score (score_id_t id);
void score_multiple (score_id_t id, U8 multiplier);
void score_sub (score_t s1, const score_t s2);
void score_mul (score_t s1, U8 multiplier);
I8 score_compare (const score_t s1, const score_t s2);
void scores_reset (void);
void score_init (void);


extern inline void score_10K (U8 count)
{
	score_add_byte_current (3, count);
}

extern inline void score_1M (U8 count)
{
	score_add_byte_current (4, count);
}

extern inline void score_100M (U8 count)
{
	score_add_byte_current (5, count);
}


#endif /* _SCORE_H */
