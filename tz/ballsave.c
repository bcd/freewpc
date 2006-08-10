/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

/* TBD: when multiple leffs are supported, this function should
 * become a true leff. */
static void ballsave_monitor (void)
{
start:
#ifdef CONFIG_TIMED_GAME
	while (timed_game_timer > 5)
#else
	for (;;) /* game-specific timeout put here */
#endif
	{
		leff_on (LM_SHOOT_AGAIN);
		task_sleep (TIME_100MS);
		leff_off (LM_SHOOT_AGAIN);
		task_sleep (TIME_100MS);
	}
	lamp_leff_free (LM_SHOOT_AGAIN);
	while (timed_game_timer != 0)
	{
		task_sleep (TIME_500MS);
		if (timed_game_timer >= 5)
			goto start;
	}
	task_exit ();
}

void ballsave_enable (void)
{
	lamp_leff_allocate (LM_SHOOT_AGAIN);
	task_recreate_gid (GID_BALLSAVER, ballsave_monitor);
}

void ballsave_disable (void)
{
	task_kill_gid (GID_BALLSAVER);
	lamp_leff_free (LM_SHOOT_AGAIN);
}

bool ballsave_test_active (void)
{
#ifdef CONFIG_TIMED_GAME
	if (timed_game_timer > 5)
		return TRUE;
#endif
	return task_find_gid (GID_BALLSAVER) ? TRUE : FALSE;
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

