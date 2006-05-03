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

void deadend_to_slot_timer (void)
{
	task_sleep_sec (4);
	task_exit ();
}


void sw_deadend_handler (void)
{
	score_add_current_const (SCORE_10K * 7 + SCORE_5K);
	task_recreate_gid (GID_SLOT_DISABLED_BY_DEAD_END, deadend_to_slot_timer);
	timer_restart (GID_CAMERA_DISABLED_BY_DEAD_END, TIME_4S, freerunning_timer_function);
}


CALLSET_ENTRY(deadend, start_ball)
{
}


DECLARE_SWITCH_DRIVER (sw_deadend)
{
	.fn = sw_deadend_handler,
	.sound = SND_ODD_CHANGE_BEGIN,
};

