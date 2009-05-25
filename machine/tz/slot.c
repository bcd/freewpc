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


void slot_kick_sound (void)
{
	sound_send (SND_SLOT_KICKOUT_2);
	task_exit ();
}


CALLSET_ENTRY (slot, dev_slot_enter)
{
	extern void door_award_flashing (void);

	task_kill_gid (GID_SKILL_SWITCH_TRIGGER);
	set_valid_playfield ();
	score (SC_1K);

	if (event_did_follow (dead_end, slot))
	{
		/* dead end was recently hit, so ignore slot */
	}
	else if (event_did_follow (piano, slot))
	{
		/* piano was recently hit, so ignore slot */
	}
	else if (event_did_follow (camera, slot))
	{
		/* camera was recently hit, so ignore slot */
	}
	else if (event_did_follow (any_skill_switch, slot))
	{
		/* skill switch was recently hit, so ignore slot */
		callset_invoke (skill_missed);
		deff_stop (DEFF_SKILL_SHOT_READY);
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

		callset_invoke (shot_slot_machine);
	}
}


CALLSET_ENTRY (slot, dev_slot_kick_attempt)
{
	if (in_live_game)
	{
		sound_send (SND_SLOT_KICKOUT_1);
		leff_start (LEFF_SLOT_KICKOUT);
		task_sleep (TIME_100MS * 5);
		task_create_gid (0, slot_kick_sound);
	}
}

