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
extern void award_unlit_shot (U8 unlit_called_from);
extern void mball_start_3_ball (void);
extern bool door_awarded_from_slot;
typedef enum {
	CAMERA_AWARD_LIGHT_LOCK=0,
	CAMERA_AWARD_DOOR_PANEL,
	CAMERA_AWARD_10_MILLION,
	CAMERA_AWARD_QUICK_MB,
	CAMERA_AWARD_20_MILLION,
	MAX_CAMERA_AWARDS,
} camera_award_t;

__local__ U8 cameras_lit;

__local__ camera_award_t camera_award_count;
/* Needed to store award for deff */
camera_award_t camera_award_count_stored;

void camera_award_deff (void)
{
	U16 fno;
	for (fno = IMG_CAMERA_START; fno <= IMG_CAMERA_END; fno += 2)
	{
		/* Play the sound effect here
		 * so it's in sync */
		if (fno == IMG_CAMERA_START)
			sound_send (SND_CAMERA_PICTURE_EJECT_1);
		if (fno == IMG_CAMERA_START + 6)
			sound_send (SND_CAMERA_PICTURE_EJECT_2);
		dmd_alloc_pair_clean ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	task_sleep_sec (1);	
	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	sprintf ("CAMERA AWARD %d", camera_award_count_stored);
	font_render_string_center (&font_mono5, 64, 6, sprintf_buffer);
	switch (camera_award_count_stored)
	{
		case CAMERA_AWARD_LIGHT_LOCK:
			sprintf ("LIGHT LOCK");
			break;
		case CAMERA_AWARD_DOOR_PANEL:
			sprintf ("SPOT DOOR PANEL");
			break;
		case CAMERA_AWARD_10_MILLION:
			sprintf ("10 MILLION");
			break;
		case CAMERA_AWARD_QUICK_MB:
			sprintf ("QUICK MULTIBALL");
			break;
		case CAMERA_AWARD_20_MILLION:
			sprintf ("20 MILLION");
			break;
		default:
			break;
	}
	font_render_string_center (&font_fixed6, 64, 23, sprintf_buffer);
	dmd_sched_transition (&trans_scroll_down_fast);
	dmd_show_low ();
	sound_send (SND_GUMBALL_LOADED);
	task_sleep_sec (2);
	deff_exit ();
}


static void do_camera_award (void)
{
	kickout_lock (KLOCK_DEFF);
	deff_start (DEFF_CAMERA_AWARD);
	camera_award_count_stored = camera_award_count;
	switch (camera_award_count)
	{
		case CAMERA_AWARD_LIGHT_LOCK:
			/* Light Lock */
			mball_light_lock ();
			break;
		case CAMERA_AWARD_DOOR_PANEL:
			/* Spot Door Panel */
			door_awarded_from_slot = FALSE;
			callset_invoke (award_door_panel);
			break;
		case CAMERA_AWARD_10_MILLION:
			/* 10 Million */
			score (SC_10M);	
			break;
		case CAMERA_AWARD_QUICK_MB:
			/* Quick Multiball */
			callset_invoke (mball_start);
			break;
		case CAMERA_AWARD_20_MILLION:
			score (SC_20M);
			break;
		default:
			break;
	}
	if (cameras_lit > 0)
		cameras_lit--;
	camera_award_count++;
	if (camera_award_count >= MAX_CAMERA_AWARDS)
		camera_award_count = 1;
	task_exit ();
}

static bool can_award_camera (void)
{
	if (cameras_lit > 0 && !multi_ball_play ())
		return TRUE;
	else
		return FALSE;
}
/* TODO Fix bug when gumball exit happens */
CALLSET_ENTRY (camera, sw_camera)
{
	device_switch_can_follow (camera, slot, TIME_6S);
	if (event_did_follow (mpf_top, camera))
	{
		callset_invoke (mpf_collected);
	}
	else if (event_did_follow (gumball_exit, camera))
	{
	}
	else if (event_did_follow (dead_end, camera))
	{
	}
	else if (can_award_camera ())
	{
		do_camera_award ();
		score (SC_500K);
		//bounded_decrement (cameras_lit, 0);
		sound_send (SND_CAMERA_AWARD_SHOWN);
	}
	else
	{
		award_unlit_shot (SW_CAMERA);	
		score (SC_250K);
		sound_send (SND_JET_BUMPER_ADDED);
	}
}



CALLSET_ENTRY (camera, lamp_update)
{
	if (can_award_camera ())
		lamp_tristate_flash (LM_CAMERA);
	else
		lamp_tristate_off (LM_CAMERA);
}

CALLSET_ENTRY (camera, start_player)
{
	cameras_lit = 1;
	camera_award_count = 0;
}


CALLSET_ENTRY (camera, door_start_camera)
{
	cameras_lit++;
}
