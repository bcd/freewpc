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

__local__ U8 lock_shots_made;
__local__ U8 multiballs_played;
__local__ U8 balls_locked;
__local__ U8 locks_lit;
__local__ U8 shots_for_lock;

static void update_lock_lamp (void)
{
	if (locks_lit > 0)
	{
		lamp_tristate_flash (LM_LOCK_ARROW);
	}
	else if (balls_locked > 0)
	{
		lamp_tristate_on (LM_LOCK_ARROW);
	}
	else
	{
		lamp_tristate_off (LM_LOCK_ARROW);
	}
}


CALLSET_ENTRY(lock, start_game)
{
}


CALLSET_ENTRY(lock, start_player)
{
	lock_shots_made = 0;
	multiballs_played = 0;
	balls_locked = 0;
	locks_lit = 1;
	shots_for_lock = 0;
}


CALLSET_ENTRY(lock, start_ball)
{
	update_lock_lamp ();
}


DECLARE_SWITCH_DRIVER (sw_lock)
{
	.devno = SW_DEVICE_DECL(DEVNO_LOCK),
};

		
void lock_enter (device_t *dev)
{
	score_add_current_const (SCORE_75K);

	if (locks_lit != 0)
	{
		balls_locked++;
		locks_lit--;
		sound_send (SND_FAST_LOCK_STARTED);
	}
	else
	{
		lock_shots_made++;
		if (lock_shots_made == shots_for_lock)
		{
			lock_shots_made = 0;
			locks_lit++;
			shots_for_lock++;
			sound_send (SND_SUPER_ROBOT_2);
		}
		else
		{
			sound_send (SND_ROBOT_AWARD);
		}
	}
	update_lock_lamp ();
	task_sleep_sec (1);
}


void lock_kick_attempt (device_t *dev)
{
	sound_send (SND_LOCK_KICKOUT);
	timer_restart_free (GID_LOOP_DISABLED_BY_LOCK_EXIT, TIME_3S);
}


device_ops_t lock_ops = {
	.enter = lock_enter,
	.kick_attempt = lock_kick_attempt,
};

device_properties_t lock_props = {
	.ops = &lock_ops,
	.name = "LOCK",
	.sol = SOL_LOCK_RELEASE,
	.sw_count = 3,
	.init_max_count = 0,
	.sw = { SW_LOCK_UPPER, SW_LOCK_CENTER, SW_LOCK_LOWER },
};


CALLSET_ENTRY (lock, init)
{
	device_register (DEVNO_LOCK, &lock_props);
}

