/*
 * Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/** The task that runs a timed mode.
 * begin, expire, and end are all optional function pointers to be
 * invoked at various points during the lifecycle of the mode.
 * expire is called when the timer hits zero; end is called when the
 * grace period is finished.
 * timer is a pointer to the 1-byte timer object.
 * mode_time and grace_time give the duration of the mode in seconds.
 */
extern inline void timed_mode_task (
	void (*begin) (void),
	void (*expire) (void),
	void (*end) (void),
	U8 *timer,
	U8 mode_time,
	U8 grace_time
	)
{
	/* Mark the task as protected, because killing it will not do all
	of the cleanup needed for ending the mode.  The proper way to end
	a mode is to invoke mode_stop(). */
	task_set_flags (TASK_PROTECTED);

	/* Initialize the timer and invoke the mode's begin hook */
	*timer = mode_time;
	if (begin)
		begin ();

	/* Run the timer down to zero.  Check for balls held up in
	devices temporarily; this will pause the timer. */
	do {
		do { 
			task_sleep (TIME_1S + TIME_66MS);
		} while (device_holdup_count ());

		if (*timer > 0)
		{
			(*timer)--;
		}
	} while (*timer != 0);

	/* If a grace period is required, then run the first second 
	of it here. */
	if ((grace_time >= 1)
		&& !in_tilt
		&& !in_bonus)
	{
		task_sleep_sec (1);
	}

	/* Invoke the expire hook */
	if (expire)
		expire ();

	/* Run the rest of the grace period out */
	if ((grace_time >= 1)
		&& !in_tilt
		&& !in_bonus)
	{
		task_sleep_sec (grace_time-1);
	}

	/* Invoke the end hook */
	if (end)
		end ();

	/* End the mode */
	task_exit ();
}


/** Starts a mode task */
extern inline task_pid_t timed_mode_start (U8 gid, void (*task_function) (void))
{
	task_pid_t tp = task_create_gid1 (gid, task_function);
#ifdef CONFIG_NATIVE
	/* TODO */
#else
	tp->state |= TASK_GAME;
#endif
	return tp;
}


/** Returns true if the mode with gid GID and timer TIMER
is still active.  It may be in its grace period. */
extern inline bool timed_mode_active_p (U8 gid, __attribute__((unused)) U8 *timer)
{
	return task_find_gid (gid) ? TRUE : FALSE;
}


/** Returns true if a mode is running with a nonzero timer.  A mode
in grace period is considered NOT to be running. */
extern inline bool timed_mode_timer_running_p (__attribute__((unused)) U8 gid, U8 *timer)
{
	return (*timer != 0);
}


/** Extends a mode timer by some amount, up to a fixed value. */
extern inline void timed_mode_extend (U8 *timer, U8 extra_time, U8 max_time)
{
	if (*timer == 0)
	{
		/* TODO */
		/* Here, the mode is technically not running but still
		active in its grace period, and more time is being added.
		Just incrementing the timer will not signal the mode task
		to go out of grace period back into its normal loop; something
		more is needed... */

		/* Note this means that the 'expire' hook can be called
		multiple times.  It also means we may need some sort of
		'restart' hook just for this case. */
	}

	/* Just increment the timer */
	*timer += extra_time;
	if (*timer >= max_time)
	{
		*timer = max_time;
	}
}


/** Stops a mode task by setting its timer to zero.  The mode task will
detect this and exit on its own.  The end_ball hook for a mode module
should stop the timer this way. */
extern inline void timed_mode_stop (U8 *timer)
{
	*timer = 0;
}


#endif /* _MODE_H */

/* vim: set ts=3: */
