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

__local__ U8 clock_hits;

void sw_clock_target_handler (void)
{
	score_add_current_const (SCORE_25K);
}


CALLSET_ENTRY(clocktarget, start_ball)
{
	lamp_tristate_off (LM_CLOCK_MILLIONS);
	clock_hits = 0;
}


DECLARE_SWITCH_DRIVER (sw_clock_target)
{
	.fn = sw_clock_target_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.sound = SND_NO_CREDITS,
	.lamp = LM_CLOCK_MILLIONS,
};

