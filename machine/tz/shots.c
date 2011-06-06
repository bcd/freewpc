/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (shots, __machine2__) */
#include <freewpc.h>

/* Module to trigger all the different 'shots' */

/* During Multiball, we don't really want to kill the timer as another ball may
 * have triggered it.  During single ball play, it's better to kill the timer so
 * another sudden switch closure (slot -> Flipper -> Slot) doesn't get ignored
 */
bool task_find_or_kill_gid (free_timer_id_t gid)
{
	if (single_ball_play ())
		return task_kill_gid (gid);
	else
		return task_find_gid (gid) ? TRUE : FALSE;
}

/* 
 * Miniplayfield
 */

/* Stop the score countdown if switch is tripped,
 * it can take a while before mpf_collected gets 
 * triggered.
 * Note: The ball may not always trip this switch on mpf exit 
 */
CALLSET_ENTRY (shots, sw_gumball_exit)
{
	task_kill_gid (GID_MPF_COUNTDOWN_SCORE_TASK);
	/* Gumball code goes here */
}


CALLSET_ENTRY (shots, sw_mpf_exit)
{
	if (in_live_game)
		callset_invoke (mpf_exited);
}

CALLSET_ENTRY (shots, sw_right_ramp)
{
	/* If the mini-playfield is enabled, we hold the ramp divertor open until
	 * the ball has had a chance to get into the powerfield.
	 * The timer is to let the mpf_enter switch know that a ball came into
	 * the mpf area legitimately, ie not a powerball, in game etc
	 */
	if (mpf_ready_p ())
		free_timer_restart (TIM_R_RAMP_TO_MPF, TIME_4S);
}

CALLSET_ENTRY (shots, sw_mpf_enter)
{
	if (!in_live_game)
		return;
	if (free_timer_test (TIM_R_RAMP_TO_MPF))
		callset_invoke (mpf_entered);
	else if (multi_ball_play ())
		callset_invoke (mpf_unexpected_ball);
}

/* Closing this switch does not imply that the
 * mpf award was collected, as it's possible for 
 * the ball to roll back down again but we can 
 * tell camera.c to expect a ball coming from 
 * the mpf.
 * */

CALLSET_ENTRY (shots, sw_mpf_top)
{
	/* This tells camera.c to be expecting a ball */
	free_timer_restart (TIM_MPF_TOP_TO_CAMERA, TIME_4S);
	if (in_live_game)
	{
		/* This triggers the leff and points */
		callset_invoke (mpf_top_triggered);
	}
}

/* 
 * Deadend, Camera, Switch, Piano
 * Gumball can stay where it is for now
 */

/* Spawned as a seperate task to avoid the deadly overflow */
static void mpf_collected_task (void)
{
	callset_invoke (mpf_collected);
	task_exit ();
}


CALLSET_ENTRY (shots, sw_camera)
{
	timer_restart_free (GID_CAMERA_TO_SLOT, TIME_4S);
	/* Start a timer for the eddy sensor between the camera and slot */
	timer_restart_free (GID_CAMERA_SLOT_PROX_DETECT, TIME_4S);
	if (!in_live_game)
		return;
	if (free_timer_test (TIM_MPF_TOP_TO_CAMERA))
	{
		callset_invoke (mpf_collected);
		//task_create_anon (mpf_collected_task);
	}
	else if (task_find_gid (GID_GUMBALL_TO_SLOT)
		|| task_find_gid (GID_DEADEND_TO_SLOT))
	{
		/* Do nothing */
	}
	else
		callset_invoke (camera_shot);
}

CALLSET_ENTRY (shots, sw_piano)
{
	timer_restart_free (GID_PIANO_TO_SLOT, TIME_4S);
	/* Start a timer for the eddy sensor between the piano and slot */
	timer_restart_free (GID_PIANO_SLOT_PROX_DETECT, TIME_4S);
	if (!in_live_game)
		return;
	else if (can_award_door_panel () && flag_test (FLAG_PIANO_DOOR_LIT))
	{
		flag_off (FLAG_PIANO_DOOR_LIT);
		flag_on (FLAG_SLOT_DOOR_LIT);
		callset_invoke (select_mode);
		callset_invoke (award_door_panel);
	}
	else 
	{
		score (SC_5130);
		if (check_relight_slot_or_piano () && !flag_test (FLAG_PIANO_DOOR_LIT))
			flag_on (FLAG_PIANO_DOOR_LIT);
		award_unlit_shot (SW_PIANO);
		callset_invoke (oddchange_collected);
	}
}
