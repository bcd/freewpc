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


void sw_piano_handler (void)
{
	extern void door_award_flashing (void);

	switch_can_follow (piano, slot, TIME_4S);

	score (SC_5130);
	sound_send (SND_ODD_CHANGE_BEGIN);
	if (lamp_flash_test (LM_SLOT_MACHINE))
		door_award_flashing ();
}


CALLSET_ENTRY(piano, start_ball)
{
}


DECLARE_SWITCH_DRIVER (sw_piano)
{
	.fn = sw_piano_handler,
};

