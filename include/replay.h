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
#define NUM_REPLAY_LEVELS 4

/* The number of games after which the auto replay score is recalculated */
#define AUTO_REPLAY_ADJUST_RATE 50

/* The maximum number of different replay score values that can be set. */
#define MAX_REPLAY_SCORES 32

extern __local__ U8 replay_award_count;
__common__ void replay_draw (void);
__common__ void replay_award (void);
__common__ void replay_check_current (void);
__common__ void replay_reset (void);
__common__ bool replay_can_be_awarded (void);
__common__ void replay_code_to_score (score_t score, U8 code);

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

#define REPLAY_MILLIONS /* default for 5 digit scores */
//#define REPLAY_TEN_THOUSANDS /* default for 4 digit scores */

/* 10, 12, 14, 16, 18,
20, 22, 24, 26, 28,
30, 32, 34, 36, 38,
40, 42, 44, 46, 48, 50 */

#ifdef __m6809__
#define REPLAYVAL(x) x ## UL
#else
#define REPLAYVAL(x) ((x >> 8) | ((x & 0xFF) << 8))
#endif

#define REPLAY_SCORE_MIN REPLAYVAL(0x0010)
#define REPLAY_SCORE_STEP REPLAYVAL(0x0002)
#define REPLAY_SCORE_MAX REPLAYVAL(0x0050)
#define REPLAY_SCORE_DEFAULT REPLAYVAL(0x0020)
#define REPLAY_SCORE_TYPE_MIN 0
#define REPLAY_SCORE_TYPE_MAX 22 /* 21 score levels */
#define REPLAY_SCORE_TYPE_DEFAULT 6 /* 20M */

#endif /* _REPLAY_H */
