/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION ( tz_flipcode_enter, __machine2__ ) */
#include <freewpc.h>

extern U8 initials_data[];

CALLSET_ENTRY (tz_flipcode, tz_flipcode_entry)
{
	SECTION_VOIDCALL (__common__, initials_enter);
}

CALLSET_ENTRY (tz_flipce, tz_flipcode_entry_stop)
{
	SECTION_VOIDCALL (__common__, initials_stop);
}
