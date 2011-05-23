/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (sharpe , __machine3__) */
#include <freewpc.h>

/* 
 * Sharpe Shooter Mode:
 * Alternative to BTTZ, Shooter pot starts with 50M, player must hit 2 lit shots,
 * otherwise standups subtract from pot.  Player can choose to continue or
 * collect after every 2 lit shots.  Lit shots must be holdable. 10 standups
 * without a 2 lit shots stops the mode, with a "Pinball is now illegal again"
 * "The court would like Mr Sharpe to hit:"
 */

/* April 1976, Manhatten */

/* Alternative rule, trap ball to start (should be coming out of slot)
 * Check for a flipper hold and no pf closures for 5 seconds
 * The 'Judge' then tells you to hit a shot
 * You get ten seconds to make the shot, if you hit any other shot you lose a
 * life and the timer is reset back to ten seconds.
 * After the third completed shot, you get the option to gamble (double your money?) or
 * collect.
 * If you lose all your lives before collecting a pot, then the Judge will
 * declare pinball is illegal */

/* Start with three lives, lose a life if you miss a shot, you get them back
 * after completing a shot */
U8 sharpe_lives;
U8 sharpe_timer;
U8 sharpe_level;
score_t sharpe_score;

struct {
	const char *shot_name;
	U8 lamp_num;
} sharpe_shots[] = {
	{ "LEFT RAMP", LM_BONUSX },
	{ "RIGHT RAMP", LM_RAMP_BATTLE, },
	{ "PIANO", LM_PIANO_JACKPOT },
	{ "CAMERA", LM_CAMERA },
	{ "POWER PAYOFF", LM_POWER_PAYOFF },
	{ "DEAD END", LM_DEAD_END},
};

void sharpe_start_deff (void)
{
	deff_exit ();
}

void sharpe_running_deff (void)
{
	for (;;)
	{
		score_update_start ();
		dmd_alloc_pair ();
		dmd_clean_page_low ();

		font_render_string_center (&font_term6, 64, 4, "SKILL MULTIBALL");

		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);

		dmd_copy_low_to_high ();

		sprintf ("%d RAMPS FOR SKILL SHOT", sssmb_ramps_to_divert);
		font_render_string_center (&font_var5, 64, 26, sprintf_buffer);

		dmd_show_low ();
		while (!score_update_required ())
		{
			task_sleep (TIME_66MS);
			dmd_show_other ();
		}
	}
}

CALLSET_ENTRY (sharpe, sharpe_start)
{
	global_flag_on (GLOBAL_FLAG_SHARPE_RUNNING);
}

CALLSET_ENTRY (sharpe, sharpe_end, stop_game, end_ball)
{
	global_flag_off (GLOBAL_FLAG_SHARPE_RUNNING);
}

CALLSET_ENTRY (sharpe, lamp_update)
{
	if (global_flag_test (GLOBAL_FLAG_SHARPE_RUNNING))
		lamp_tristate_flash (sharpe_shots[sharpe_level].lamp_num);
}

CALLSET_ENTRY (sharpe, display_update)
{
	if (global_flag_test (GLOBAL_FLAG_SHARPE_RUNNING))
		deff_start_bg (DEFF_SHARPE_RUNNING, 0);
}

CALLSET_ENTRY (sssmb, music_refresh)
{
	if (global_flag_test (GLOBAL_FLAG_SHARPE_RUNNING))
		music_request (MUS_FASTLOCK_EARTHSHAKER, PRI_GAME_MODE1 + 9);
}


CALLSET_ENTRY (sharpe, start_game)
{
	sharpe_lives = 3;
	sharpe_timer = 10;
	sharpe_level = 0;
	score_zero (sharpe_score);
}

CALLSET_ENTRY (sharpe, 
