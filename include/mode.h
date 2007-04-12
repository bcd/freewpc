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

#ifndef _MODE_H
#define _MODE_H

extern inline void mode_start (
	void (*begin) (void),
	void (*expire) (void),
	void (*end) (void),
	U8 *timer,
	U8 mode_time,
	U8 grace_time
	)
{
	task_set_flags (TASK_PROTECTED);

	*timer = mode_time;
	if (begin)
		begin ();

	do {
		task_sleep (TIME_1S + TIME_66MS);
		if (held_balls)
			continue;
	} while (--*timer != 0);

	if ((grace_time >= 1)
		&& !in_tilt
		&& !in_bonus)
	{
		task_sleep_sec (1);
	}

	if (expire)
		expire ();

	if ((grace_time >= 1)
		&& !in_tilt
		&& !in_bonus)
	{
		task_sleep_sec (grace_time-1);
	}

	if (end)
		end ();
	task_exit ();
}


extern inline void mode_stop (U8 *timer)
{
	*timer = 0;
}


#endif /* _MODE_H */

/* vim: set ts=3: */
