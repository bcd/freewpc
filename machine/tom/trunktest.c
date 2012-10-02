/*
 * Copyright 2006, 2007, 2008, 2010 by Brian Dominy <brian@oddchange.com>
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
#include <trunk_motor.h>

S8 tom_trunk_test_mode;

U8 tom_trunk_motor_on;


void tom_trunk_test_update (void)
{
	if (!tom_trunk_motor_on)
		trunk_motor_stop ();
	else
	{
		switch (tom_trunk_test_mode)
		{
			case -2:
				trunk_motor_set_speed (BIVAR_DUTY_100);
				trunk_motor_start_reverse ();
				break;
			case -1:
				trunk_motor_set_speed (BIVAR_DUTY_25);
				trunk_motor_start_reverse ();
				break;
			case 0:
				trunk_motor_stop ();
				break;
			case 1:
				trunk_motor_set_speed (BIVAR_DUTY_25);
				trunk_motor_start_forward ();
				break;
			case 2:
				trunk_motor_set_speed (BIVAR_DUTY_100);
				trunk_motor_start_forward ();
				break;
		}
	}
}


void tom_trunk_test_init (void)
{
	trunk_motor_stop ();
	tom_trunk_test_mode = 0;
	tom_trunk_motor_on = 1;
	tom_trunk_test_update ();
}


extern U8 trunk_curr_pos;

void tom_trunk_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "TRUNK MOTOR TEST");
	switch (tom_trunk_test_mode)
	{
		case -2: sprintf ("REV. FAST"); break;
		case -1: sprintf ("REV. SLOW"); break;
		case 0: sprintf ("NO SPEED"); break;
		case 1: sprintf ("FWD. SLOW"); break;
		case 2: sprintf ("FWD. FAST"); break;
	}
	font_render_string_center (&font_mono5, 32, 11, sprintf_buffer);

	font_render_string_center (&font_mono5, 96, 11,
		tom_trunk_motor_on ? "RUNNING" : "STOPPED");

	sprintf ("POS %1d", trunk_curr_pos);
	font_render_string_center (&font_mono5, 96, 18, sprintf_buffer);

	dmd_show_low ();
}


void tom_trunk_test_down (void)
{
	if (tom_trunk_test_mode > -2)
		tom_trunk_test_mode--;
	tom_trunk_test_update ();
}


void tom_trunk_test_up (void)
{
	if (tom_trunk_test_mode < 2)
		tom_trunk_test_mode++;
	tom_trunk_test_update ();
}


void tom_trunk_test_enter (void)
{
	/* Start/stop the clock */
	tom_trunk_motor_on ^= 1;
	tom_trunk_test_update ();
}

void tom_trunk_test_thread (void)
{
	for (;;)
	{
		tom_trunk_test_draw ();
		task_sleep (TIME_66MS);
	}
}

struct window_ops tom_trunk_test_window = {
	DEFAULT_WINDOW,
	.init = tom_trunk_test_init,
	.draw = tom_trunk_test_draw,
	.up = tom_trunk_test_up,
	.down = tom_trunk_test_down,
	.exit = trunk_motor_stop,
	.enter = tom_trunk_test_enter,
	.thread = tom_trunk_test_thread,
};


struct menu tom_trunk_test_item = {
	.name = "TRUNK TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tom_trunk_test_window, NULL } },
};

