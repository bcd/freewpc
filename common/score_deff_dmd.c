/*
 * Copyright 2008-2010 by Brian Dominy <brian@oddchange.com>
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


void ll_scores_draw_current (U8 player)
{
	const struct score_font_info *info;

	/* Figure out what font to use and where to print it */
	info = &score_font_info_table[
		score_font_info_key[num_players-1][player_up][player] ];

	/* Load the font info into the appropriate registers. */
	DECL_FONTARGS (info->font, info->x, info->y, sprintf_buffer);

	/* Start printing to the display */
	info->render ();
}

void ll_score_draw_timed (U8 min, U8 sec)
{
	sprintf ("TIME REMAINING: %d:%02d", min, sec);
	font_render_string_center (&font_var5, 64, 26, sprintf_buffer);
}

void ll_score_draw_ball (void)
{
	credits_render ();
	font_render_string_center (&font_var5, 96, 29, sprintf_buffer);
	sprintf ("BALL %1i", ball_up);
	font_render_string_center (&font_var5, 32, 29, sprintf_buffer);
}

void ll_score_redraw (void)
{
	dmd_map_overlay ();
	dmd_clean_page_low ();
	scores_draw_status_bar ();
	scores_draw_current (SCORE_DRAW_ALL);
	dmd_copy_low_to_high ();
	scores_draw_current (player_up);
}

void ll_score_strobe_novalid (void)
{
	dmd_alloc_low_clean ();
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, DMD_OVERLAY_PAGE+1);
	dmd_copy_page (dmd_low_buffer, dmd_high_buffer);
	dmd_show_low ();
}

void ll_score_strobe_valid (void)
{
	dmd_map_overlay ();
	dmd_dup_mapped ();
	dmd_overlay_color ();
	callset_invoke (score_overlay);
	dmd_show2 ();
}

