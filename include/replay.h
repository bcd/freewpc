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

#ifndef _REPLAY_H
#define _REPLAY_H

/* The maximum number of replays per player per game */
#ifndef NUM_REPLAY_LEVELS
#define NUM_REPLAY_LEVELS 4
#endif

/* The number of games after which the auto replay score is recalculated */
#define AUTO_REPLAY_ADJUST_RATE 50

/* The maximum number of different replay score values that can be set. */
#define MAX_REPLAY_SCORES 32

/* The type of a replay score.  We only store at most 2 bytes of
   BCD; the least significant digits of the score are not saved
   and do not affect comparisons. */
typedef U8 replay_score_t[2];

/* Because of the size of replay_score_t, the maximum possible replay
   value is limited. */
#define MAX_REPLAY_VALUE 9999UL
#define MAX_REPLAY_VALUE_BCD 0x9999UL


/* Convert a constant, 16-bit decimal value to 2 byte BCD at
   compile-time. */
#define __U16_TO_BCD2(u16) \
	((((u16 % 10000UL) / 1000UL) * 0x1000UL) + \
	 (((u16 % 1000UL) / 100) * 0x100) + \
	 (((u16 % 100) / 10) * 0x10) + \
	 (u16 % 10))
#ifdef __m6809__
#define U16_TO_BCD2(u16) __U16_TO_BCD2(u16)
#else
#define U16_TO_BCD2(u16) ((__U16_TO_BCD2(u16) >> 8) | ((__U16_TO_BCD2(u16) & 0xFF) << 8))
#endif

extern __local__ U8 replay_total_this_player;
__common__ void replay_draw (void);
__common__ void replay_award (void);
__common__ void replay_check_current (void);
__common__ void replay_info_reset (void);
__common__ bool replay_can_be_awarded (void);
__common__ void replay_code_to_score (score_t score, U8 code);
#ifndef CONFIG_REPLAY_BOOST_BOOLEAN
__common__ void replay_code_to_boost (score_t, U8);
#endif


/* Replay scores can be configured in the adjustments menu, but the
   adjustments themselves are only 8-bit values.  You must define a method
	for how to convert an adjustment level to an actual score.

   Level 0 is reserved and means OFF, or no replay value.  Otherwise you
	can define the mapping however you like, as long as increasing values
	refer to higher replay scores.

   First, you must define MACHINE_REPLAY_SCORE_CHOICES to the number
	of valid values.  Then, use one of the following methods to define the
	mapping.

	The most flexible method is to define a function which converts the
	value.  Then set MACHINE_REPLAY_CODE_TO_SCORE in the .md file to tell
	the OS to call it when necessary.
 */


/* Provide default parameters to the replay system.
   Machines should normally override these values.
   All values are given in decimal; use U16_TO_BCD2 when you
   need a BCD-value. */

#if !defined(REPLAY_MILLIONS) && !defined(REPLAY_TEN_THOUSANDS)
#if (BYTES_PER_SCORE >= 5)
#define REPLAY_MILLIONS
#else
#define REPLAY_TEN_THOUSANDS
#endif
#endif

#ifndef REPLAY_SCORE_DEFAULT
#define REPLAY_SCORE_MIN 10
#define REPLAY_SCORE_STEP 2
#define REPLAY_SCORE_MAX 50
#define REPLAY_SCORE_DEFAULT 20
#define REPLAY_BOOST_MIN 1
#define REPLAY_BOOST_STEP 1
#define REPLAY_BOOST_MAX 10
#define REPLAY_BOOST_DEFAULT 5
#endif

/* Compute other constants based on the configuration.
   The REPLAY_SCORE_TYPE_xxx constants mean the same thing
   as the parameters above, but they express the encoding as
   the adjustment system stores them. */

#define REPLAY_SCORE_TO_TYPE(score) \
	(1 + ((score - REPLAY_SCORE_MIN) / REPLAY_SCORE_STEP))

#define REPLAY_SCORE_TYPE_MIN 0
#define REPLAY_SCORE_TYPE_MAX (1 + REPLAY_SCORE_TO_TYPE(REPLAY_SCORE_MAX))
#define REPLAY_SCORE_TYPE_DEFAULT REPLAY_SCORE_TO_TYPE(REPLAY_SCORE_DEFAULT)

#define REPLAY_BOOST_TO_TYPE(boost) \
	(1 + ((boost - REPLAY_BOOST_MIN) / REPLAY_BOOST_STEP))

#define REPLAY_BOOST_TYPE_MIN 0
#define REPLAY_BOOST_TYPE_MAX (1 + REPLAY_BOOST_TO_TYPE(REPLAY_BOOST_MAX))
#define REPLAY_BOOST_TYPE_DEFAULT REPLAY_BOOST_TO_TYPE(REPLAY_BOOST_DEFAULT)

/* Define the offset within a score_t at which the replay value
   can be compared.  This depends on the units of the replay, and
   the length of scores. */
#ifdef REPLAY_MILLIONS
#define REPLAY_SCORE_OFFSET (BYTES_PER_SCORE - 5)
#endif
#ifdef REPLAY_TEN_THOUSANDS
#define REPLAY_SCORE_OFFSET (BYTES_PER_SCORE - 4)
#endif

#endif /* _REPLAY_H */

