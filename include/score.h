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

/** BYTES_PER_SCORE rounds the number of digits to the next highest
 * multiple of 2, since scores are kept in packed BCD */
#define BYTES_PER_SCORE ((MAX_SCORE_DIGITS+1)/2)

/** The standard score type, kept in packed BCD, 2 digits per byte */
typedef bcd_t score_t[BYTES_PER_SCORE];

/** Score IDs, which are indices into the score table */
typedef enum {
	SC_10,
	SC_100,
	SC_500,
	SC_1K,
	SC_2500,
	SC_5K,
	SC_5130,
	SC_10K,
	SC_15K,
	SC_20K,
	SC_25K,
	SC_30K,
	SC_40K,
	SC_50K,
	SC_75K,
	SC_100K,
	SC_200K,
	SC_250K,
	SC_500K,
	SC_750K,
	SC_1M,
	SC_2M,
	SC_3M,
	SC_4M,
	SC_5M,
	SC_10M,
	SC_20M,
	SC_30M,
	SC_40M,
	SC_50M,
	SC_100M,
	SC_200M,
	SC_250M,
	SC_500M,
	SC_1B,
} score_id_t;


typedef struct
{
	score_t min;
	score_t step;
	score_t max;
} score_ladder_t;

extern U8 score_change;
extern score_t scores[];
extern U8 *current_score;

void scores_draw (void);
void scores_draw_ball (void);
void scores_draw_current (void);
void scores_deff (void) __taskentry__;
void score_zero (score_t *s);
void score_add (bcd_t *s1, bcd_t *s2, U8 len);
void score_add_current (const bcd_t *s);
void score (score_id_t id);
void score_sub (score_t s1, score_t s2);
void score_mul (score_t s1, uint8_t multiplier);
I8 score_compare (score_t s1, score_t s2);
void scores_reset (void);
void score_init (void);

/** Macros for adding to the CURRENT score; these are shortcuts **/

#if 0
#define score_decl(val) \
	{ \
		((val) & 0xFF000000ULL) >> 24, \
		((val) & 0x00FF0000ULL) >> 16, \
		((val) & 0x0000FF00ULL) >> 8, \
		((val) & 0x000000FFULL) \
	}

#define score_ladder_decl(min,step,max) \
	{ \
		.min = score_decl (min), \
		.step = score_decl (step), \
		.max = score_decl (max), \
	}

#define score_add_current_const(val) \
{ \
	static const U8 score[] = score_decl(val); \
	score_add_current (score); \
}
#endif

#define MAKE_BCD(b)	0x##b

#endif /* _SCORE_H */
