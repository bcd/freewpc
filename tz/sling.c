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

U8 rtsol_left_sling;
U8 rtsol_right_sling;


void slingshot_rtt (void)
{
	rt_solenoid_update (&rtsol_left_sling,
		SOL_LEFT_SLING, RTSOL_ACTIVE_HIGH,
		SW_LEFT_SLING, RTSW_ACTIVE_HIGH,
		8, 8);

	rt_solenoid_update (&rtsol_right_sling,
		SOL_RIGHT_SLING, RTSOL_ACTIVE_HIGH,
		SW_RIGHT_SLING, RTSW_ACTIVE_HIGH,
		8, 8);
}


void sw_sling_handler (void)
{
	score (SC_10);
}


DECLARE_SWITCH_DRIVER (sw_left_sling)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_SLINGSHOT,
	.fn = sw_sling_handler,
};


DECLARE_SWITCH_DRIVER (sw_right_sling)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_SLINGSHOT,
	.fn = sw_sling_handler,
};

