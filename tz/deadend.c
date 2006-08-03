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

__local__ U8 dead_end_value;


CALLSET_ENTRY (deadend, start_ball)
{
	dead_end_value = 100;
}


void sw_deadend_handler (void)
{
	score (SC_100K);
	switch_can_follow (dead_end, slot, TIME_8S);
	switch_can_follow (dead_end, camera, TIME_4S);
}



DECLARE_SWITCH_DRIVER (sw_deadend)
{
	.fn = sw_deadend_handler,
	.sound = SND_DEAD_END_SCREECH,
};

