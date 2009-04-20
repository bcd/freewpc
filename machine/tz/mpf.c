/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/** The number of balls enabled to go to the MPF */
__local__ U8 mpf_enable_count;

/** Nonzero when the MPF is active */
U8 mpf_active;

__fastram__ S8 rtsol_mpf_left;

__fastram__ S8 rtsol_mpf_right;

/** Number of balls currently on the mini-playfield */
U8 mpf_ball_count;


void mpf_active_deff (void)
{
	while (task_find_gid (GID_MPF_ACTIVE))
	{
		task_sleep (TIME_66MS);
	}
}

bool mpf_ready_p (void)
{
	return (mpf_enable_count > 0)
		&& !flag_test (FLAG_POWERBALL_IN_PLAY)
		&& !flag_test (FLAG_MULTIBALL_RUNNING)
		&& !flag_test (FLAG_QUICK_MB_RUNNING)
		&& !flag_test (FLAG_BTTZ_RUNNING);
}


CALLSET_ENTRY (mpf, lamp_update)
{
	if (mpf_ready_p ())
		lamp_tristate_on (LM_RAMP_BATTLE);
	else
		lamp_tristate_off (LM_RAMP_BATTLE);
}


void mpf_enable (void)
{
	mpf_enable_count++;
}


void mpf_active_monitor (void)
{
	task_sleep_sec (5);
	task_exit ();
}


void mpf_start (void)
{
	if (mpf_ready_p ())
	{
		mpf_enable_count--;
		mpf_ball_count++;
		if (!task_find_gid (GID_MPF_ACTIVE))
		{
			mpf_active = 1;
			task_create_gid1 (GID_MPF_ACTIVE, mpf_active_monitor);
			music_refresh ();
		}
	}
}


void mpf_stop (void)
{
	if (task_find_gid (GID_MPF_ACTIVE))
	{
		mpf_active = 0;
		task_kill_gid (GID_MPF_ACTIVE);
		sound_send (SND_POWER_HUH_3);
		music_refresh ();
	}
}


CALLSET_ENTRY (mpf, music_refresh)
{
	if (task_find_gid (GID_MPF_ACTIVE))
		music_request (MUS_POWERFIELD, PRI_GAME_MODE2);
};


CALLSET_ENTRY (mpf, door_start_battle_power)
{
	mpf_enable ();
	sound_send (SND_ARE_YOU_READY_TO_BATTLE);
}


CALLSET_ENTRY (mpf, powerfield_win)
{
	if (mpf_ball_count > 0)
	{
		mpf_ball_count--;
		if (mpf_ball_count == 0)
		{
			callset_invoke (powerfield_end);
			mpf_stop ();
		}
	}
}


CALLSET_ENTRY (mpf, sw_camera)
{
	if (event_did_follow (mpf_top, camera))
	{
		if (task_find_gid (GID_MPF_ACTIVE))
			callset_invoke (powerfield_win);
	}
}


CALLSET_ENTRY (mpf, sw_mpf_top)
{
	event_can_follow (mpf_top, camera, TIME_4S);
}


CALLSET_ENTRY (mpf, sw_mpf_enter)
{
	/* If tripped immediately after the right ramp opto, then a ball
	has truly entered the mini-playfield.  Note this may trip later
	on when a ball is already in play. */
	if (event_did_follow (right_ramp, mpf_enter))
	{
		if (mpf_ball_count == 0)
		{
			callset_invoke (powerfield_begin);
			mpf_start ();
		}

		callset_invoke (powerfield_enter);
	}
}


CALLSET_ENTRY (mpf, sw_mpf_exit)
{
	if (mpf_ball_count > 0)
	{
		mpf_ball_count--;
		callset_invoke (powerfield_exit);
	
		if (mpf_ball_count == 0)
		{
			callset_invoke (powerfield_end);
			mpf_stop ();
			sound_send (SND_HAHA_POWERFIELD_EXIT);
		}
	}
}


CALLSET_ENTRY (mpf, sw_mpf_left)
{
	if (task_find_gid (GID_MPF_ACTIVE))
		sound_send (SND_POWER_GRUNT_1);
}


CALLSET_ENTRY (mpf, sw_mpf_right)
{
	if (task_find_gid (GID_MPF_ACTIVE))
		sound_send (SND_POWER_GRUNT_2);
}


CALLSET_ENTRY (mpf, sw_right_ramp)
{
	/* If the mini-playfield is enabled, open the ramp
	divertor fully.  The ordinary catch and drop is bypassed. */
	event_should_follow (right_ramp, mpf_enter, TIME_3S);
}

CALLSET_ENTRY (mpf, start_player)
{
	mpf_enable_count = 0;
	mpf_ball_count = 0;
}

