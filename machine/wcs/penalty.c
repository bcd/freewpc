/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

/* Penalties for ball drain during a timed game.
 * The first two drains are free.
 * The third drain incurs a 10 second penalty.
 * The fourth drain incurs a 15 second penalty.
 * Every subsequent drain incurs a 20 second penalty.
 */

__local__ U8 single_ball_drains;

void drain_penalty_warning_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void drain_penalty_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

CALLSET_ENTRY (penalty, timed_drain_penalty)
{
	if (single_ball_drains >= 3)
	{
		/* deff_start (DEFF_DRAIN_PENALTY); */
	}
	else
	{
		single_ball_drains++;
		/* deff_start (DEFF_DRAIN_PENALTY_WARNING); */
	}
}

CALLSET_ENTRY (penalty, start_player)
{
	single_ball_drains = 0;
}
