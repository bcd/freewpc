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

__local__ U8 clock_hits;


void clock_millions_hit_deff (void)
{
	generic_deff ("CLOCK MILLIONS", "5,000,000");
}


CALLSET_ENTRY (clocktarget, sw_clock_target)
{
	if (!lamp_test (LM_PANEL_CLOCK_MIL) && !lamp_test (LM_PANEL_CLOCK_CHAOS))
	{
		score (SC_50K);
		sound_send (SND_NO_CREDITS);
		return;
	}

	sound_send (SND_CLOCK_BELL);
	leff_start (LEFF_CLOCK_TARGET);
	deff_start (DEFF_CLOCK_MILLIONS_HIT);
	score (SC_5M);

	if (clock_hits < 4)
		clock_hits++;
}


CALLSET_ENTRY(clocktarget, start_player)
{
	clock_hits = 0;
}


CALLSET_ENTRY (clocktarget, start_ball)
{
	lamp_tristate_off (LM_CLOCK_MILLIONS);
}


CALLSET_ENTRY (clocktarget, door_start_clock_millions)
{
	lamp_tristate_flash (LM_CLOCK_MILLIONS);
}

