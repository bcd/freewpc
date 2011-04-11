/*
 * Copyright 2010 by Ewan Meadows <sonny_jim@hotmail.com>
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
/* 1440 minutes in a day */
#define ALARM_DISABLED 1441

/* alarm_time is stored as minutes from midnight */
__permanent__ U16 alarm_time;
__permanent__ bool alarm_persistant;

extern U8 mute_and_pause_timeout;

/* How long in minutes before the alarm goes off */
static void render_alarm_time (void)
{
	static const U8 rtc_us_hours[] = {
		12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
	};

	switch (system_config.clock_style)
	{
		case CLOCK_STYLE_AMPM:
		default:
			sprintf ("%d:%02d %s",
				rtc_us_hours[alarm_hour], alarm_minute, (alarm_hour >= 12)?"PM":"AM");
			break;

		case CLOCK_STYLE_24HOUR:
			sprintf ("%02d:%02d", alarm_hour, alarm_minute);
			break;
	}
}

static void render_minutes_to_alarm (void)
{
	U16 minutes_to_alarm;

	char s;
	if (alarm_hour > hour)
	{	
		minutes_to_alarm = ((alarm_hour - hour) * 60);
		minutes_to_alarm += alarm_minute;
	}
	else if (alarm_hour < hour)
	{
		minutes_to_alarm = (24 - hour) - alarm_hour;minutes_to_alarm
		minutes_to_alarm += alarm_minute;
		//Erm....
	}
	else if (alarm_hour = hour)
		minutes_to_alarm = alarm_minute - minute;
	sprintf ("ALARM IN %s MINUTES", minutes_to_alarm);
}

void paused_deff (void)
{
	while (task_find_gid (GID_MUTE_AND_PAUSE))
	{
		dmd_alloc_pair_clean ();
		
		font_render_string_center (&font_fixed10, 64, 10, "PAUSED");
		if (alarm_hour >= ALARM_DISABLED)
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
	alarm_hour = ALARM_DISABLED;
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
	else if (hour >= alarm_hour && minute >= alarm_minute)
		return TRUE;
	else
		return FALSE;
}

CALLSET_ENTRY (alarm, minute_elapsed)
{
	if (check_alarm_time ())
		task_create_anon (alarm_task);
}

CALLSET_ENTRY (alarm, amode_page)
{
	if (alarm_hour < ALARM_DISABLED)
	{
		dmd_alloc_pair_clean ();
		sprintf ("ALARM IN %d MINUTES", alarm_timer);
		font_render_string_center (&font_var5, 64, 16, sprintf_buffer);
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

CALLSET_ENTRY (alarm, sw_left_button)
{
	if (deff_get_active () == DEFF_PAUSED)
	{
		timer_restart_gid (GID_ALARM_BUTTON_PUSH, TIME_2S);
		bounded_decrement (alarm_time, 0);
		while (switch_poll_logical (SW_RIGHT_BUTTON))
			{
				task_sleep (TIME_200MS);
				bounded_decrement (alarm_time, 0);
			}

	}
}

CALLSET_ENTRY (alarm, sw_right_button)
{
	if (deff_get_active () == DEFF_PAUSED)
	{
		timer_restart_gid (GID_ALARM_BUTTON_PUSH, TIME_2S);
		bounded_increment (alarm_time, 0);
			while (switch_poll_logical (SW_RIGHT_BUTTON))
			{
				task_sleep (TIME_200MS);
				bounded_increment (alarm_time, ALARM_DISABLED - 1);
			}
	}
}

CALLSET_ENTRY (alarm, factory_reset)
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
