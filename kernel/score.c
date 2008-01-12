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
#include <coin.h>
#include <bcd.h>

extern const score_t score_table[];

/** Nonzero if the current score has changed and needs to be redrawn */
bool score_update_needed;

/** The array of player scores */
score_t scores[MAX_PLAYERS];

/** A pointer to the score for the current player up */
U8 *current_score;

/** The all-inclusive score multiplier */
U8 global_score_multiplier;


/** Draw the current ball number at the bottom of the display. */
void scores_draw_ball (void)
{
#if defined (CONFIG_TIMED_GAME)
	U8 time_minutes, time_seconds;
	time_minutes = 0;
	time_seconds = timed_game_timer;
	while (time_seconds >= 60)
	{
		time_minutes++;
		time_seconds -= 60;
	}
	sprintf ("TIME REMAINING: %d:%02d", time_minutes, time_seconds);
	font_render_string_center (&font_var5, 64, 26, sprintf_buffer);
#else
	credits_render ();
	font_render_string_center (&font_var5, 96, 27, sprintf_buffer);
	sprintf ("BALL %1i", ball_up);
	font_render_string_center (&font_var5, 32, 27, sprintf_buffer);
#endif
}


/** Draw the current credit count at the bottom of the display. */
void scores_draw_credits (void)
{
	credits_render ();
	font_render_string_center (&font_mono5, 64, 27, sprintf_buffer);
}


#define SCORE_POS_CENTER_LARGE 0
#define SCORE_POS_UL_SMALL 1
#define SCORE_POS_UR_SMALL 2
#define SCORE_POS_LL_SMALL 3
#define SCORE_POS_LR_SMALL 4
#define SCORE_POS_UL_LARGE 5
#define SCORE_POS_LR_LARGE 6
#define SCORE_POS_LL_LARGE 7
#define SCORE_POS_UR_LARGE 8
#define SCORE_POS_UL_TINY 9
#define SCORE_POS_LR_TINY 10
#define SCORE_POS_LL_TINY 11
#define SCORE_POS_UR_TINY 12


/** A lookup table for mapping a 'score font key' into a font and location on the DMD. */
const struct score_font_info 
{
	void (*render) (void);
	const font_t *font;
	U8 x;
	U8 y;
} score_font_info_table[] = {
	[SCORE_POS_CENTER_LARGE] = { fontargs_render_string_center, &font_lucida9, 64, 10 },

	[SCORE_POS_UL_LARGE] = { fontargs_render_string_left, &font_lucida9, 0, 1 },
	[SCORE_POS_UR_LARGE] = { fontargs_render_string_right, &font_lucida9, 127, 1 },
	[SCORE_POS_LL_LARGE] = { fontargs_render_string_left, &font_lucida9, 0, 10 },
	[SCORE_POS_LR_LARGE] = { fontargs_render_string_right, &font_lucida9, 127, 10 },

	[SCORE_POS_UL_SMALL] = { fontargs_render_string_left, &font_mono5, 0, 1 },
	[SCORE_POS_UR_SMALL] = { fontargs_render_string_right, &font_mono5, 127, 1 },
	[SCORE_POS_LL_SMALL] = { fontargs_render_string_left, &font_mono5, 0, 16 },
	[SCORE_POS_LR_SMALL] = { fontargs_render_string_right, &font_mono5, 127, 16 },

	[SCORE_POS_UL_TINY] = { fontargs_render_string_left, &font_tinynum, 0, 1 },
	[SCORE_POS_UR_TINY] = { fontargs_render_string_right, &font_tinynum, 127, 1 },
	[SCORE_POS_LL_TINY] = { fontargs_render_string_left, &font_tinynum, 0, 16 },
	[SCORE_POS_LR_TINY] = { fontargs_render_string_right, &font_tinynum, 127, 16 },
};


