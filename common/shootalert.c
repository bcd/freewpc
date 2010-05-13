/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Warn the player if a ball is left on to the shooter too long.
 */

#include <freewpc.h>

/**
 * Monitor if a ball is stuck in the plunger lane at ball
 * start, and periodically remind the player to plunge it...
 * This task begins at ball start, and is cancelled on
 * valid playfield.
 */
void shoot_alert_task (void)
{
#ifdef MACHINE_SHOOTER_SWITCH
	task_sleep_sec (15);
	for (;;)
	{
		if (switch_poll (MACHINE_SHOOTER_SWITCH))
		{
			deff_start (DEFF_PLUNGE_BALL);
			task_sleep_sec (20);
		}
		else
			task_sleep_sec (5);
	}
#endif /* MACHINE_SHOOTER_SWITCH */
	task_exit ();
}

CALLSET_ENTRY (shootalert, start_ball)
{
	task_create_gid1 (GID_SHOOT_ALERT, shoot_alert_task);
}


CALLSET_ENTRY (shootalert, valid_playfield)
{
	task_kill_gid (GID_SHOOT_ALERT);
	deff_stop (DEFF_PLUNGE_BALL);
}

