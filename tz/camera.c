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


__local__ U8 camera_award_count;


void camera_award_deff (void)
{
	kickout_lock (KLOCK_DEFF);
	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	sprintf ("CAMERA AWARD %d", camera_award_count);
	font_render_string_center (&font_mono5, 64, 6, sprintf_buffer);
	switch (camera_award_count)
	{
		case 0:
			sprintf ("LIGHT LOCK");
			break;
		case 1:
			sprintf ("SPOT DOOR PANEL");
			break;
		case 2:
			sprintf ("20 SECONDS");
			break;
		case 3:
			sprintf ("QUICK MULTIBALL");
			break;
		case 4:
			sprintf ("250,000");
			break;
	}
	font_render_string_center (&font_fixed6, 64, 23, sprintf_buffer);
	dmd_sched_transition (&trans_scroll_up_avg);
	dmd_show_low ();
	sound_send (SND_GUMBALL_LOADED);
	task_sleep_sec (3);
	kickout_unlock (KLOCK_DEFF);
	deff_exit ();
}


void do_camera_award (void)
{
	switch (camera_award_count)
	{
		case 0:
			/* Light Lock */
			break;
		case 1:
			/* Spot Door Panel */
			break;
		case 2:
			/* Extra Time: 20 seconds */
			break;
		case 3:
			/* Quick Multiball */
			break;
		case 4:
			/* Big Points: 250K */
			break;
	}
	camera_award_count++;
	if (camera_award_count > 4)
		camera_award_count = 0;
}


void sw_camera_handler (void)
{
	if (event_did_follow (gumball_exit, camera))
	{
	}
	else if (event_did_follow (dead_end, camera))
	{
	}
	else
	{
		if (lamp_test (LM_PANEL_CAMERA))
		{
			sound_send (SND_CAMERA_PICTURE_EJECT_1);
			do_camera_award ();
			score (SC_100K);
			task_sleep (TIME_100MS);
			sound_send (SND_CAMERA_PICTURE_EJECT_2);
		}
		else
			score (SC_50K);
		sound_send (SND_JET_BUMPER_ADDED);
	}
	switch_can_follow (camera, slot, TIME_4S);
}


CALLSET_ENTRY (camera, start_player)
{
	lamp_tristate_off (LM_CAMERA);
	camera_award_count = 0;
}


CALLSET_ENTRY (camera, door_panel_awarded)
{
	if (lamp_test (LM_PANEL_CAMERA))
		lamp_tristate_flash (LM_CAMERA);
}


DECLARE_SWITCH_DRIVER (sw_camera)
{
	.fn = sw_camera_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

