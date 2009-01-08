/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>
#include <coin.h>

/* CALLSET_SECTION (score, __effect__) */

/** Nonzero if the current score has changed and needs to be redrawn */
bool score_update_needed;


/** Draw the current ball number at the bottom of the display. */
void scores_draw_ball (void)
{
#if (MACHINE_DMD == 1)
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
	font_render_string_center (&font_var5, 96, 29, sprintf_buffer);
	sprintf ("BALL %1i", ball_up);
	font_render_string_center (&font_var5, 32, 29, sprintf_buffer);
#endif
#else
	sprintf ("BALL %1i", ball_up);
	seg_write_string (1, 10, sprintf_buffer);
#endif
}


/** Draw the current credit count at the bottom of the display. */
void scores_draw_credits (void)
{
	credits_render ();
	font_render_string_center (&font_mono5, 64, 29, sprintf_buffer);
}

#if (MACHINE_DMD == 1)

/* A list of score font keys.  Each of these is an index into the
 * table below. */
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


/** A lookup table for mapping a 'score font key' into a font and
 * location on the DMD. */
const struct score_font_info
{
	/* The function to call to draw the string.  This determines
	the justification */
	void (*render) (void);

	/* The font to be used */
	const font_t *font;

	/* The location where the score should be drawn, subject to
	justification */
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


/* The lookup is [num_players-1][player_up][score_to_draw-1].
 *
 * The first index says how many players there are total.  Zero players
 * is not an option; num_players must always be at least 1.
 *
 * The second index says which player is up.  If zero, it means no
 * player is up, as during attract mode.  This will be nonzero during
 * a game.
 *
 * The third index says which player's score is being drawn.
 */
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
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_TINY, SCORE_POS_LL_TINY, SCORE_POS_LR_TINY, },
		{SCORE_POS_UL_TINY, SCORE_POS_UR_SMALL, SCORE_POS_LL_TINY, SCORE_POS_LR_TINY, },
		{SCORE_POS_UL_TINY, SCORE_POS_UR_TINY, SCORE_POS_LL_SMALL, SCORE_POS_LR_TINY, },
		{SCORE_POS_UL_TINY, SCORE_POS_UR_TINY, SCORE_POS_LL_TINY, SCORE_POS_LR_SMALL, },
	},
};

#endif


/** Render the default score screen. */
void scores_draw_current (U8 single_player)
{
	U8 p;
#if (MACHINE_DMD == 1)
	const struct score_font_info *info;
#endif

	/* Each player's score is drawn in turn.
	If skip_player is not 0, then it will cause a particular
	player's score *NOT* to be drawn, which can be used to
	implement a single flashing score. */
	for (p=0; p < num_players; p++)
	{
		if (single_player && p+1 != single_player)
			continue;

		/* Render the score into the print buffer */
		sprintf_score (scores[p]);

		/* Figure out what font to use and where to print it */
#if (MACHINE_DMD == 1)
		info = &score_font_info_table[
			score_font_info_key[num_players-1][player_up][p] ];

		/* Load the font info into the appropriate registers. */
		DECL_FONTARGS (info->font, info->x, info->y, sprintf_buffer);

		/* Start printing to the display */
		info->render ();
#else /* TODO */
		seg_write_string (0, 0, sprintf_buffer);
#endif
	}
}


/** Draw the entire score screen statically.  In this mode,
no scores are flashing; everything is fixed. */
void scores_draw (void)
{
	if (in_game)
		scores_draw_ball ();
	else if (MACHINE_DMD)
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
 * running. */
void scores_deff (void)
{
#ifdef MACHINE_TZ
	star_reset ();
#endif

	/* This effect always runs, until it is preempted. */
	for (;;)
	{
		/* Clear score change flag */
		score_update_start ();

#if (MACHINE_DMD == 1)
		/* First, the static elements are drawn: the opponents' scores
		 * and the ball number.  Then the flashing element, the current
		 * player's score is drawn. */
		/* TODO - I'd prefer to draw all players without flashing, and
		 * use dark/bright colors to indicate player up. */
		dmd_map_lookaside (0);
		dmd_clean_page_low ();
		scores_draw_ball ();
		scores_draw_current (0);
		dmd_copy_low_to_high ();
		scores_draw_current (player_up);
#else
		seg_alloc ();
		seg_erase ();
		scores_draw_ball ();
		scores_draw_current (0);
		seg_show ();
#endif

		/* Display the score with effects, until a score change. */
		for (;;)
		{
#if (MACHINE_DMD == 1)
			if (valid_playfield)
			{
				dmd_map_lookaside (0);
				dmd_dup_mapped ();
				dmd_overlay2 (wpc_dmd_get_mapped (), 0);
#ifdef MACHINE_TZ
				star_draw ();
#endif
				callset_invoke (score_overlay);
				dmd_show2 ();
			}
			else
			{
				dmd_alloc_low_clean ();
				wpc_dmd_set_high_page (dmd_get_lookaside (0) + 1);
				dmd_copy_page (dmd_low_buffer, dmd_high_buffer);
				dmd_show_low ();
			}
#else
/* TODO */
#endif

			task_sleep (TIME_166MS);

			/* TODO - use a sweeping effect rather than the flashing
			when ball is in play. */
			if (score_update_required ())
				break;
		}
	}
}

