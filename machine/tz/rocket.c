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


void rocket_enter (device_t *dev)
{
	score (SC_10K);
}

__taskentry__ void rocket_kick_sound (void)
{
	sound_send (SND_ROCKET_KICK_DONE);
	flasher_pulse (FLASH_UR_FLIPPER);
	task_exit ();
}

void rocket_kick_attempt (device_t *dev)
{
	event_should_follow (rocket, hitchhiker, TIME_2S);
	if (in_live_game)
	{
		db_puts ("Sending rocket kick sound\n");
		leff_start (LEFF_NO_GI);
		sound_send (SND_ROCKET_KICK_REVVING);
		task_sleep (TIME_100MS * 8);
		task_create_gid (0, rocket_kick_sound);
	}
}



device_ops_t rocket_ops = {
	.enter = rocket_enter,
	.kick_attempt = rocket_kick_attempt,
};

device_properties_t rocket_props = {
	.ops = &rocket_ops,
	.name = "ROCKET KICKER",
	.sol = SOL_ROCKET_KICKER,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_ROCKET_KICKER },
};


CALLSET_ENTRY (rocket, init)
{
	device_register (DEVNO_ROCKET, &rocket_props);
}

