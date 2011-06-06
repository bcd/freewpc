/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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


extern void magnet_enable_catch (U8);
extern void magnet_disable_catch (U8);
extern bool juggle_ball;

extern bool left_magnet_enabled_to_throw, lower_right_magnet_enabled_to_throw;
/* 'Fudge' number to try and learn timings of the magnet throw */
extern U8 left_magnet_swag, lower_right_magnet_swag;
extern U8 left_magnet_throw_successes, lower_right_magnet_throw_successes;
extern bool magnets_enabled;

struct magnet_test_option {
	U8 sw;
	const char *name;
	U8 id;
} magnet_test_options[] = {
	{ 0, "NO CATCH", 0 },
	{ SW_LEFT_MAGNET, "LEFT CATCH", 0x1 },
	{ SW_LOWER_RIGHT_MAGNET, "RIGHT CATCH", 0x4 },
	{ 0, "ANY CATCH", 0x5 },
};

#define MAGNET_TEST_OPTIONS 4

U8 mode;

void tz_magnet_test_init (void)
{
	if (!task_find_gid (GID_MAGNET_ENABLE_MONITOR))
		task_create_gid (GID_MAGNET_ENABLE_MONITOR, magnet_enable_monitor_task);
	magnets_enabled = TRUE;
}

void tz_magnet_test_exit (void)
{
	task_kill_gid (GID_MAGNET_ENABLE_MONITOR);
	juggle_ball = FALSE;
	magnets_enabled = FALSE;
}

void tz_magnet_test_draw (void)
{
	struct magnet_test_option *opt = &magnet_test_options[mode];
	U8 id;

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "MAGNET TEST");
	font_render_string_center (&font_mono5, 64, 8, opt->name);
	sprintf ("SW: %d", left_magnet_swag);
	font_render_string_center (&font_mono5, 32, 16, sprintf_buffer);
	sprintf ("SU: %d", left_magnet_throw_successes);
	font_render_string_center (&font_mono5, 32, 24, sprintf_buffer);
	sprintf ("SW: %d", lower_right_magnet_swag);
	font_render_string_center (&font_mono5, 96, 16, sprintf_buffer);
	sprintf ("SU: %d", lower_right_magnet_throw_successes);
	font_render_string_center (&font_mono5, 96, 24, sprintf_buffer);
	if (juggle_ball)
		font_render_string_center (&font_mono5, 64, 28, "J");
		
	for (id = 0; id < 3; id++)
	{
		if (opt->id & (1 << id))
		{
			magnet_enable_catch_and_throw (id);
		}
		else if (!juggle_ball)
		{
			magnet_disable_catch (id);
		}
	}		
	dmd_show_low ();
}

void tz_magnet_test_enter (void)
{
	autofire_add_ball ();
}

void tz_magnet_test_up (void)
{
	mode++;
	mode &= 3;
}

void tz_magnet_test_down (void)
{
	mode--;
	mode &= 3;
}

void tz_magnet_test_left (void)
{
	if (mode == 1)
	{
		left_magnet_swag--;
	}
	else if (mode == 2)
	{
		lower_right_magnet_swag--;
	}
}

void tz_magnet_test_right (void)
{	
	if (mode == 1)
	{
		left_magnet_swag++;
	}
	else if (mode == 2)
	{
		lower_right_magnet_swag++;
	}

}

void tz_magnet_test_start (void)
{
	if (juggle_ball)
		juggle_ball = FALSE;
	else
		juggle_ball = TRUE;
}

struct window_ops tz_magnet_test_window = {
	DEFAULT_WINDOW,
	.init = tz_magnet_test_init,
	.draw = tz_magnet_test_draw,
	.enter = tz_magnet_test_enter,
	.up = tz_magnet_test_up,
	.down = tz_magnet_test_down,
	.left = tz_magnet_test_left,
	.right = tz_magnet_test_right,
	.start = tz_magnet_test_start,
};


struct menu tz_magnet_test_item = {
	.name = "MAGNET TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_magnet_test_window, NULL } },
};

