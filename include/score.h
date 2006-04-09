/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
#ifdef MACHINE_SCORE_DIGITS
#define MAX_SCORE_DIGITS MACHINE_SCORE_DIGITS
#else
#define MAX_SCORE_DIGITS 8
#endif

#define BYTES_PER_SCORE ((MAX_SCORE_DIGITS+1)/2)

/** The standard score type, kept in packed BCD, 2 digits per byte */
typedef bcd_t score_t[BYTES_PER_SCORE];

/** Macros for adding to the CURRENT score; these are shortcuts **/

extern U8 score_change;
extern U8 scores[][BYTES_PER_SCORE];
extern U8 *current_score;

void scores_draw (void);
void scores_draw_ball (void);
void scores_draw_current (void);
void scores_deff (void) __taskentry__;
void score_zero (score_t *s);
void score_add (bcd_t *s1, bcd_t *s2, U8 len);
void score_add_current (bcd_t *s);
void score_sub (score_t *s1, score_t *s2);
void score_mul (score_t *s1, uint8_t multiplier);
I8 score_compare (bcd_t *s1, bcd_t *s2);
void scores_reset (void);
void score_init (void);


#define score_decl(val) \
	{ \
		(val & 0xFF000000ULL) >> 24, \
		(val & 0x00FF0000ULL) >> 16, \
		(val & 0x0000FF00ULL) >> 8, \
		(val & 0x000000FFULL) \
	}


#define score_add_current_const(val) \
{ \
	static U8 score[] = score_decl(val ## ULL); \
	score_add_current (score); \
}

#endif /* _SCORE_H */
