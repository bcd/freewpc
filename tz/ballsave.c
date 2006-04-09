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
	int i;
	lamp_leff_allocate (LM_SHOOT_AGAIN);
	for (i=0; i < 40; i++)
	{
		leff_on (LM_SHOOT_AGAIN);
		task_sleep (TIME_100MS);
		leff_off (LM_SHOOT_AGAIN);
		task_sleep (TIME_100MS);
	}
	lamp_leff_free (LM_SHOOT_AGAIN);
	task_sleep_sec (2);
	task_exit ();
}

void ballsave_enable (void)
{
	task_recreate_gid (GID_BALLSAVER, ballsave_monitor);
}

void ballsave_disable (void)
{
	task_kill_gid (GID_BALLSAVER);
}

bool ballsave_test_active (void)
{
	return task_find_gid (GID_BALLSAVER) ? TRUE : FALSE;
}


void ballsave_launch (void)
{
	extern void autofire_add_ball (void);

	autofire_add_ball ();
	deff_start (DEFF_BALL_SAVE);
	ballsave_disable ();
}


