/*
 * Copyright 2008-2011 by Brian Dominy <brian@oddchange.com>
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

#define CONFIG_SCORE_SWEEP
#define CONFIG_SCORES_COLOR

/* The starting row for the status bar */
#define DMD_STATUS_ROW 29

/* How often the display is updated when sweeping the score.  We run about as fast
   as the human eye can perceive. */
#define LL_SWEEP_RATE TIME_33MS

/* The rate at which the score overlay is redrawn */
#define SCORE_OVERLAY_RATE TIME_166MS

/* How long we delay after a score change, before it begins to sweep */
#define LL_SWEEP_INIT_DELAY (TIME_2S / SCORE_OVERLAY_RATE)

/* How long we delay between one sweep and the next */
#define LL_SWEEP_REFRESH_DELAY (TIME_1S / SCORE_OVERLAY_RATE)

/* The number of iterations between overlay updates */
#define LL_STROBE_OVERLAY_RATE (SCORE_OVERLAY_RATE / LL_SWEEP_RATE)


/* A list of score font keys.  Each of these is an index into the
 * table below.  A key identifies a location on the DMD where
 * a score can be placed, and its font size there. */
enum score_font_key
{
	SCORE_POS_CENTER_LARGE,
	SCORE_POS_UL_SMALL,
	SCORE_POS_UR_SMALL,
	SCORE_POS_LL_SMALL,
	SCORE_POS_LR_SMALL,
	SCORE_POS_UL_LARGE,
	SCORE_POS_LR_LARGE,
	SCORE_POS_LL_LARGE,
	SCORE_POS_UL_TINY,
	SCORE_POS_LR_TINY,
	SCORE_POS_LL_TINY,
	SCORE_POS_UR_TINY,
	SCORE_POS_UL_MED,
	SCORE_POS_LR_MED,
	SCORE_POS_LL_MED,
	SCORE_POS_UR_MED,
};


/** A lookup table for mapping a 'score font key' into a font and
 * location on the DMD. */
const struct score_font_info
{
	/* The function to call to draw the string.  This determines
	the justification */
	void (*render) (const char *);

	/* The font to be used */
	const font_t *font;

	/* The location where the score should be drawn, subject to
	justification */
	U8 x;
	U8 y;

	/* The base pointer of the bounding box */
	U16 bb_base;

	/* The width of the bounding box in bytes */
	U8 bb_width;

	/* The height of the bounding box in rows (equal to font height) */
} score_font_info_table[] = {
	[SCORE_POS_CENTER_LARGE] = { fontargs_render_string_center, &font_lucida9, 64, 10, 3*DMD_BYTE_WIDTH, DMD_BYTE_WIDTH },

#define LROW(fontsize) (DMD_STATUS_ROW - 3 - (fontsize))
	[SCORE_POS_UL_LARGE] = { fontargs_render_string_left, &font_lucida9, 0, 1, 1*DMD_BYTE_WIDTH, DMD_BYTE_WIDTH },
	[SCORE_POS_LL_LARGE] = { fontargs_render_string_left, &font_lucida9, 0, LROW(13), LROW(13UL)*DMD_BYTE_WIDTH, DMD_BYTE_WIDTH },
	[SCORE_POS_LR_LARGE] = { fontargs_render_string_right, &font_lucida9, 127, LROW(13), LROW(13UL)*DMD_BYTE_WIDTH, DMD_BYTE_WIDTH },

	[SCORE_POS_UL_MED] = { fontargs_render_string_left, &font_mono9, 0, 1, 1*DMD_BYTE_WIDTH, 8 },
	[SCORE_POS_UR_MED] = { fontargs_render_string_right, &font_mono9, 127, 1, 1*DMD_BYTE_WIDTH+8, 8 },
	[SCORE_POS_LL_MED] = { fontargs_render_string_left, &font_mono9, 0, LROW(8), LROW(8UL)*DMD_BYTE_WIDTH, 8 },
	[SCORE_POS_LR_MED] = { fontargs_render_string_right, &font_mono9, 127, LROW(8), LROW(8UL)*DMD_BYTE_WIDTH+8, 8 },

	[SCORE_POS_UL_SMALL] = { fontargs_render_string_left, &font_mono5, 0, 1 },
	[SCORE_POS_UR_SMALL] = { fontargs_render_string_right, &font_mono5, 127, 1 },
	[SCORE_POS_LL_SMALL] = { fontargs_render_string_left, &font_mono5, 0, LROW(6) },
	[SCORE_POS_LR_SMALL] = { fontargs_render_string_right, &font_mono5, 127, LROW(6) },

	[SCORE_POS_UL_TINY] = { fontargs_render_string_left, &font_tinynum, 0, 1 },
	[SCORE_POS_UR_TINY] = { fontargs_render_string_right, &font_tinynum, 127, 1 },
	[SCORE_POS_LL_TINY] = { fontargs_render_string_left, &font_tinynum, 0, LROW(5) },
	[SCORE_POS_LR_TINY] = { fontargs_render_string_right, &font_tinynum, 127, LROW(5) },
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
const enum score_font_key score_font_info_key[MAX_PLAYERS][MAX_PLAYERS+1][MAX_PLAYERS] = {
	/* 1 player */  {
		{SCORE_POS_UL_SMALL},
		{SCORE_POS_CENTER_LARGE},
	},
	/* 2 players */ {
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL },
		{SCORE_POS_UL_LARGE, SCORE_POS_LR_MED },
		{SCORE_POS_UL_MED, SCORE_POS_LR_LARGE }
	},
	/* 3 players */ {
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL, SCORE_POS_LL_SMALL },
		{SCORE_POS_UL_MED, SCORE_POS_UR_SMALL, SCORE_POS_LL_SMALL },
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_MED, SCORE_POS_LL_SMALL },
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL, SCORE_POS_LL_LARGE },
	},
	/* 4 players */ {
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL, SCORE_POS_LL_SMALL,
			SCORE_POS_LR_SMALL },
		{SCORE_POS_UL_MED, SCORE_POS_UR_SMALL, SCORE_POS_LL_SMALL, SCORE_POS_LR_SMALL, },
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_MED, SCORE_POS_LL_SMALL, SCORE_POS_LR_SMALL, },
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL, SCORE_POS_LL_MED, SCORE_POS_LR_SMALL, },
		{SCORE_POS_UL_SMALL, SCORE_POS_UR_SMALL, SCORE_POS_LL_SMALL, SCORE_POS_LR_MED, },
	},
};


