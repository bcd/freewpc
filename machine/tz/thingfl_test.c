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

U16 loop_speed_stored;
__fastram__ U8 thingfl_test_delay;

extern U8 thing_flips_successes;
extern U8 thing_flips_attempts;
extern U8 loop_time;
extern void autofire_add_ball (void);
extern void thing_flips (void);
extern void stop_loop_speed_timer (void);
extern void start_loop_speed_timer (void);

void tz_thingfl_test_draw (void)
{
	//if (loop_speed < loop_speed_stored)
		loop_speed_stored = loop_time;
	
	font_render_string_center (&font_mono5, 64, 2, "THING FLIPS TEST");
	sprintf("%d OUT OF %d", thing_flips_successes, thing_flips_attempts);
	font_render_string_center (&font_mono5, 64, 9, sprintf_buffer);
	sprintf(" %d DELAY", thingfl_test_delay);
	font_render_string_left (&font_mono5, 64, 20, sprintf_buffer);
	sprintf("%ld SPEED", loop_speed_stored);
	font_render_string_right (&font_mono5, 64, 20, sprintf_buffer);
}
void tz_thingfl_test_init (void)
{
	thingfl_test_delay = 1;
}

void tz_thingfl_test_enter (void)
{
	loop_speed_stored = 0;
	autofire_add_ball ();
}

void tz_thingfl_test_up (void)
{
	thingfl_test_delay++;
}

void tz_thingfl_test_down (void)
{
	thingfl_test_delay--;
}

void tz_thingfl_test_thread (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		tz_thingfl_test_draw ();
		dmd_show_low ();
		task_sleep (TIME_100MS);
	}
}

CALLSET_ENTRY (thingfl_test, sw_lower_right_magnet)
{
	if (!in_test)
	{
		return;
	}
	else
	{
		start_loop_speed_timer ();
	}
}


CALLSET_ENTRY (thingfl_test, sw_left_magnet)
{
	if (!in_test)
	{
		return;
	}
	else 
	{
		stop_loop_speed_timer ();
		thing_flips ();
	}
}

struct window_ops tz_thingfl_test_window = {
	DEFAULT_WINDOW,
	.init = tz_thingfl_test_init,
	.draw = tz_thingfl_test_draw,
	.up = tz_thingfl_test_up,
	.down = tz_thingfl_test_down,
	.enter = tz_thingfl_test_enter,
	.thread = tz_thingfl_test_thread,
};


struct menu tz_thingfl_test_item = {
	.name = "THINGFL TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_thingfl_test_window, NULL } },
};

