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

bool alarm_enabled;
/* Time in minutes before the alarm goes off */
U8 alarm_timer;

extern U8 mute_and_pause_timeout;

void paused_deff (void)
{
	bool on = TRUE;	
	while (task_find_gid (GID_MUTE_AND_PAUSE))
	{
		dmd_alloc_pair_clean ();
		
		if (on)
		{
			font_render_string_center (&font_fixed10, 64, 10, "PAUSED");
			on = FALSE;
		}
		else
		{
			on  = TRUE;
		}
		/* mute_and_pause_timeout is stored as 5 second chunks, to save
		 * having to use a U16 */
		if (!alarm_enabled)
		{
			if (mute_and_pause_timeout <= 12)
				sprintf ("TIMEOUT IN %d SECONDS", mute_and_pause_timeout * 5);
			else
				sprintf ("TIMEOUT IN %d MINUTES", mute_and_pause_timeout / 12);
		}
		else
			sprintf ("ALARM IN %d MINUTES", alarm_timer);
			
		font_render_string_center (&font_var5, 64, 20, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "PRESS BUYIN TO CONTINUE");
		dmd_show_low ();
		task_sleep (TIME_500MS);
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

CALLSET_ENTRY (alarm, amode_page)
{
	if (alarm_enabled)
	{
		dmd_alloc_pair_clean ();
		sprintf ("ALARM IN %d MINUTES", alarm_timer);
		font_render_string_center (&font_var5, 64, 16, sprintf_buffer);
		show_text_on_stars ();
	}
}

CALLSET_ENTRY (alarm, sw_left_button)
{
	if (deff_get_active () == DEFF_PAUSED)
	{
		bounded_decrement (alarm_timer, 0);
		if (alarm_timer == 0)
			alarm_enabled = FALSE;
	}
}

CALLSET_ENTRY (alarm, sw_right_button)
{
	if (deff_get_active () == DEFF_PAUSED)
	{
		alarm_enabled = TRUE;
		bounded_increment (alarm_timer, 60);
	}
}


CALLSET_ENTRY (alarm, init)
{
	alarm_enabled = FALSE;
	alarm_timer = 5;
}

static void alarm_task (void)
{
	alarm_enabled = FALSE;
	U8 i;
	for (i = 0; i < 4; i++)
	{
		sound_send (SND_CLOCK_BELL);
		task_sleep_sec (1);
	}
	deff_start (DEFF_ALARM);
	task_exit ();
}

CALLSET_ENTRY (alarm, minute_elapsed)
{
	if (alarm_enabled)
	{
		bounded_decrement (alarm_timer, 0);
		if (alarm_timer == 0)
			task_create_anon (alarm_task);
	}	
}

CALLSET_ENTRY (alarm, display_update)
{
	if (task_find_gid (GID_MUTE_AND_PAUSE)
		&& deff_get_active () != DEFF_PAUSED)
	{
		deff_start_bg (DEFF_PAUSED, 0);	
	}
}
