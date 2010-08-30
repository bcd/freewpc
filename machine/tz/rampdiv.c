/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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
#include <ramp_div.h>

void ramp_divert (void)
{
	ramp_div_start ();
	task_yield ();
}

void ramp_divert_to_autoplunger (void)
{
	ramp_divert ();
	autofire_catch ();
}

/* Close the ramp again when successful */
CALLSET_ENTRY (rampdiv, sw_shooter)
{
	ramp_div_stop ();
}

CALLSET_ENTRY (rampdiv, sw_autofire2)
{
	ramp_div_stop ();
}

