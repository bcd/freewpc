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

/** Defines for score values */
#define SCORE_10		0x10ULL
#define SCORE_100		0x100ULL
#define SCORE_500		0x500ULL
#define SCORE_1K		0x1000ULL
#define SCORE_2500	0x2500ULL
#define SCORE_5K		0x5000ULL
#define SCORE_7500	0x7500ULL
#define SCORE_10K		0x10000ULL
#define SCORE_20K		0x20000ULL
#define SCORE_25K		0x25000ULL
#define SCORE_30K		0x30000ULL
#define SCORE_40K		0x40000ULL
#define SCORE_50K		0x50000ULL
#define SCORE_75K		0x75000ULL
#define SCORE_100K	0x100000ULL
#define SCORE_200K	0x200000ULL
#define SCORE_250K	0x250000ULL
#define SCORE_300K	0x300000ULL
#define SCORE_400K	0x400000ULL
#define SCORE_500K	0x500000ULL
#define SCORE_750K	0x750000ULL
#define SCORE_1M		0x1000000ULL
#define SCORE_5M		0x5000000ULL


#define SCORE2(tens,ones) \
	((SCORE_10 * tens) + ones)

#define SCORE3(huns,tens,ones) \
	((SCORE_100 * huns) + SCORE2(tens,ones))

#define SCORE4(thous,huns,tens,ones) \
	((SCORE_1K * thous) + SCORE3(huns,tens,ones))

/** Macros for adding to the CURRENT score; these are shortcuts **/

extern U8 score_change;
extern score_t scores[];
extern U8 *current_score;

void scores_draw (void);
void scores_draw_ball (void);
void scores_draw_current (void);
void scores_deff (void) __taskentry__;
void score_zero (score_t *s);
void score_add (bcd_t *s1, bcd_t *s2, U8 len);
void score_add_current (bcd_t *s);
void score_sub (score_t s1, score_t s2);
void score_mul (score_t s1, uint8_t multiplier);
I8 score_compare (score_t s1, score_t s2);
void scores_reset (void);
void score_init (void);


#define score_decl(val) \
	{ \
		((val) & 0xFF000000ULL) >> 24, \
		((val) & 0x00FF0000ULL) >> 16, \
		((val) & 0x0000FF00ULL) >> 8, \
		((val) & 0x000000FFULL) \
	}


#define score_add_current_const(val) \
{ \
	static U8 score[] = score_decl(val); \
	score_add_current (score); \
}

#endif /* _SCORE_H */
