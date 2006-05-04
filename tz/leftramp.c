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

__local__ U8 left_ramps;


void left_ramp_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d LEFT RAMPS", left_ramps);
	font_render_string_center (&font_fixed6, 64, 8, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


CALLSET_ENTRY(leftramp, start_game)
{
	left_ramps = 0;
}


void sw_left_ramp_handler (void)
{
	deff_start (DEFF_LEFT_RAMP);
	leff_start (LEFF_LEFT_RAMP);
	left_ramps++;
	score_add_current_const (SCORE_10K);
}


DECLARE_SWITCH_DRIVER (sw_left_ramp_enter)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_LEFT_RAMP_ENTER,
};


DECLARE_SWITCH_DRIVER (sw_left_ramp_exit)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_LEFT_RAMP_MADE,
	.fn = sw_left_ramp_handler,
};


