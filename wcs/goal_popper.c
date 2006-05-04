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
#include <mach/switch.h>
#include <mach/coil.h>



DECLARE_SWITCH_DRIVER (sw_goal_popper)
{
	.devno = SW_DEVICE_DECL(DEVNO_GOAL_POPPER),
};

void goal_popper_kick_sound (void)
{
	task_exit ();
}


void goal_popper_enter (device_t *dev)
{
	mark_ball_in_play ();
	score_add_current_const (SCORE_1K);
}


void goal_popper_kick_attempt (device_t *dev)
{
	db_puts ("Sending goal_popper kick sound\n");
	if (in_game && !in_tilt)
	{
	}
}


device_ops_t goal_popper_ops = {
	.enter = goal_popper_enter,
	.kick_attempt = goal_popper_kick_attempt,
};

device_properties_t goal_popper_props = {
	.ops = &goal_popper_ops,
	.name = "GOAL POPPER",
	.sol = SOL_GOAL_POPPER,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_GOAL_POPPER },
};


CALLSET_ENTRY (goal_popper, init)
{
	device_register (DEVNO_GOAL_POPPER, &goal_popper_props);
}

