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

__local__ U8 mball_locks_lit;
__local__ U8 mball_locks_made;
__local__ U8 mballs_played;


void mball_lock_lamp_update (void)
{
	if (mball_locks_lit)
		lamp_tristate_flash (LM_LOCK_ARROW);
}

void mball_light_lock (void)
{
	mball_locks_lit++;
	lamp_tristate_flash (LM_LOCK_ARROW);
	sound_send (SND_GUMBALL_COMBO);
}


void mball_check_light_lock (void)
{
	if (lamp_test (LM_GUM) && lamp_test (LM_BALL))
	{
		mball_light_lock ();
		timed_game_extend (15);
	}
}


CALLSET_ENTRY (mball, sw_left_ramp_exit)
{
	if (!lamp_test (LM_GUM))
	{
		lamp_on (LM_GUM);
		mball_check_light_lock ();
	}
}

CALLSET_ENTRY (mball, right_ramp)
{
	if (!lamp_test (LM_BALL))
	{
		lamp_on (LM_BALL);
		mball_check_light_lock ();
	}
}


CALLSET_ENTRY (mball, piano)
{
	if (lamp_flash_test (LM_PIANO_JACKPOT))
	{
		music_change (MUS_MULTIBALL);
		lamp_tristate_off (LM_PIANO_JACKPOT);
	}
}


CALLSET_ENTRY (mball, lock)
{
	if (mball_locks_lit > 0)
	{
		mball_locks_lit--;
		device_lock_ball (device_entry (DEVNO_LOCK));
		sound_send (SND_FAST_LOCK_STARTED);
		if (mball_locks_lit == 0)
		{
			lamp_off (LM_GUM);
			lamp_off (LM_BALL);
			lamp_tristate_off (LM_LOCK_ARROW);
		}
		mball_locks_made++;
		lamp_on (LM_LOCK1);
		if (mball_locks_made == 2)
			lamp_on (LM_LOCK2);
		lamp_tristate_flash (LM_PIANO_JACKPOT);
	}
}

CALLSET_ENTRY (mball, start_player)
{
	lamp_off (LM_GUM);
	lamp_off (LM_BALL);
	lamp_off (LM_LOCK1);
	lamp_off (LM_LOCK2);
	lamp_tristate_off (LM_LOCK_ARROW);
	lamp_tristate_off (LM_PIANO_JACKPOT);
	mball_locks_lit = 0;
	mball_locks_made = 0;
	mballs_played = 0;
}

