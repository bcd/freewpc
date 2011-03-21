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

/* CALLSET_SECTION (piano, __machine2__) */

#include <freewpc.h>

CALLSET_ENTRY (piano, sw_piano)
{	/* Say that a ball entering this device is destined to
	enter the slot machine device, within a certain amount
	of time.  This information can only be used reliably
	during single-ball play.  It is currently used for 
	pausing timers. */

	device_switch_can_follow (piano, slot, TIME_3S + TIME_200MS);
}
