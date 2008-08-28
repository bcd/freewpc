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

#include <freewpc.h>

U8 ball_save_timer;


void ball_save_leff (void)
{
	for (;;)
	{
#ifdef MACHINE_EXTRA_BALL_LAMP
		leff_toggle (MACHINE_EXTRA_BALL_LAMP);
#endif
		task_sleep (TIME_100MS);
	}
}

void ballsave_timer_begin (void)
{
#ifdef LEFF_BALL_SAVE
	leff_start (LEFF_BALL_SAVE);
#endif
}

void ballsave_timer_expire (void)
{
#ifdef LEFF_BALL_SAVE
	leff_stop (LEFF_BALL_SAVE);
#endif
}


void ballsave_timer_task (void)
{
	U8 secs = (U8)task_get_arg ();
	timed_mode_task (ballsave_timer_begin,
		ballsave_timer_expire, NULL,
		&ball_save_timer, secs, 3);
}

void ballsave_add_time (U8 secs)
{
	if (timed_mode_active_p (GID_BALLSAVER_TIMER, &ball_save_timer))
	{
		timed_mode_extend (&ball_save_timer, secs, 20);
	}
	else
	{
		task_pid_t tp = timed_mode_start (GID_BALLSAVER_TIMER,
			ballsave_timer_task);
		task_set_arg (tp, secs);
	}
}

void ballsave_disable (void)
{
	timed_mode_stop (&ball_save_timer);
}


static bool ballsave_test_active (void)
{
	return timed_mode_active_p (GID_BALLSAVER_TIMER, &ball_save_timer);
}


void ballsave_launch (void)
{
#ifdef CONFIG_TZ
	autofire_add_ball ();
#endif
#ifdef DEFF_BALL_SAVE
	deff_start (DEFF_BALL_SAVE);
#endif
	if (config_timed_game)
		timed_game_extend (2);
}


/*
 * On any drain switch, extend the ball save timer so that
 * ball save doesn't timeout before endball is called.
 */

CALLSET_ENTRY (ballsave, sw_left_outlane)
{
	if (live_balls == 1 && ballsave_test_active ())
	{
		ballsave_add_time (5);
	}
}

CALLSET_ENTRY (ballsave, sw_right_outlane)
{
	ballsave_sw_left_outlane ();
}

CALLSET_ENTRY (ballsave, sw_outhole)
{
	ballsave_sw_left_outlane ();
}


/*
 * Default ballsaver is turned on as soon as valid
 * playfield is asserted.
 */
CALLSET_ENTRY (ballsave, valid_playfield)
{
	ballsave_add_time (10);
}

/*
 * Ball save is turned on when any multiball ends.
 */
CALLSET_ENTRY (ballsave, single_ball_play)
{
	ballsave_disable ();
}

/*
 * Ball save is activated at ball drain if
 * it is active.
 */
CALLSET_BOOL_ENTRY (ballsave, ball_drain)
{
	if (ballsave_test_active ())
	{
		ballsave_launch ();
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

