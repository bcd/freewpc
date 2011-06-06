/*
 * Copyright 2006, 2007, 2009 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (lock, __machine3__ ) */

/* More code for the lock is in Multiball.c */

#include <freewpc.h>
#include <eb.h>


CALLSET_ENTRY (lock, dev_lock_kick_attempt)
{
	while (timer_find_gid (GID_AUTOFIRE_HANDLER))
	{
		task_sleep (TIME_100MS);
	}

	sound_send (SND_LOCK_KICKOUT);
	/* Used to disable camera magnet grab */
	timer_restart_free (GID_LOCK_KICKED, TIME_3S);
	magnet_disable_catch (MAG_RIGHT);
}
