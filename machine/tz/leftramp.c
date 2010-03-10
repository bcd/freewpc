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
__local__ U8 left_ramp_level;
__local__ bool left_ramp_extra_ball_awarded;
U8 left_ramp_level_stored;

extern U8 cameras_lit;
extern U8 gumball_enable_count;
extern U8 autofire_request_count;	
extern U8 mball_locks_made;
extern bool multiball_ready (void);
extern bool autofire_busy;
extern bool chaosmb_can_divert_to_autoplunger (void);

//TODO Get rid of this
extern void mball_left_ramp_exit (void);
extern void sssmb_left_ramp_exit (void);
extern void chaosmb_left_ramp_exit (void);

/* Struct containing hint, ramps needed and award text */
struct {
	const char *left_ramp_award_hint;
	U8 left_ramps_for_award;
	const char *left_ramp_award_text;
} left_ramp_awards[]= {
	{ "CAMERA AT 3", 3, "CAMERA LIT" },
	{ "GUMBALL AT 6", 6, "GUMBALL LIT" },
	{ "EXTRA BALL AT 10", 10, "EXTRA BALL LIT" },
	{ "20 MILLION AT 20", 20, "20 MILLION" },
};
#define MAX_LEFT_RAMP_AWARDS 3

void left_ramp_increment (void)
{
	bounded_increment (left_ramps, 250);
	/* If the extra ball has been awarded already, jump a level */
	if (left_ramps >= left_ramp_awards[left_ramp_level].left_ramps_for_award)
	{
		switch (left_ramp_level)
		{
			case 0:
				cameras_lit++;
				break;
			case 1:
				gumball_enable_count++;
				break;
			case 2:
				/* Don't allow if previously awarded */
				if (left_ramp_extra_ball_awarded == TRUE && left_ramp_level == 2)
					score (SC_10M);
				else
				{
					left_ramp_extra_ball_awarded = TRUE;
					light_easy_extra_ball ();
				}
				break;
			case 4:
				score (SC_20M);
				break;
			default:
				score (SC_1M);
				break;
		}
		/* Store level for deff */
		left_ramp_level_stored = left_ramp_level;
		left_ramp_level++;
		if (left_ramp_level > MAX_LEFT_RAMP_AWARDS)
		{
			left_ramp_level = 0;
			left_ramp_level_stored = left_ramp_level;
		}
	}
	else
		left_ramp_level_stored = left_ramp_level;
}

void left_ramp_deff (void)
{
	dmd_alloc_low_clean ();
	
	if (left_ramps >= left_ramp_awards[left_ramp_level_stored].left_ramps_for_award)
		sprintf ("%s", left_ramp_awards[left_ramp_level_stored].left_ramp_award_text);
	else
		sprintf ("%s", left_ramp_awards[left_ramp_level_stored].left_ramp_award_hint);
	
	/* If the extra ball has already been awarded, award 10M instead */
	if (left_ramp_extra_ball_awarded == TRUE 
		&& left_ramp_level == 2 
		&& left_ramps < left_ramp_awards[left_ramp_level_stored].left_ramps_for_award)
		sprintf ("10M AT 10");
	else if (left_ramp_extra_ball_awarded == TRUE 
			&& left_ramp_level == 2 
			&& left_ramps >= left_ramp_awards[left_ramp_level_stored].left_ramps_for_award)
			sprintf ("10 MILLION");

	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	psprintf ("1 LEFT RAMP", "%d LEFT RAMPS", left_ramps);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	

	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

static void maybe_ramp_divert (void)
{
	/* Don't divert if a ball is waiting to be fired */
	if (autofire_request_count != 0 || (autofire_busy))
		return;
	
	/* Divert to autoplunger if mball ready */
	if (multiball_ready ())
		ramp_divert_to_autoplunger ();
	/* Divert to autoplunger for chaosmb */
	if (chaosmb_can_divert_to_autoplunger ())	
		ramp_divert_to_autoplunger ();
	/* Divert to plunger lane for sssmb*/
	/* TODO Shore up logic by event_should_follow (plunger_switch); */
	if (sssmb_can_divert_to_plunger ())
		ramp_divert ();
}
CALLSET_ENTRY (leftramp, lamp_update)
{
	if (timer_find_gid (GID_LEFT_RAMP))
		lamp_tristate_flash (LM_BONUS_X);
	else
		lamp_tristate_off (LM_BONUS_X);
}

CALLSET_ENTRY (leftramp, sw_left_ramp_enter)
{
	score (SC_1K);
}

CALLSET_ENTRY (leftramp, sw_left_ramp_exit)
{
	/* Tell the other bits of code that a left ramp has been completed */
	maybe_ramp_divert ();
	sssmb_left_ramp_exit ();
	mball_left_ramp_exit ();
	chaosmb_left_ramp_exit ();
	/* Start another timer for the right ramp combo */
	if (task_kill_gid (GID_LEFT_RAMP))
	{
		left_ramp_increment ();
		timer_restart_free (GID_LEFT_RAMP_TO_RIGHT_RAMP, TIME_3S);	
	}
	left_ramp_increment ();
	deff_start (DEFF_LEFT_RAMP);
	leff_start (LEFF_LEFT_RAMP);
	score (SC_250K);
}

CALLSET_ENTRY(leftramp, start_player)
{
	left_ramps = 0;
	left_ramp_level = 0;
	left_ramp_extra_ball_awarded = FALSE;
}

CALLSET_ENTRY(leftramp, start_ball)
{
//	left_ramps = 0;
}
