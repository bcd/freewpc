/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
	psprintf ("1 LEFT RAMP", "%d LEFT RAMPS", left_ramps);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);

	if (left_ramps < 3 && config_timed_game)
		sprintf ("EXTRA TIME AT 3");
	else if (left_ramps == 3 && config_timed_game)
		sprintf ("15 SECS. ADDED");
	else if (left_ramps < 3 && !config_timed_game)
		sprintf ("MYSTERY AT 3");
	else if (left_ramps == 3 && !config_timed_game)
		sprintf ("MYSTERY IS LIT");
	else if (left_ramps < 6)
		sprintf ("SPOT PANEL AT 6");
	else if (left_ramps == 6)
		sprintf ("PANEL SPOTTED");
	else
		sprintf ("");
	font_render_string_center (&font_fixed6, 64, 21, sprintf_buffer);

	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


CALLSET_ENTRY(leftramp, start_player)
{
	left_ramps = 0;
}

static void maybe_ramp_divert (void)
{
	if (lamp_flash_test (LM_MULTIBALL))
	{
		ramp_divert_to_autoplunger ();
	}
	else if (lamp_test (LM_SUPER_SKILL))
	{
		ramp_divert ();
	}
}

CALLSET_ENTRY (left_ramp, sw_left_ramp_enter)
{
	maybe_ramp_divert ();
	score (SC_1K);
}

CALLSET_ENTRY (left_ramp, sw_left_ramp_exit)
{
	maybe_ramp_divert ();
	bounded_increment (left_ramps, 250);
	deff_start (DEFF_LEFT_RAMP);
	leff_start (LEFF_LEFT_RAMP);
	score (SC_250K);
}

