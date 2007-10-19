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
}

void tz_magnet_test_draw (void)
{
	struct magnet_test_option *opt = &magnet_test_options[mode];
	U8 id;

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 4, "MAGNET TEST");
	font_render_string_center (&font_mono5, 64, 12, opt->name);

	for (id = 0; id < 3; id++)
		if (opt->id & (1 << id))
		{
			magnet_enable_catch (id);
		}
		else
		{
			magnet_disable_catch (id);
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

struct window_ops tz_magnet_test_window = {
	DEFAULT_WINDOW,
	.init = tz_magnet_test_init,
	.draw = tz_magnet_test_draw,
	.enter = tz_magnet_test_enter,
	.up = tz_magnet_test_up,
	.down = tz_magnet_test_down,
};


struct menu tz_magnet_test_item = {
	.name = "MAGNET TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_magnet_test_window, NULL } },
};

