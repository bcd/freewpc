/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

extern const score_t score_table[];

/** Nonzero if the current score has changed and needs to be redrawn */
bool score_update_needed;

/* TODO : scores should be in nvram and checksummed */
score_t scores[MAX_PLAYERS];

U8 *current_score;



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

const struct score_font_info 
{
	void (*render) (const fontargs_t *);
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
	struct score_font_info *info;

	for (p=0; p < num_players; p++)
	{
		if (p+1 == skip_player)
			continue;

		sprintf_score (scores[p]);
		
		info = &score_font_info_table[
			score_font_info_key[num_players-1][player_up][p] ];
		DECL_FONTARGS (info->font, info->x, info->y, sprintf_buffer);
		(*info->render) (&font_args);
	}
}


void scores_draw (void)
{
	if (in_game)
		scores_draw_ball ();
	else
		scores_draw_credits ();
	scores_draw_current (0);
}


void scores_important_deff (void)
{
	dmd_alloc_low_clean ();
	scores_draw_ball ();
	scores_draw_current (0);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


void scores_deff (void) __taskentry__
{
	U8 delay;

	for (;;)
	{
redraw:
		/* Clear score change flag */
		score_update_start ();

		/* Stop any score effects (i.e. flashing) */

		/* Redraw the scores on the screen */
		dmd_alloc_low_high ();
		dmd_clean_page_low ();
		scores_draw_ball ();
		scores_draw_current (player_up);
		dmd_copy_low_to_high ();
		scores_draw_current (0);
		dmd_show_low ();
		
		/* Restart score effects */

		/* Wait for a score change */
		for (;;)
		{
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


void score_zero (score_t *s)
{
	memset (s, 0, sizeof (score_t));
}


#ifdef __m6809__
#define __bcd_add8(px, py, carry) \
do { \
	asm volatile ("lda\t,-%0" :: "a" (px)); \
	if (carry == 2) \
		asm volatile ("adca\t#0"); \
	else if (carry == 1) \
		asm volatile ("adca\t,-%0" :: "a" (py)); \
	else \
		asm volatile ("adda\t,-%0" :: "a" (py)); \
	asm volatile ("daa"); \
	asm volatile ("sta\t,%0" :: "a" (px)); \
} while (0)
#else
/* TODO */
#define __bcd_add8(px, py, carry)
#endif


/** Adds one binary-coded decimal score to another. */
void score_add (score_t s1, const score_t s2)
{
	/* Advance to just past the end of each score */
	s1 += BYTES_PER_SCORE;
	s2 += BYTES_PER_SCORE;

	/* Add one byte at a time, however many times it takes */
#if (BYTES_PER_SCORE >= 5)
	__bcd_add8 (s1, s2, 0);
#endif
#if (BYTES_PER_SCORE >= 4)
	__bcd_add8 (s1, s2, 1);
#endif
#if (BYTES_PER_SCORE >= 3)
	__bcd_add8 (s1, s2, 1);
#endif
#if (BYTES_PER_SCORE >= 2)
	__bcd_add8 (s1, s2, 1);
#endif
#if (BYTES_PER_SCORE >= 1)
	__bcd_add8 (s1, s2, 1);
#endif
}


void score_add_byte (score_t s1, U8 offset, bcd_t val)
{
	s1 += BYTES_PER_SCORE - offset;
	__bcd_add8 (s1, &val, 0);
	while (offset < BYTES_PER_SCORE)
	{
		__bcd_add8 (s1, NULL, 2);
	}
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


/** Adds to the current score.  The input score is given as a score ID. */
void score (score_id_t id)
{
	score_add_current (score_table[id]);
}


void score_multiple (score_id_t id, U8 multiplier)
{
	if (!in_live_game)
		return;

	/* TODO - this is cheating */
	while (multiplier > 0)
	{
		score_add (current_score, score_table[id]);
		multiplier--;
	}

#if 0
	/* The proper way is to calculate the product (score_table[id] x N)
	and do a single score_add() of that value.  Also, there is currently no
	way to do a multiplication without adding it to the current score...
	Since packed BCD cannot be multiplied directly, we have to convert the
	BCD value to decimal and then do normal binary multiplication. */
#endif
	score_update_request ();
}


/* score[sizeof(score_t)-1] = ones
 * score[sizeof(score_t)-2] = hundreds
 * score[sizeof(score_t)-3] = ten thousands
 * score[sizeof(score_t)-4] = millions
 */

void score_sub (score_t s1, const score_t s2)
{
	/* TODO */
}

void score_mul (score_t s1, U8 multiplier)
{
	/* TODO */
}


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


void scores_reset (void)
{
	score_update_start ();
	memset ((U8 *)scores, 0, sizeof (scores));
	current_score = &scores[0][0];
}


CALLSET_ENTRY (score, init)
{
	scores_reset ();
}


