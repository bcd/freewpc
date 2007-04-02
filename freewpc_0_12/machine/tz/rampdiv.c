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
#include <rtsol.h>


U8 ramp_divertor_state;

static void ramp_divert_task (void)
{
	task_set_flags (TASK_PROTECTED);
	sol_modify (SOL_RAMP_DIVERTOR, SOL_DUTY_50_50);
	task_sleep (TIME_100MS);
	sol_modify (SOL_RAMP_DIVERTOR, SOL_DUTY_12_88);
	task_sleep_sec (3);
	sol_off (SOL_RAMP_DIVERTOR);
	task_exit ();
}

void ramp_divert (void)
{
	task_recreate_gid (GID_RAMP_DIVERTING, ramp_divert_task);
	task_yield ();
}

void ramp_divert_to_autoplunger (void)
{
	ramp_divert ();
	autofire_catch ();
}


