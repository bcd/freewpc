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

DECLARE_SWITCH_DRIVER (sw_slot)
{
	.devno = SW_DEVICE_DECL(DEVNO_SLOT),
};


void slot_kick_sound (void)
{
	sound_send (SND_SLOT_KICKOUT_2);
	task_exit ();
}


void slot_enter (device_t *dev)
{
	extern void door_award_flashing (void);

	task_kill_gid (GID_SKILL_SWITCH_TRIGGER);
	mark_ball_in_play ();
	score (SC_1K);

	if (switch_did_follow (slot_proximity, slot))
	{
	}

	if (switch_did_follow (dead_end, slot))
	{
	}
	else if (switch_did_follow (piano, slot))
	{
		/* piano was recently hit, so ignore slot */
	}
	else if (switch_did_follow (camera, slot))
	{
		/* camera was recently hit, so ignore slot */
	}
	else if (switch_did_follow (any_skill_switch, slot))
	{
		/* skill switch was recently hit, so ignore slot */
	}
	else
	{
		if (lamp_test (LM_PANEL_SUPER_SLOT))
		{
			sound_send (SND_KACHING);
			score (SC_250K);
			task_sleep_sec (1);
		}
		else
			score (SC_50K);

		callset_invoke (slot_machine);
	}
}


void slot_kick_attempt (device_t *dev)
{
	db_puts ("Sending slot kick sound\n");
	if (in_game && !in_tilt)
	{
		sound_send (SND_SLOT_KICKOUT_1);
		leff_start (LEFF_SLOT_KICKOUT);
		task_sleep (TIME_100MS * 5);
		task_create_gid (0, slot_kick_sound);
	}
}


device_ops_t slot_ops = {
	.enter = slot_enter,
	.kick_attempt = slot_kick_attempt,
};

device_properties_t slot_props = {
	.ops = &slot_ops,
	.name = "SLOT KICKOUT",
	.sol = SOL_SLOT,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_SLOT },
};


CALLSET_ENTRY (slot, init)
{
	device_register (DEVNO_SLOT, &slot_props);
}

