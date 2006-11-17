/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
#include <test.h>

#define TEST_GUMBALL_LOAD 0
#define TEST_GUMBALL_RELEASE 1
#define TEST_GUMBALL_EMPTY 2


U8 gumball_op;


void tz_gumball_test_init (void)
{
	gumball_op = 0;
}


void tz_gumball_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "GUMBALL TEST");
	switch (gumball_op)
	{
		case TEST_GUMBALL_LOAD:
			sprintf ("LOAD"); break;
		case TEST_GUMBALL_RELEASE:
			sprintf ("RELEASE"); break;
	}
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	dmd_show_low ();
}


void tz_gumball_test_down (void)
{
	gumball_op = 1 - gumball_op;
}


void tz_gumball_test_up (void)
{
	gumball_op = 1 - gumball_op;
}


void tz_gumball_test_enter (void)
{
	switch (gumball_op)
	{
		case TEST_GUMBALL_LOAD:
			gumball_load_from_trough ();
			break;

		case TEST_GUMBALL_RELEASE:
			gumball_release ();
			break;
	}
}


struct window_ops tz_gumball_test_window = {
	DEFAULT_WINDOW,
	.init = tz_gumball_test_init,
	.draw = tz_gumball_test_draw,
	.up = tz_gumball_test_up,
	.down = tz_gumball_test_down,
	.enter = tz_gumball_test_enter,
};


struct menu tz_gumball_test_item = {
	.name = "GUMBALL TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_gumball_test_window, NULL } },
};

