/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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


void mpf_rtt (void) /* TODO : move to system page */
{
	if (mpf_active && in_live_game)
	{
		rt_solenoid_update (&rtsol_mpf_left,
			SOL_MPF_LEFT_MAGNET, RTSOL_ACTIVE_HIGH,
			SW_L_L_FLIPPER_BUTTON, RTSW_ACTIVE_HIGH,
			8, 8);
	
		rt_solenoid_update (&rtsol_mpf_right,
			SOL_MPF_RIGHT_MAGNET, RTSOL_ACTIVE_HIGH,
			SW_L_R_FLIPPER_BUTTON, RTSW_ACTIVE_HIGH,
			8, 8);
	}
}


void mpf_active_deff (void)
{
	while (mpf_active)
	{
	}
}


void mpf_battle_lamp_update (void)
{
	if (mpf_enable_count > 0)
		lamp_tristate_on (LM_RAMP_BATTLE);
	else
		lamp_tristate_off (LM_RAMP_BATTLE);
}

void mpf_enable (void)
{
	mpf_enable_count++;
	mpf_battle_lamp_update ();
}


void mpf_activate (void)
{
	if (mpf_enable_count > 0)
	{
		mpf_enable_count--;
		mpf_active = 1;
		mpf_ball_count++;
	}
}


void mpf_deactivate (void)
{
	mpf_active = 0;
}


void mpf_battle_running (void)
{
	task_sleep_sec (5);
	task_exit ();
}

void mpf_start (void)
{
	task_create_gid1 (GID_BATTLE_RUNNING, mpf_battle_running);
}


void mpf_stop (void)
{
	task_kill_gid (GID_BATTLE_RUNNING);
}


CALLSET_ENTRY (mpf, door_start_battle_power)
{
	mpf_enable ();
}


CALLSET_ENTRY (mpf, sw_camera)
{
	if (event_did_follow (mpf_top, camera))
	{
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

		mpf_ball_count++;
		callset_invoke (powerfield_enter);
	}
}


CALLSET_ENTRY (mpf, sw_mpf_exit)
{
	mpf_ball_count--;
	callset_invoke (powerfield_exit);

	if (mpf_ball_count == 0)
	{
		callset_invoke (powerfield_end);
		mpf_stop ();
	}
}


CALLSET_ENTRY (mpf, sw_mpf_left)
{
}


CALLSET_ENTRY (mpf, sw_mpf_right)
{
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

