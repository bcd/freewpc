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
#include <window.h>
#include <test.h>
#include <clock_mech.h>

S8 clock_test_setting;

U8 clock_can_run;
extern U8 clock_minute_sw;
extern U8 clock_hour;
extern U8 clock_sw_seen_active;
extern U8 clock_sw_seen_inactive;

void tz_clock_test_update (void)
{
	if (!clock_can_run)
		tz_clock_stop ();
	else
	{
		switch (clock_test_setting)
		{
			case -3:
				clock_mech_set_speed (BIVAR_DUTY_100);
				tz_clock_start_backward ();
				break;
			case -2:
				clock_mech_set_speed (BIVAR_DUTY_50);
				tz_clock_start_backward ();
				break;
			case -1:
				clock_mech_set_speed (BIVAR_DUTY_25);
				tz_clock_start_backward ();
				break;
			case 0:
				tz_clock_stop ();
				break;
			case 1:
				clock_mech_set_speed (BIVAR_DUTY_25);
				tz_clock_start_forward ();
				break;
			case 2:
				clock_mech_set_speed (BIVAR_DUTY_50);
				tz_clock_start_forward ();
				break;
			case 3:
				clock_mech_set_speed (BIVAR_DUTY_100);
				tz_clock_start_forward ();
				break;
		}
	}
}


void tz_clock_test_init (void)
{
	tz_clock_stop ();
	clock_test_setting = 0;
	clock_can_run = 1;
	tz_clock_test_update ();
}


extern __fastram__ U8 clock_sw;
extern U8 tz_clock_opto_to_hour[];

void tz_clock_test_draw (void)
{
	extern __machine__ U8 tz_clock_gettime (void);
	U8 intervals;
	U8 hour;
	U8 minute;

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "CLOCK MECH. TEST");
	switch (clock_test_setting)
	{
		case -3: sprintf ("REV. FAST"); break;
		case -2: sprintf ("REV. MID"); break;
		case -1: sprintf ("REV. SLOW"); break;
		case 0: sprintf ("NO SPEED"); break;
		case 1: sprintf ("FWD. SLOW"); break;
		case 2: sprintf ("FWD. MID"); break;
		case 3: sprintf ("FWD. FAST"); break;
	}
	font_render_string_center (&font_mono5, 32, 11, sprintf_buffer);

	font_render_string_center (&font_mono5, 96, 11,
		clock_can_run ? "RUNNING" : "STOPPED");

	intervals = tz_clock_gettime ();
	hour = intervals / 4;
	minute = (intervals % 4 * 15);

	//sprintf ("%02d:%02d", hour, minute);
	sprintf ("MIN: %02X", clock_minute_sw);
	font_render_string_center (&font_mono5, 32, 18, sprintf_buffer);
	sprintf ("HOUR: %d", clock_hour);
	font_render_string_center (&font_mono5, 32, 24, sprintf_buffer);

	sprintf ("SW.: %02X", clock_sw);
	font_render_string_center (&font_mono5, 96, 18, sprintf_buffer);

	sprintf ("ACTIVE: %02X", (clock_sw_seen_active & clock_sw_seen_inactive));
	font_render_string_center (&font_mono5, 96, 24, sprintf_buffer);
	dmd_show_low ();
}


void tz_clock_test_down (void)
{
	if (clock_test_setting > -3)
		clock_test_setting--;
	tz_clock_test_update ();
}


void tz_clock_test_up (void)
{
	if (clock_test_setting < 3)
		clock_test_setting++;
	tz_clock_test_update ();
}


void tz_clock_test_enter (void)
{
	/* Start/stop the clock */
	clock_can_run ^= 1;
	tz_clock_test_update ();
}

void tz_clock_test_start (void)
{
	//tz_clock_reset ();
}

void tz_clock_test_right (void)
{
	/* Set as 12:00 */
	callset_invoke (clock_at_home);
}

void tz_clock_test_thread (void)
{
	for (;;)
	{
		tz_clock_test_draw ();
		task_sleep (TIME_66MS);
	}
}

struct window_ops tz_clock_test_window = {
	DEFAULT_WINDOW,
	.init = tz_clock_test_init,
	.draw = tz_clock_test_draw,
	.up = tz_clock_test_up,
	.down = tz_clock_test_down,
	.right = tz_clock_test_right,
	.exit = tz_clock_stop,
	.enter = tz_clock_test_enter,
	.start = tz_clock_test_start,
	.thread = tz_clock_test_thread,
};


struct menu tz_clock_test_item = {
	.name = "CLOCK MECH. TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_clock_test_window, NULL } },
};

