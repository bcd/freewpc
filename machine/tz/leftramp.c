/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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
#include <status.h>

U8 left_ramps;
extern U8 cameras_lit;
extern U8 gumball_enable_count;
extern U8 autofire_request_count;	
extern U8 mball_locks_made;
extern bool multiball_ready (void);
//extern bool autofire_busy;
extern bool chaosmb_can_divert_to_autoplunger (void);

extern void mball_left_ramp_exit (void);
extern void sssmb_left_ramp_exit (void);
extern void chaosmb_left_ramp_exit (void);

extern score_t tnf_score;

static inline void left_ramp_speech_subtask (void)
{
	switch (left_ramps)
	{
		default:
			break;
		case 3:
			sound_send (SND_MOST_UNUSUAL_CAMERA);
			break;
		case 6:
			sound_send (SND_THIS_IS_NO_ORDINARY_GUMBALL);
			break;
		case 10:
			if (can_award_extra_ball ())
				sound_send (SND_GET_THE_EXTRA_BALL);
			else
				sound_send (SND_TEN_MILLION_POINTS);
			break;
		case 20:	
			sound_send (SND_YES);
			break;
	}


}

static void left_ramp_deff_subtask (void)
{
	psprintf ("1 LEFT RAMP", "%d LEFT RAMPS", left_ramps);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);

	if (left_ramps < 3)
		sprintf ("CAMERA AT 3");
	else if (left_ramps == 3)
	{
		sprintf ("CAMERA LIT");
	}
	else if (left_ramps < 6)
		sprintf ("GUMBALL AT 6");
	else if (left_ramps == 6)
	{
		sprintf ("GUMBALL LIT");
	}
	else if (left_ramps < 10 && can_award_extra_ball ())
		sprintf ("EXTRA BALL AT 10");
	else if (left_ramps < 10 && !can_award_extra_ball ())
		sprintf ("10M AT 10");
	else if (left_ramps == 10)
	{
		if (can_award_extra_ball ())
			sprintf ("EXTRA BALL LIT");
		else	
			sprintf ("10 MILLION");
	}
	else if (left_ramps > 10)
		sprintf ("20 MILLION AT 20");
	else if (left_ramps == 20)
	{
		sprintf ("20 MILLION");
	}
	else
		sprintf ("VROOOM");
}

void left_ramp_deff (void)
{
	left_ramp_speech_subtask ();
	U8 i;
	for (i = 0; i < 20; i++)
	{
		dmd_alloc_pair_clean ();
		left_ramp_deff_subtask ();
		font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
		dmd_copy_low_to_high ();
		callset_invoke (score_overlay);
		dmd_show2 ();
		task_sleep (TIME_100MS);
	}
	deff_exit ();
}

void award_left_ramp (void)
{
	if (left_ramps == 3)
		cameras_lit++;
	else if (left_ramps == 6)
		gumball_enable_count++;
	else if (left_ramps == 10 && can_award_extra_ball ())
		light_easy_extra_ball ();
	else if (left_ramps == 10 && !can_award_extra_ball ())
		score (SC_10M);
	else if (left_ramps == 20)
	{
		score (SC_20M);
		left_ramps = 0;
	}
	else
		score (SC_250K);
}

CALLSET_ENTRY(leftramp, start_player)
{
	left_ramps = 0;
}

CALLSET_ENTRY(leftramp, start_ball)
{
	left_ramps = 0;
}
/* Check for a combo from the right inlane.  If so, divert to autoplunger for
 * Doink mode.  If Doink mode has already been collected AND the player earned 
 * more than 20M points then they gain another 10m
 */

inline static bool right_inlane_combo_check (void)
{
	if (!single_ball_play ())
		return FALSE;

	if (task_kill_gid (GID_TNF_READY))
	{
		//event_can_follow (left_ramp_exit, tnf, TIME_4S);
		timer_restart_free (GID_TNF_APPROACHING, TIME_4S);
		deff_start (DEFF_GET_READY_TO_DOINK);
		return TRUE;
	}
	else
		return FALSE;

}

static void maybe_ramp_divert (void)
{
	/* Don't divert if a ball is waiting to be fired */
	if (autofire_request_count != 0)
		return;
	if (task_find_gid (GID_AUTOFIRE_HANDLER))
		return;
	
	/* Divert to autoplunger if mball ready */
	/* Divert to autoplunger for chaosmb */
	if (multiball_ready () ||
		chaosmb_can_divert_to_autoplunger () ||
		right_inlane_combo_check ())
	{
		leff_start (LEFF_STROBE_DOWN);
		ramp_divert_to_autoplunger ();
	}
	/* Divert to plunger lane for sssmb*/
	if (sssmb_can_divert_to_plunger ())
	{
		/* TODO Shore up logic by event_should_follow (plunger_switch); */
		ramp_divert ();
	}
}

CALLSET_ENTRY (left_ramp, lamp_update)
{
	if (timer_find_gid (GID_TNF_READY) || task_find_gid (GID_SDSS_READY))
		lamp_tristate_flash (LM_BONUS_X);
	else
		lamp_tristate_off (LM_BONUS_X);
}

CALLSET_ENTRY (left_ramp, sw_left_ramp_enter)
{
	score (SC_1K);
	device_switch_can_follow (left_ramp_enter, left_ramp_exit, TIME_2S);
}

CALLSET_ENTRY (left_ramp, sw_left_ramp_exit)
{
	device_switch_can_follow (left_ramp_exit, inlane2, TIME_1S);
	/* Tell the other bits of code that a left ramp has been completed */
	maybe_ramp_divert ();
	sssmb_left_ramp_exit ();
	mball_left_ramp_exit ();
	chaosmb_left_ramp_exit ();
	callset_invoke (left_ramp_exit);
	
	/* Add two ramps if hit from the right inlane */
	if (task_find_gid (GID_LEFT_RAMP))
		bounded_increment (left_ramps, 250);
	bounded_increment (left_ramps, 250);
	deff_start (DEFF_LEFT_RAMP);
	leff_start (LEFF_LEFT_RAMP);
	award_left_ramp ();
}

CALLSET_ENTRY (left_ramp, status_report)
{
	status_page_init ();
	left_ramp_deff_subtask ();
	status_page_complete ();
}
