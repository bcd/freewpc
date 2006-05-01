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


void sw_righthole_handler (void)
{
}


DECLARE_SWITCH_DRIVER (sw_righthole)
{
	.fn = sw_righthole_handler,
	.devno = SW_DEVICE_DECL(1),
};

void righthole_kick_sound (void)
{
	task_exit ();
}


void righthole_enter (device_t *dev)
{
	mark_ball_in_play ();
	score_add_current_const (SCORE_2500);
}


void righthole_kick_attempt (device_t *dev)
{
	db_puts ("Sending righthole kick sound\n");
	if (in_game && !in_tilt)
	{
	}
}


device_ops_t righthole_ops = {
	.enter = righthole_enter,
	.kick_attempt = righthole_kick_attempt,
};

device_properties_t righthole_props = {
	.ops = &righthole_ops,
	.name = "RIGHT POPPER",
	.sol = SOL_RIGHT_POPPER,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_RIGHT_POPPER },
};


CALLSET_ENTRY (righthole, init)
{
	device_register (1, &righthole_props);
}