/* The lookup is [num_players-1][player_up][score_to_draw-1] */
const U8 score_font_info_key[4][5][4] = {
	/* 1 player */  {
		{SCORE_POS_UL_SMALL},
		{SCORE_POS_CENTER_LARGE},
	},
	/* 2 players */ {
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL }, 
		{SCORE_POS_UL_LARGE, SCORE_POS_LR_SMALL }, 
		{SCORE_POS_UL_SMALL, SCORE_POS_LR_LARGE }
	},
	/* 3 players */ {
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL, SCORE_POS_LL_SMALL }, 
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_TINY, SCORE_POS_LL_TINY }, 
		{SCORE_POS_UL_TINY, SCORE_POS_UR_SMALL, SCORE_POS_LL_SMALL }, 
		{SCORE_POS_UL_TINY, SCORE_POS_UR_TINY, SCORE_POS_LL_LARGE }, 
	},
	/* 4 players */ {
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL, SCORE_POS_LL_SMALL, 
			SCORE_POS_LR_SMALL }, 
	},
};


/** Render the default score screen. */
void scores_draw_current (U8 skip_player)
{
	U8 p;
	const struct score_font_info *info;

	/* Each player's score is drawn in turn.
	If skip_player is not 0, then it will cause a particular
	player's score *NOT* to be drawn, which can be used to
	implement a single flashing score. */
	for (p=0; p < num_players; p++)
	{
		if (p+1 == skip_player)
			continue;

		/* Render the score into the print buffer */
		sprintf_score (scores[p]);
	
		/* Figure out what font to use and where to print it */
		info = &score_font_info_table[
			score_font_info_key[num_players-1][player_up][p] ];

		/* Load the font info into the appropriate registers. */	
		DECL_FONTARGS (info->font, info->x, info->y, sprintf_buffer);

		/* Start printing to the display */
		info->render ();
	}
}


/** Draw the entire score screen statically.  In this mode,
no scores are flashing; everything is fixed. */
void scores_draw (void)
{
	if (in_game)
		scores_draw_ball ();
	else
		scores_draw_credits ();
	scores_draw_current (0);
}


/** A display effect for showing all the scores, without
flashing.  This is used when new players are added or
at ball start. */
void scores_important_deff (void)
{
	dmd_alloc_low_clean ();
	scores_draw_ball ();
	scores_draw_current (0);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


/** The score screen display effect.  This function redraws the scores
 * in the default manner when there are no other high priority effects
 * running.  In ths mode, the current player's score will flash. */
void scores_deff (void)
{
	U8 delay;

	/* This effect always runs, until it is preempted. */
	for (;;)
	{
redraw:
		/* Clear score change flag */
		score_update_start ();

		/* Stop any score effects (i.e. flashing) */

		/* Redraw the scores.  player_up signals which score
		 * should be drawn larger and will flash.
		 *
		 * First, the static elements are drawn: the opponents' scores
		 * and the ball number.  Then the flashing element, the current
		 * player's score is drawn. */
		dmd_alloc_low_high ();
		dmd_clean_page_low ();
		scores_draw_ball ();
		scores_draw_current (player_up);
		dmd_copy_low_to_high ();
		scores_draw_current (0); /* TODO - this is redrawing other players ! */
		dmd_show_low ();
		
		/* Restart score effects */

		/* Wait for a score change. */
		for (;;)
		{
			/* TODO - use a sweeping effect rather than the flashing
			when ball is in play. */
			delay = ball_in_play ? TIME_500MS : TIME_100MS;
			while (delay != 0)
			{
				task_sleep (TIME_33MS);
				delay -= TIME_33MS;
				if (score_update_required ())
					goto redraw;
			}
			dmd_show_other ();

			delay = ball_in_play ? TIME_200MS : TIME_100MS;
			while (delay != 0)
			{
				task_sleep (TIME_33MS);
				delay -= TIME_33MS;
				if (score_update_required ())
					goto redraw;
			}
			dmd_show_other ();
		}
	}
}


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


CALLSET_ENTRY (score, init)
{
	/* TODO : scores should persist across a reset */
	scores_reset ();
}


