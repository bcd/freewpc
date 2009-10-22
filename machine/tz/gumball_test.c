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
//TODO Service trough kicker and gumball_enable_from_trough sticks on

#include <freewpc.h>
#include <window.h>
#include <test.h>

#define TEST_GUMBALL_LOAD 0
#define TEST_GUMBALL_RELEASE 1
#define TEST_GUMBALL_EMPTY 2


U8 gumball_op;


typedef struct {
	U8 sw;
	const char *name;
	union dmd_coordinate box_coord;
	union dmd_coordinate text_coord;
} switch_monitor_t;

#define SW_MON_COORD(x,y) \
	.box_coord = MKCOORD(x, y), .text_coord = MKCOORD(x+6, y)

const switch_monitor_t tz_gumball_test_switch_monitor[] = {
	{ SW_LOWER_RIGHT_MAGNET, "MAGNET", SW_MON_COORD(1, 17), },
	{ SW_GUMBALL_LANE, "LANE", SW_MON_COORD(1, 24), },
	{ SW_GUMBALL_POPPER, "POPPER", SW_MON_COORD (45, 17), },
	{ SW_GUMBALL_ENTER, "ENTER", SW_MON_COORD(45, 24), },
	{ SW_GUMBALL_GENEVA, "GENEVA", SW_MON_COORD(90, 17), },
	{ SW_GUMBALL_EXIT, "EXIT", SW_MON_COORD(90, 24), },
	{ 0, NULL, SW_MON_COORD(0, 0), },
};

#define NUM_GUMBALL_SWITCHES	\
	(sizeof(tz_gumball_test_switch_monitor) /  \
		sizeof (tz_gumball_test_switch_monitor[0]))


void switch_monitor_prepare (const switch_monitor_t *monitor)
{
	while (monitor->name != NULL)
	{
		font_render_string_left (&font_var5, monitor->text_coord.x,
			monitor->text_coord.y, monitor->name);
		monitor++;
	}
}


void switch_monitor_poll (const switch_monitor_t *monitor)
{
	while (monitor->name != NULL)
	{
		if (switch_poll_logical (monitor->sw))
			bitmap_draw (monitor->box_coord, BM_X5);
		else
			bitmap_draw (monitor->box_coord, BM_BOX5);
		monitor++;
	}
}


void tz_gumball_test_init (void)
{
	gumball_op = 0;
	magnet_disable_catch (MAG_RIGHT);
}


void tz_gumball_test_draw (void)
{
	extern U8 gumball_count;

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "GUMBALL TEST");
	switch (gumball_op)
	{
		case TEST_GUMBALL_LOAD:
			sprintf ("LOAD"); break;
		case TEST_GUMBALL_RELEASE:
			sprintf ("RELEASE"); break;
	}
	font_render_string_center (&font_mono5, 32, 10, sprintf_buffer);

	sprintf ("%d", gumball_count);
	font_render_string_center (&font_mono5, 96, 10, sprintf_buffer);
	switch_monitor_prepare (tz_gumball_test_switch_monitor);
	dmd_show_low ();
}


void tz_gumball_test_thread (void)
{
	for (;;)
	{
		tz_gumball_test_draw ();
		switch_monitor_poll (tz_gumball_test_switch_monitor);
		task_sleep (TIME_33MS);
		switch_monitor_poll (tz_gumball_test_switch_monitor);
		task_sleep (TIME_33MS);
	}
}

void tz_gumball_test_down (void)
{
	sound_send (SND_TEST_UP);
	gumball_op = 1 - gumball_op;
}


void tz_gumball_test_up (void)
{
	sound_send (SND_TEST_DOWN);
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
	sound_send (SND_TEST_ENTER);
}


struct window_ops tz_gumball_test_window = {
	DEFAULT_WINDOW,
	.init = tz_gumball_test_init,
	.draw = tz_gumball_test_draw,
	.up = tz_gumball_test_up,
	.down = tz_gumball_test_down,
	.enter = tz_gumball_test_enter,
	.thread = tz_gumball_test_thread,
};


struct menu tz_gumball_test_item = {
	.name = "GUMBALL TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_gumball_test_window, NULL } },
};
