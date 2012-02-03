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

/**
 * \file
 * \brief Basic score screen
 */

#include <freewpc.h>
#include <coin.h>


/** Draw the current ball number at the bottom of the display. */
void scores_draw_status_bar (void)
{
	if (config_timed_game)
	{
		U8 min, sec;
		min = 0;
		sec = timed_game_timer;
		while (sec >= 60)
		{
			min++;
			sec -= 60;
		}
		ll_score_draw_timed (min, sec);
	}
	else
	{
		ll_score_draw_ball ();
	}
}


/** Draw the current credit count at the bottom of the display. */
void scores_draw_credits (void)
{
	credits_render ();
	/* TODO - this could also be moved into a ll_ routine, but
	it just happens to work for alphanumeric as well. */
	font_render_string_center (&font_mono5, 64, 29, sprintf_buffer);
}


/**
 * Render the default score screen.
 * skip_player indicates which player is up and should not be drawn.
 * SCORE_DRAW_ALL means to draw everything; otherwise, we do not
 * draw player up's score, so that it can be flashed.
 */
void scores_draw_current (U8 skip_player)
{
	U8 p;

	/* Each player's score is drawn in turn.
	If skip_player is not 0, then it will cause a particular
	player's score *NOT* to be drawn, which can be used to
	implement a single flashing score. */
	for (p=0; p < num_players; p++)
	{
		if (skip_player && p+1 == skip_player)
			continue;

		/* Render the score into the print buffer */
		sprintf_score (scores[p]);

		/* Call the low-level handler for the hardware type */
		ll_scores_draw_current (p);
	}
}


/** Draw the entire score screen statically.  In this mode,
no scores are flashing; everything is fixed. */
void scores_draw (void)
{
	if (in_game && ball_up)
		scores_draw_status_bar ();
	else if (MACHINE_DMD)
		scores_draw_credits ();
	scores_draw_current (SCORE_DRAW_ALL);
}


/** A display effect for showing all the scores, without
flashing.  This is used when new players are added or
at ball start. */
void scores_important_deff (void)
{
	dmd_alloc_low_clean ();
	scores_draw ();
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


/** The normal score screen display effect.  This function redraws the scores
 * in the default manner when there are no other high priority effects
 * running. */
void scores_deff (void)
{
	/* Some machines may want to run a background effect while
	scores are shown (e.g. TZ's starfield, the swimming fish on Fish
	Tales, etc.)  Give the effect a chance to initialize whenever
	the main display effect starts. */
	callset_invoke (score_deff_start);

	/* This effect always runs, until it is preempted. */
	for (;;)
	{
		/* Clear score change flag */
		score_update_start ();

		/* Call the low-level redraw handler */
		ll_score_redraw ();

		/* Display the score with effects, until a score change. */
		for (;;)
		{
			if (valid_playfield)
				ll_score_strobe_valid ();
			else
				ll_score_strobe_novalid ();

			if (score_update_required ())
				break;
		}
	}
}

