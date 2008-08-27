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
#include <rtsol.h>



static void ramp_divert_task (void)
{
	U8 n;

	sol_start (SOL_RAMP_DIVERTOR, SOL_DUTY_50, TIME_1S);
	task_sleep (TIME_500MS);

	for (n = 0; n < 5; n++)
	{
		sol_start (SOL_RAMP_DIVERTOR, SOL_DUTY_12, TIME_1S);
		task_sleep (TIME_500MS);
	}

	sol_stop (SOL_RAMP_DIVERTOR);
	task_exit ();
}


void ramp_divert (void)
{
	task_recreate_gid_while (GID_RAMP_DIVERTING, ramp_divert_task, TASK_DURATION_INF);
	task_yield ();
}


void ramp_divert_to_autoplunger (void)
{
	ramp_divert ();
	autofire_catch ();
}