const U8 *dmd_score_info_base;

U16 ll_dmd_sweep_addr;

U8 ll_sweep_row;

U8 ll_sweep_off;

U8 ll_sweep_width;

U8 ll_sweep_overlay_needed;

U8 ll_dmd_sweep_counter;


__attribute__((noinline)) void ll_score_change_player (void)
{
	/* Take a pointer to the score key for index 0 (not in game).
	 * This calculation only needs to happen when num_players or
	 * player_up changes. */
	dmd_score_info_base = &score_font_info_key[num_players-1][player_up][0];
}


void ll_scores_draw_current (U8 player)
{
	const struct score_font_info *info;

	/* Get score info for the desired player */
	info = score_font_info_table + dmd_score_info_base[player];

	/* Load the font info into the appropriate registers. */
	DECL_FONTARGS (info->font, info->x, info->y, sprintf_buffer);

	/* Start printing to the display */
	info->render (sprintf_buffer);
}


#ifdef CONFIG_TIMED_GAME
void ll_score_draw_timed (U8 min, U8 sec)
{
	sprintf ("TIME REMAINING: %d:%02d", min, sec);
	font_render_string_center (&font_var5, 64, DMD_STATUS_ROW, sprintf_buffer);
}
#endif


void ll_score_draw_ball (void)
{
	credits_render ();
	font_render_string_center (&font_var5, 96, DMD_STATUS_ROW, sprintf_buffer);
	sprintf ("BALL %1i", ball_up);
	font_render_string_center (&font_var5, 32, DMD_STATUS_ROW, sprintf_buffer);
}


/* Reinitialize the score sweep after a full cycle */
static void ll_score_sweep_rescan (void)
{
	const struct score_font_info *info;
	info = score_font_info_table + dmd_score_info_base[player_up-1];
	ll_dmd_sweep_addr = info->bb_base;
	ll_sweep_row = far_read8 ((U8 *)info->font + 1, FON_PAGE) + 1;
	ll_sweep_width = info->bb_width;
}


