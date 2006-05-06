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

__local__ int multiballs_started;


void piano_to_slot_timer (void)
{
	task_sleep_sec (4);
	task_exit ();
}


void sw_piano_handler (void)
{
	extern void door_award_flashing (void);

	task_recreate_gid (GID_SLOT_DISABLED_BY_PIANO, piano_to_slot_timer);

	score_add_current_const (0x5130);
	sound_send (SND_ODD_CHANGE_BEGIN);
	device_multiball_set (3);
}


CALLSET_ENTRY(piano, start_ball)
{
}


DECLARE_SWITCH_DRIVER (sw_piano)
{
	.fn = sw_piano_handler,
};

