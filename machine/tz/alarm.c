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

/* CALLSET_SECTION (alarm, __machine2__) */

/* Turns a pinball table into an Egg Timer */
#include <freewpc.h>

/* 1440 minutes in a day, park timer at 1441 to disable */
#define ALARM_DISABLED 1441

/* alarm_time is stored as minutes since midnight */
__permanent__ U16 alarm_time;

extern U8 mute_and_pause_timeout;
extern U8 hour;
extern U8 minute;

/* returns how many minutes from midnight we are */
static U16 rtc_minutes_from_midnight (void)
{
	U16 minutes;
	if (hour)
	{
		minutes = hour * 60;
		minutes += minute;
		return minutes;
	}
	else
		return minute;
}

static void render_minutes_to_alarm (void)
{
	U16 minutes_to_alarm;
	U8 hours_to_alarm;

	if (alarm_time > rtc_minutes_from_midnight ())
	{
		minutes_to_alarm = alarm_time;
		minutes_to_alarm -= rtc_minutes_from_midnight ();
	}
	else
	{
		minutes_to_alarm = rtc_minutes_from_midnight ();
		minutes_to_alarm -= alarm_time;
	}
		
	if (minutes_to_alarm < 60)
		sprintf ("ALARM IN %ld MINUTES", minutes_to_alarm);
	else
	{
		hours_to_alarm = minutes_to_alarm / 60;
		minutes_to_alarm = minutes_to_alarm % 60;
		sprintf ("ALARM IN %02dH%ldM", hours_to_alarm, minutes_to_alarm);
	}
}

void paused_deff (void)
{
	while (task_find_gid (GID_MUTE_AND_PAUSE))
	{
		dmd_alloc_pair_clean ();
		
		font_render_string_center (&font_fixed10, 64, 10, "PAUSED");
		if (alarm_time >= ALARM_DISABLED)
		{
			/* mute_and_pause_timeout is stored as 5 second chunks, to save
			 * having to use a U16 */
			if (mute_and_pause_timeout <= 12)
				sprintf ("TIMEOUT IN %d SECONDS", mute_and_pause_timeout * 5);
			else
				sprintf ("TIMEOUT IN %d MINUTES", mute_and_pause_timeout / 12);
		}
		else
		{
			render_minutes_to_alarm ();
		}
		font_render_string_center (&font_var5, 64, 20, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "PRESS BUYIN TO CONTINUE");
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
	deff_exit ();
}

void alarm_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	sprintf ("WAKEY WAKEY");
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	sound_send (SND_CLOCK_GONG);
	deff_swap_low_high (50, TIME_66MS);
	deff_exit ();
}

static void alarm_task (void)
{
	/* To stop accidental triggering*/
	if (deff_get_active () == DEFF_PAUSED)
		task_exit ();
	U8 i;
	for (i = 0; i < 4; i++)
	{
		sound_send (SND_CLOCK_BELL);
		task_sleep_sec (1);
	}
	deff_start (DEFF_ALARM);
	task_exit ();
}

static inline bool check_alarm_time (void)
{
	if (hour >= ALARM_DISABLED)
		return FALSE;
	/* This should cover cases where the machine was switched off before the
	 * alarm could sound */
	else if (alarm_time == rtc_minutes_from_midnight ())
		return TRUE;
	else
		return FALSE;
}

CALLSET_ENTRY (alarm, minute_elapsed)
{
	if (check_alarm_time ())
	{
		alarm_time = ALARM_DISABLED;
		task_create_anon (alarm_task);
	}
}

CALLSET_ENTRY (alarm, amode_page)
{
	if (alarm_time < ALARM_DISABLED)
	{
		dmd_sched_transition (&trans_vstripe_right2left);
		dmd_map_overlay ();
		dmd_clean_page_low ();
		render_minutes_to_alarm ();
		show_text_on_stars ();
	}
}

CALLSET_ENTRY (alarm, sw_start_button)
{
	if (deff_get_active () == DEFF_PAUSED)
	{
		alarm_time = ALARM_DISABLED;
	}
}

static inline void enable_alarm (void)
{
	if (alarm_time == ALARM_DISABLED)
		alarm_time = rtc_minutes_from_midnight ();
}	

static inline void decrease_alarm_time (void)
{
	if (alarm_time > rtc_minutes_from_midnight ()
		&& alarm_time < ALARM_DISABLED)
		alarm_time--;

	if (alarm_time <= rtc_minutes_from_midnight ())
		alarm_time = ALARM_DISABLED;
}

static inline void increase_alarm_time (void)
{
	if (alarm_time < ALARM_DISABLED)
		alarm_time++;
}


CALLSET_ENTRY (alarm, sw_left_button)
{
	if (deff_get_active () == DEFF_PAUSED)
	{
		decrease_alarm_time ();
		task_sleep (TIME_200MS);
		while (switch_poll_logical (SW_LEFT_BUTTON))
			{
				task_sleep (TIME_100MS);
				decrease_alarm_time ();
			}

	}
}

CALLSET_ENTRY (alarm, sw_right_button)
{
	if (deff_get_active () == DEFF_PAUSED)
	{
		enable_alarm ();
		increase_alarm_time ();
		task_sleep (TIME_200MS);
		while (switch_poll_logical (SW_RIGHT_BUTTON))
			{
				task_sleep (TIME_100MS);
				increase_alarm_time ();
			}
	}
}

CALLSET_ENTRY (alarm, factory_reset, time_modified)
{
	alarm_time = ALARM_DISABLED;
}


CALLSET_ENTRY (alarm, display_update)
{
	if (task_find_gid (GID_MUTE_AND_PAUSE)
		&& deff_get_active () != DEFF_PAUSED)
	{
		deff_start_bg (DEFF_PAUSED, 0);	
	}
}