/* Initialize the score sweep after a score change */
static void ll_score_sweep_init (void)
{
	ll_dmd_sweep_counter = LL_SWEEP_INIT_DELAY;
	ll_sweep_overlay_needed = LL_STROBE_OVERLAY_RATE;
	ll_sweep_off = TRUE;
	ll_score_sweep_rescan ();
}


/* Redraw the active score screen.
 * This function does not actually show the scores, but renders them
 * to the overlay buffer for showing later.
 * Before valid playfield, we also render a second screen without the
 * current player's score; this allows it to be flashed.
 */
void ll_score_redraw (void)
{
	if (valid_playfield)
	{
		dmd_map_overlay ();
		dmd_clean_page_low ();
		scores_draw_status_bar ();
		scores_draw_current (SCORE_DRAW_ALL);
		dmd_copy_low_to_high ();
		/* Take care that player_up != 0 */
		ll_score_sweep_init ();
	}
	else
	{
		dmd_alloc_pair ();
		dmd_clean_page_low ();
		scores_draw_status_bar ();
		dmd_copy_low_to_high ();
		scores_draw_current (player_up);
		dmd_flip_low_high ();
		scores_draw_current (SCORE_DRAW_ALL);
		dmd_show_low ();
	}
}



/* Strobe the display during the score deff, prior to valid playfield. */
void ll_score_strobe_novalid (void)
{
	dmd_show_other ();
	score_deff_sleep (TIME_133MS);
}


/* Strobe the display during the score deff, after valid playfield. */
void ll_score_strobe_valid (void)
{
#ifdef CONFIG_SCORE_SWEEP
	if (ll_dmd_sweep_counter == 0)
	{
		/* Copy the previous page - scores and overlay */
		dmd_dup_mapped ();

		/* Modify the current score area - either erase a line, or add back a line
		that was erased earlier. */
		if (ll_sweep_off)
		{
			memset (dmd_low_buffer + ll_dmd_sweep_addr, 0, ll_sweep_width);
		}
		else
		{
			pinio_dmd_window_set (PINIO_DMD_WINDOW_1, DMD_OVERLAY_PAGE);
			memcpy (dmd_low_buffer + ll_dmd_sweep_addr,
				dmd_high_buffer + ll_dmd_sweep_addr, ll_sweep_width);
			pinio_dmd_window_set (PINIO_DMD_WINDOW_1,
				pinio_dmd_window_get (PINIO_DMD_WINDOW_0) + 1);
		}

#ifdef CONFIG_SCORES_COLOR
		dmd_copy_low_to_high ();
#endif

		/* Draw the overlay periodically */
		if (--ll_sweep_overlay_needed == 0)
		{
			ll_sweep_overlay_needed = LL_STROBE_OVERLAY_RATE;
			callset_invoke (score_overlay);
		}

		/* Show the new frame */
#ifdef CONFIG_SCORES_COLOR
		dmd_show2 ();
#else
		dmd_show_low ();
#endif

		/* Advance to the next row */
		ll_dmd_sweep_addr += DMD_BYTE_WIDTH;
		if (--ll_sweep_row == 0)
		{
			ll_score_sweep_rescan ();
			ll_sweep_off = !ll_sweep_off;
			if (ll_sweep_off == TRUE)
			{
				ll_dmd_sweep_counter = LL_SWEEP_REFRESH_DELAY;
				score_deff_sleep (TIME_100MS);
				return;
			}
		}

		score_deff_sleep (LL_SWEEP_RATE);
		return;
	}

	ll_dmd_sweep_counter--;
#endif

#ifdef CONFIG_SCORES_COLOR
	//dmd_map_overlay ();
	dmd_dup_mapped ();
	dmd_overlay_onto_color ();
	callset_invoke (score_overlay);
	dmd_show2 ();
#else
	dmd_map_overlay ();
	dmd_dup_mapped ();
	dmd_overlay ();
	callset_invoke (score_overlay);
	dmd_show_low ();
#endif
	score_deff_sleep (TIME_133MS);
}


/**
 * Update the score screen font information when necessary.
 * If player_up or num_players changes, we need to do this.
 */
CALLSET_ENTRY (dmd_score, init_complete, start_game, start_ball, add_player, amode_start, stop_game)
{
	ll_score_change_player ();
}

