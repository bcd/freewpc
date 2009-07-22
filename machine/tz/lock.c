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

#include <freewpc.h>


CALLSET_ENTRY (lock, dev_lock_enter)
{
	if (lamp_test (LM_PANEL_FAST_LOCK))
		score (SC_250K);
	else
		score (SC_50K);
	sound_send (SND_ROBOT_FLICKS_GUN);
	leff_start (LEFF_LOCK);
}


CALLSET_ENTRY (lock, dev_lock_kick_attempt)
{
	for (;;)
	{
		if (!task_find_gid (GID_AUTOFIRE_HANDLER))
			break;
		task_sleep (TIME_100MS);
	}

	sound_send (SND_LOCK_KICKOUT);
	event_can_follow (dev_lock_kick_attempt, right_loop, TIME_2S);
}

