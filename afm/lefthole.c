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



DECLARE_SWITCH_DRIVER (sw_lefthole)
{
	.devno = SW_DEVICE_DECL(DEVNO_LEFTHOLE),
};

void lefthole_kick_sound (void)
{
	task_exit ();
}


void lefthole_enter (device_t *dev)
{
	mark_ball_in_play ();
	score_add_current_const (SCORE_1K);
}


void lefthole_kick_attempt (device_t *dev)
{
	db_puts ("Sending lefthole kick sound\n");
	if (in_game && !in_tilt)
	{
	}
}


device_ops_t lefthole_ops = {
	.enter = lefthole_enter,
	.kick_attempt = lefthole_kick_attempt,
};

device_properties_t lefthole_props = {
	.ops = &lefthole_ops,
	.name = "LEFT POPPER",
	.sol = SOL_LEFT_POPPER,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_LEFT_POPPER },
};


CALLSET_ENTRY (lefthole, init)
{
	device_register (DEVNO_LEFTHOLE, &lefthole_props);
}

