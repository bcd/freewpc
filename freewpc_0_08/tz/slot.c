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


DECLARE_SWITCH_DRIVER (sw_slot)
{
	.devno = SW_DEVICE_DECL(DEVNO_SLOT),
};


CALLSET_ENTRY (slot, update_lamps)
{
	if (flag_test (FLAG_DOOR_AWARD_LIT))
	{
		extern void door_award_rotate (void);
		lamp_tristate_on (LM_SLOT_MACHINE);
		lamp_tristate_off (LM_LEFT_INLANE2);
		task_recreate_gid (GID_DOOR_AWARD_ROTATE, door_award_rotate);
	}
	else
	{
		lamp_tristate_off (LM_SLOT_MACHINE);
		lamp_tristate_on (LM_LEFT_INLANE2);
		task_kill_gid (GID_DOOR_AWARD_ROTATE);
	}
}

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
	score_add_current_const (SCORE_1K);

	if (task_kill_gid (GID_SLOT_DISABLED_BY_PIANO))
	{
		/* piano was recently hit, so ignore slot */
	}
	else if (task_kill_gid (GID_SLOT_DISABLED_BY_CAMERA))
	{
		/* camera was recently hit, so ignore slot */
	}
	else if (task_kill_gid (GID_SLOT_DISABLED_BY_SKILL_SWITCH))
	{
		/* skill switch was recently hit, so ignore slot */
	}
	else
	{
		score_add_current_const (SCORE_5K);

		if (flag_test (FLAG_DOOR_AWARD_LIT))
		{
			door_award_flashing ();
			flag_off (FLAG_DOOR_AWARD_LIT);
			slot_update_lamps ();
		}
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


CALLSET_ENTRY(slot, start_ball)
{
	slot_update_lamps ();
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

