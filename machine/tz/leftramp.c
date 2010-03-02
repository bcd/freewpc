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
#include <eb.h>

__local__ U8 left_ramps;
extern U8 cameras_lit;
extern U8 gumball_enable_count;
extern U8 autofire_request_count;	
extern U8 mball_locks_made;
//extern U8 sssmb_ramps_to_divert;

//extern U8 chaosmb_level;
//extern U8 chaosmb_hits_to_relight;
extern bool multiball_ready (void);
extern bool autofire_busy;
//extern bool sssmb_ball_in_plunger;
extern bool chaosmb_can_divert_to_autoplunger (void);

//TODO Get rid of this
extern void mball_left_ramp_exit (void);
extern void sssmb_left_ramp_exit (void);
extern void chaosmb_left_ramp_exit (void);

void left_ramp_deff (void)
{
	dmd_alloc_low_clean ();
	psprintf ("1 LEFT RAMP", "%d LEFT RAMPS", left_ramps);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);

	/*if (left_ramps < 3 && config_timed_game)
		sprintf ("EXTRA TIME AT 3");
	else if (left_ramps == 3 && config_timed_game)
		sprintf ("15 SECS. ADDED");
	else if (left_ramps < 3 && !config_timed_game)
		sprintf ("MYSTERY AT 3");
	else if (left_ramps == 3 && !config_timed_game)
		sprintf ("MYSTERY IS LIT");
	else if (left_ramps < 6)
		sprintf ("EXTRA BALL AT 6");
	else if (left_ramps == 6)
	{
		sprintf ("EXTRA BALL");
		light_easy_extra_ball ();
	}*/
	
	if (left_ramps < 3)
		sprintf ("CAMERA AT 3");
	else if (left_ramps == 3)
	{
		sprintf ("CAMERA LIT");
		cameras_lit++;
	}
	else if (left_ramps < 6)
		sprintf ("GUMBALL AT 6");
	else if (left_ramps == 6)
	{
		sprintf ("GUMBALL LIT");
		gumball_enable_count++;
	}
	else if (left_ramps < 10)
		sprintf ("EXTRA BALL AT 10");
	else if (left_ramps == 10)
	{
		sprintf ("EXTRA BALL LIT");
		sound_send (SND_GET_THE_EXTRA_BALL);
		light_easy_extra_ball ();
	}
	else if (left_ramps >= 10)
		sprintf ("20 MILLION AT 20");
	else if (left_ramps == 20)
	{
		sprintf ("20 MILLION");
		score (SC_20M);
		left_ramps = 0;
	}
	else
		sprintf ("");
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);

	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY(leftramp, start_player)
{
	left_ramps = 0;
}


CALLSET_ENTRY(leftramp, start_ball)
{
	left_ramps = 0;
}


static void maybe_ramp_divert (void)
{
	/* Don't divert if a ball is waiting to be fired */
	if (autofire_request_count != 0)
	{	sound_send (SND_TWO);
		return;
	}
	if (autofire_busy)
	{
		sound_send (SND_ONE);
		return;
	}
	
	/* Divert to autoplunger if mball ready */
	if (multiball_ready ())
		ramp_divert_to_autoplunger ();
	/* Divert to autoplunger for chaosmb */
	if (chaosmb_can_divert_to_autoplunger ())	
		ramp_divert_to_autoplunger ();
	/* Divert to plunger lane for sssmb*/
	if (sssmb_can_divert_to_plunger ())
	{
		/* TODO Shore up logic by event_should_follow (plunger_switch); */
		//sssmb_ball_in_plunger = TRUE;
		ramp_divert ();
	}
}

CALLSET_ENTRY (left_ramp, lamp_update)
{
	if (timer_find_gid (GID_LEFT_RAMP))
		lamp_tristate_flash (LM_BONUS_X);
	else
		lamp_tristate_off (LM_BONUS_X);
}

CALLSET_ENTRY (left_ramp, sw_left_ramp_enter)
{
	score (SC_1K);
}

CALLSET_ENTRY (left_ramp, sw_left_ramp_exit)
{
	/* Tell the other bits of code that a left ramp has been completed */
	maybe_ramp_divert ();
	sssmb_left_ramp_exit ();
	mball_left_ramp_exit ();
	chaosmb_left_ramp_exit ();
	
	/* Add two ramps if hit from the right inlane */
	if (task_kill_gid (GID_LEFT_RAMP))
		bounded_increment (left_ramps, 250);
	bounded_increment (left_ramps, 250);
	deff_start (DEFF_LEFT_RAMP);
	leff_start (LEFF_LEFT_RAMP);
	score (SC_250K);
}

