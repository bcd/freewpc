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

#include <freewpc.h>

void ball_save_leff (void)
{
#if defined (INFINITE_BALL_SAVER)
	for (;;)
#elif defined (CONFIG_TIMED_GAME)
start:
	while (timed_game_timer > 5)
#else
	while (timer_find_gid (GID_BALLSAVER_TIMER))
#endif
	{
		leff_on (LM_SHOOT_AGAIN);
		task_sleep (TIME_100MS);
		leff_off (LM_SHOOT_AGAIN);
		task_sleep (TIME_100MS);
	}

#ifdef CONFIG_TIMED_GAME
	while (timed_game_timer != 0)
	{
		task_sleep (TIME_500MS);
		if (timed_game_timer >= 5)
			goto start;
	}
#endif
	leff_exit ();
}

void ballsave_enable (void)
{
#ifndef CONFIG_TIMED_GAME
	timer_restart_free (GID_BALLSAVER_TIMER, TIME_10S);
#endif
	leff_start (LEFF_BALL_SAVE);
}

void ballsave_disable (void)
{
#ifndef INFINITE_BALL_SAVER
	task_kill_gid (GID_BALLSAVER);
	leff_stop (LEFF_BALL_SAVE);
#endif
}

bool ballsave_test_active (void)
{
#ifdef CONFIG_TIMED_GAME
	if (timed_game_timer > 5)
		return TRUE;
#endif
	return timer_find_gid (GID_BALLSAVER) ? TRUE : FALSE;
}


void ballsave_launch (void)
{
	extern void autofire_add_ball (void);

	autofire_add_ball ();
	deff_start (DEFF_BALL_SAVE);
#ifdef CONFIG_TIMED_GAME
	timed_game_extend (2);
#else
	ballsave_disable ();
#endif
}

CALLSET_ENTRY (ballsave, sw_left_outlane)
{
#ifndef CONFIG_TIMED_GAME
	if (timer_find_gid (GID_BALLSAVER_TIMER))
		timer_restart_free (GID_BALLSAVER_TIMER, TIME_10S);
#endif
}

CALLSET_ENTRY (ballsave, sw_right_outlane)
{
	ballsave_sw_left_outlane ();
}

CALLSET_ENTRY (ballsave, sw_outhole)
{
	ballsave_sw_left_outlane ();
}

CALLSET_ENTRY (ballsave, ball_in_play)
{
	/* start ballsaver if enabled */
	ballsave_enable ();
}


CALLSET_BOOL_ENTRY(ballsave, end_ball_check)
{
	if (ballsave_test_active ())
	{
		ballsave_launch ();
		return FALSE;
	}
	else
		return TRUE;
}

