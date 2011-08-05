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

#ifndef _SCORE_H
#define _SCORE_H

/** Define the number of digits contained in standard-length scores. */
#ifndef MACHINE_SCORE_DIGITS
#define MACHINE_SCORE_DIGITS 10
#endif

/** BYTES_PER_SCORE rounds the number of digits to the next highest
 * multiple of 2, since scores are kept in packed BCD */
#define BYTES_PER_SCORE ((MACHINE_SCORE_DIGITS+1)/2)

#define SCORE_DRAW_ALL 0

/** The standard score type, kept in packed BCD, 2 digits per byte */
typedef bcd_t score_t[BYTES_PER_SCORE], *score_pointer_t;

typedef const bcd_t *const const_score_pointer_t;

typedef U8 score_id_t;

/**
 * The generic ladder tracks an award whose values increase in
 * an irregular way.
 */
struct generic_ladder
{
	/* The number of scores in the ladder */
	const U8 size;

	/* An array of score values */
	const score_id_t *scores;

	/* A pointer to the level tracker.  The level is an index
	into the above array which gives the next score to be awarded */
	U8 *level;
};


/**
 * The fixed ladder tracks an award whose values increase by
 * fixed size steps.
 */
struct fixed_ladder
{
	const_score_pointer_t base;
	const_score_pointer_t increment;
	const_score_pointer_t max;
	score_pointer_t current;
};


/** A flag that is nonzero when the score screen needs to be updated */
extern bool score_update_needed;

/** The array of player scores */
extern score_t scores[];

/** A pointer to the current score in the scores[] array */
extern U8 *current_score;

/** The last score that was awarded, used by display effects */
extern score_t last_score;

/** The all-inclusive score multiplier */
extern U8 global_score_multiplier;

/** A pointer to the fixed score table */
extern const score_t score_table[];


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

extern inline void score_update_wait (void)
{
	while (!score_update_required ())
		task_sleep (TIME_133MS);
	score_update_start ();
}

#define when_score_changes for (score_update_start ();; score_update_wait ())

#define score_deff_sleep(duration) \
	do { \
		task_sleep (duration); \
		if (score_update_required ()) \
			return; \
	} while (0)

/** Portable score display effect routines */
__effect__ void scores_draw (void);
__effect__ void scores_draw_ball (void);
__effect__ void scores_draw_current (U8 skip_player);
__effect__ void scores_draw_status_bar (void);

/** External low-level functions.  These differ between DMD and
alphanumeric games. */
__effect__ void ll_score_change_player (void);
__effect__ void ll_scores_draw_current (U8);
__effect__ void ll_score_redraw (void);
__effect__ void ll_score_draw_timed (U8 min, U8 sec);
__effect__ void ll_score_draw_ball (void);
__effect__ void ll_score_strobe_novalid (void);
__effect__ void ll_score_strobe_valid (void);

void score_zero (score_t s);
void score_copy (score_t dst, const score_t src);
void score_add (score_t s1, const score_t s2);
void score_add_byte (score_t s1, U8 offset, bcd_t val);
void score_sub (score_t s1, const score_t s2);
void score_mul (score_t s1, U8 multiplier);
I8 score_compare (const score_t s1, const score_t s2);

void score_award_compact (U8 offset, bcd_t val);

void score (score_id_t id);
void score_multiple (score_id_t id, U8 multiplier);

void score_long_unmultiplied (const score_t score);
void score_long_multiple (const score_t score, U8 multiplier);
void score_long (const score_t score);

void score_deff_set (void);
score_pointer_t score_deff_get (void);

void score_multiplier_set (U8 m);

void scores_reset (void);
void score_init (void);

extern inline void score_10K (U8 count)
{
	score_award_compact (3, count);
}

extern inline void score_1M (U8 count)
{
	score_award_compact (4, count);
}

extern inline void score_100M (U8 count)
{
	score_award_compact (5, count);
}

void generic_ladder_reset (const struct generic_ladder *ladder);
void generic_ladder_advance (const struct generic_ladder *ladder);
void generic_ladder_score (const struct generic_ladder *ladder);
void generic_ladder_score_and_advance (const struct generic_ladder *ladder);

void fixed_ladder_reset (const struct fixed_ladder *ladder);
void fixed_ladder_advance (const struct fixed_ladder *ladder);
void fixed_ladder_scorex (const struct fixed_ladder *ladder, U8 multiplier);
void fixed_ladder_score (const struct fixed_ladder *ladder);
void fixed_ladder_score_and_advance (const struct fixed_ladder *ladder);
void fixed_ladder_scorex_and_advance (const struct fixed_ladder *ladder, U8 multiplier);

#endif /* _SCORE_H */
