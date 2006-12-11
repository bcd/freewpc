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

/**
 * \file
 * \brief Ball device logic
 *
 * A ball device, or simply a device, is any playfield mechanism that
 * can hold and/or release a pinball.  Typically, a device consists of
 * one or more switches used to keep track of the number of balls in
 * the device, and a coil for ejecting a single ball from the device.
 * Examples include ejects (single ball), poppers, scoops, troughs, etc.
 * Devices are also sometimes referred to as containers.
 *
 * The machine description file declares the containers and their
 * properties.  This module contains all of the common code
 * for servicing requests from the rest of the kernel.  This module
 * exposes several per-device events that can be caught by the game
 * code, for example, when a ball enters a device or a kickout is
 * successful.
 *
 * The ball device code is also responsible for determining when
 * end-of-ball occurs, and keeps track of the total number of balls
 * in play at all times.
 */

#include <freewpc.h>

/** Pointer to the device properties table generated from the md */
extern device_properties_t device_properties_table[];

/** Runtime status about each device */
device_t device_table[NUM_DEVICES];

/** The global state of the entire device subsystem */
U8 device_ss_state;

/** The number of devices registered */
U8 device_count;

/** The maximum number of balls that this machine should have */
U8 max_balls;

/** The number of balls accounted for */
U8 counted_balls;

/** The number of balls that have gone missing */
U8 missing_balls;

/** The number of unaccounted balls that are assumed to be on
 * the table.  This includes balls in the plunger lane. */
U8 live_balls;

/** The number of kickout locks currently held.
 * Normally this is zero, and kickouts occur as soon as possible.
 * When nonzero, kickouts are delayed, e.g. to allow an effect to
 * run.  The lock is then released and things continue. */
U8 kickout_locks;


#ifdef DEBUGGER
void device_debug (void)
{
	devicenum_t devno;

	for (devno = 0; devno < device_count; devno++)
	{
		device_t *dev = &device_table[devno];

		db_puti (devno); 
		db_puts (") " ); 
		db_puts (dev->props->name);
		db_puts (" : "); 
		db_puti (dev->previous_count); 
		db_puts (" -> ");
		db_puti (dev->actual_count); 
		db_puts (", max ");
		db_puti (dev->max_count); 
		db_puts (", pending kicks ");
		db_puti (dev->kicks_needed); 
		db_puts (", state ");
		db_puts ((dev->state == DEV_STATE_IDLE) ? "IDLE" : "RELEASING");
		db_putc ('\n');
	}

	dbprintf ("Counted %d Missing %d Live %d\n", 
		counted_balls, missing_balls, live_balls);
}
#else
#define device_debug()
#endif


void device_clear (device_t *dev)
{
	dev->size = 0;
	dev->actual_count = 0;
	dev->previous_count = 0;
	dev->max_count = 0;
	dev->kicks_needed = 0;
	dev->kick_errors = 0;
	dev->state = DEV_STATE_IDLE;
	dev->props = NULL;
}

void device_register (devicenum_t devno, device_properties_t *props)
{
	device_t *dev;
	
	dev = &device_table[devno];
	if (dev->props != NULL)
		return;

	dev->devno = devno;
	dev->devno_mask = (1 << devno);
	dev->props = props;
	dev->size = props->sw_count;
	dev->max_count = props->init_max_count;
	device_count++;

	dbprintf ("Registering device, total count is now %d\n", device_count);
}

/* Return the number of balls currently present in the device */
uint8_t device_recount (device_t *dev)
{
	uint8_t i;
	uint8_t count = 0;

	dev->previous_count = dev->actual_count;

	for (i=0; i < dev->size; i++)
	{
		switchnum_t sw = dev->props->sw[i];
		U8 level = switch_poll_logical (sw);
		if (level)
			count++;
	}

	dev->actual_count = count;
	return (count);
}


/*
 * The core function for handling a device.
 * This function is invoked (within its own task context) whenever
 * a switch closure occurs on a device, or when a request is made to
 * kick a ball from a device.
 */
void device_update (void)
{
	device_t *dev = &device_table[task_getgid () - DEVICE_GID_BASE];

wait_and_recount:
	task_sleep_sec (1);
	device_recount (dev);
	device_update_globals ();
	device_debug ();

	dbprintf ("Updating device %s\n", dev->props->name);

	/*****************************************
	 * Handle "count" changes
	 *****************************************/
	if (dev->state == DEV_STATE_IDLE)
	{
		/* Device is idle */
		if (dev->actual_count == dev->previous_count)
		{
			/* Strange condition; switch closures were
			 * detected but in the end, the count did not
			 * change.  Nothing to do...
			 */
			db_puts ("Idle but count same, ignoring.\n");
		}
		else if (dev->actual_count < dev->previous_count)
		{
			/* Also unusual in that a ball came out of
			 * the device without explicitly kicking it.
			 * (Although this can happen in test mode.)
			 */
			if (!in_test)
			{
				db_puts ("Idle but ball lost\n");
				nonfatal (ERR_IDLE_BALL_LOST);
			}
		}
		else if (dev->actual_count > dev->previous_count)
		{
			/* More typical : when idle, the count should
			 * have gone up */
			device_call_op (dev, enter);
		}
	}
	else if ((dev->state == DEV_STATE_RELEASING) && (dev->kicks_needed > 0))
	{
		/* Device is in the middle of a release cycle */
		if (dev->actual_count == dev->previous_count)
		{
			/* After attempting a release, the count did not
			 * change ... the kick must have failed, and we
			 * should retry up to a point.  Since no state
			 * is changed, the code will get reinvoked. */

			/* TODO : not completely true.  A second ball could enter
			the device immediately after the kick.  The kick didn't
			really fail, but this is difficult to detect.  Of course
			this can only be the case during a multiball.

			If this is a multi-switch device, we can detect the difference
			because the first switch would trip again, as opposed to only
			the last switch tripping in a failed kick.  The current scheme
			only relies on ball count and doesn't care which switch caused
			the change.

			For a single-switch device, even this won't help.  However,
			for such devices we never lock a ball anyway, so treating it
			as a failure and rekicking is harmless (except that points
			may not be scored the second time).

			In any case, this is common enough that a nonfatal shouldn't
			be thrown. */
			db_puts ("Kick did not change anything\n");
			nonfatal (ERR_FAILED_KICK);
			device_call_op (dev, kick_failure);

			if (++dev->kick_errors == 5)
			{
				/* OK, we tried 5 times and still know ball came out.
				 * Cancel all kick requests. */
				db_puts ("Cancelling kick requests\n");
				dev->kicks_needed = 0;
				dev->state = DEV_STATE_IDLE;
			}
		}
		else if (dev->actual_count < dev->previous_count)
		{
			/* The count decreased as expected.  Hopefully by the
			 * same number as the number of kicks requested. */
			if (dev->actual_count == dev->previous_count - 1)
			{
				/* Well done */
				db_puts ("Kick succeeded\n");
				device_call_op (dev, kick_success);
				dev->kicks_needed--;
				if (dev->kicks_needed == 0)
				{
					/* All kicks done */
					dev->state = DEV_STATE_IDLE;
				}
			}
			else
			{
				/* More than one ball was released */
				db_puts ("Kick succeeded, but an extra ball came out\n");
				nonfatal (ERR_KICK_TOO_MANY);
			}
		}
		else if (dev->actual_count > dev->previous_count)
		{
			/* The count went up during a kick cycle.
			 * kicks_needed is presumably still nonzero, so
			 * the code below should attempt the kick again. */

			/* See long TODO comment above -- it applies here too. */
			db_puts ("After kick, count increased\n");
			nonfatal (ERR_KICK_CAUSED_INCREASE);
		}
	}

	/*****************************************
	 * Handle global count changes
	 *****************************************/
	device_update_globals ();

	/*****************************************
	 * Handle "count" limits (empty & full)
	 *****************************************/
	if (dev->actual_count != dev->previous_count)
	{
		if (dev->actual_count == 0)
			device_call_op (dev, empty);
		else if (dev->actual_count > dev->max_count)
		{
			device_call_op (dev, full);
			dev->kicks_needed++;
			dev->kick_errors = 0;
		}
	}

	/*****************************************
	 * Handle counts that are different from
	 * what the system wants
	 *****************************************/
	if (dev->kicks_needed > 0)
	{
		if (dev->actual_count == 0)
		{
			/* Container has fewer balls in it than we
			 * would like */
			db_puts ("Can't kick when no balls available!\n");
		}
		else if (kickout_locks > 0)
		{
			/* Container ready to kick, but 1 or more
			 * locks are held so we must wait. */
			goto wait_and_recount;
		}
		else 
		{
			/* Container has balls ready to kick */
			db_puts ("About to call kick_attempt\n");

			/* Mark state as releasing if still idle */
			if (dev->state == DEV_STATE_IDLE)
				dev->state = DEV_STATE_RELEASING;

			device_call_op (dev, kick_attempt);
		
			sol_pulse (dev->props->sol);
#ifdef CONFIG_TIMED_GAME
			timed_game_pause (TIME_1S);
#endif
			goto wait_and_recount;
		}
	}

	device_debug ();
	task_exit ();
}


/** Returns the number of balls that can be kicked out of
 * a device.  This is the number of balls in the device,
 * minus any pending kicks. */
static inline U8 device_kickable_count (device_t *dev)
{
	return (dev->actual_count - dev->kicks_needed);
}


/** Request that a device eject 1 ball */
void device_request_kick (device_t *dev)
{
	task_gid_t gid = DEVICE_GID (device_devno (dev));
	task_kill_gid (gid);
	if (device_kickable_count (dev) > 0)
	{
		dev->kicks_needed++;
		dev->kick_errors = 0;
#ifdef DEVNO_TROUGH
		/* TODO - this logic probably belongs somewhere else */
		if (device_devno (dev) != DEVNO_TROUGH)
			live_balls++;
#endif
	}
	task_create_gid1 (gid, device_update);
}


/** Request that a device ejects all balls */
void device_request_empty (device_t *dev)
{
	U8 can_kick;

	task_gid_t gid = DEVICE_GID (device_devno (dev));
	task_kill_gid (gid);
	if ((can_kick = device_kickable_count (dev)) > 0)
	{
		dev->kicks_needed += can_kick;
		dev->kick_errors = 0;
		dev->max_count -= can_kick;
#ifdef DEVNO_TROUGH
		/* TODO - this logic probably belongs somewhere else */
		if (device_devno (dev) != DEVNO_TROUGH)
			live_balls += can_kick;
#endif
	}
	task_create_gid1 (gid, device_update);
}


/** Update the global state of the machine.  This happens nearly
 * anytime a change happens locally within a particular device. */
void device_update_globals (void)
{
	devicenum_t devno;
	U8 held_balls = 0;

	/* Recount the number of balls that are held */
	counted_balls = 0;
	for (devno = 0; devno < device_count; devno++)
	{
		device_t *dev = device_entry (devno);
		counted_balls += dev->actual_count;

#ifdef DEVNO_TROUGH
		if (devno != DEVNO_TROUGH)
			if (dev->actual_count >= dev->max_count)
				held_balls += dev->actual_count - dev->max_count;
#endif
	}

	/* Count how many balls are missing */
	missing_balls = max_balls - counted_balls;
	if (missing_balls != live_balls)
	{
		/* Number of balls not accounted for is NOT what we expect */
		db_puts ("Missing != Live\n");
#if 0 /* this isn't right */
		if (missing_balls + 1 == live_balls)
		{
			device_request_kick (device_entry (DEVNO_TROUGH));
		}
#endif
	}

	/* If any balls are held up temporarily (more than "max" are
	 * in the device presently), then delay timers */
	dbprintf ("held_balls = %d\n", held_balls);
	if (held_balls > 0)
	{
#ifdef CONFIG_TIMED_GAME
		timed_game_suspend ();
#endif
	}
	else
	{
#ifdef CONFIG_TIMED_GAME
		timed_game_resume ();
#endif
	}
}


/** Probes all devices to see if any balls are present that
 * shouldn't be.  Balls are kicked as necessary.
 *
 * For example, this clears out the balls of a lockup device
 * at the end of a game.
 */
void device_probe (void)
{
	devicenum_t devno;

	task_sleep_sec (1);

	for (devno = 0; devno < device_count; devno++)
	{
		device_t *dev = device_entry (devno);
		device_recount (dev);
		dev->previous_count = dev->actual_count;
		dev->kicks_needed = 0;
		dev->kick_errors = 0;
		task_kill_gid (DEVICE_GID(devno));

		/* If there are more balls in the device than ought to be,
		 * schedule the extras to be emptied. */
		dev->max_count = dev->props->init_max_count;
		if (dev->actual_count > dev->max_count)
		{
			U8 kick_count = dev->actual_count - dev->max_count;
			do {
				device_request_kick (dev);
			} while (--kick_count != 0);
		}
	}

	/* Update the global state information again based on the
	 * new counts */
	device_update_globals ();

	device_ss_state = 1;

	device_debug ();
	task_exit ();
}


/* Called from a switch handler to do the common processing */
void device_sw_handler (uint8_t devno)
{
#ifdef DEBUGGER
	device_t *dev = device_entry (devno);
#endif

	dbprintf ("Device switch handler for %s\n", dev->props->name);

	if (device_ss_state == 0)
	{
		dbprintf ("Aborting because haven't probed yet\n");
		return;
	}

	if (task_find_gid (DEVICE_GID (devno)))
	{
		dbprintf ("Aborting because update already running\n");
		return;
	}

	task_create_gid (DEVICE_GID (devno), device_update);
}


void device_add_live (void)
{
	if (missing_balls > 0)
	{
		missing_balls--;
		live_balls++;
		if (in_game)
		{
			callset_invoke (ball_count_change);
		}
	}
}


/** Called by the trough when a ball has entered it.
 * The intent is to signal that there is one less live ball than before.
 */
void device_remove_live (void)
{
	/* If any balls were missing, and now one is rediscovered in the
	 * trough, then just hold onto it.  This condition was seen when
	 * the game thought 1 ball was in play, but 2 were on the playfield. */
	if (missing_balls > live_balls)
	{
		callset_invoke (missing_ball_found);
		missing_balls--;
	}
	else if (live_balls > 0)
	{
		/* Decrement the count of balls in play.  Now what? */
		live_balls--;
		if (in_game)
		{
			/* Notify that the ball count changed, and that a ball drained */
			callset_invoke (ball_count_change);
			callset_invoke (ball_drain);
			switch (live_balls)
			{
				case 0:
					/* With zero balls in play, this might be end of ball. */
					end_ball ();

					/* FALLTHRU : end_ball may be cancelled due to a
					ball save, but must be treated as going back to
					single_ball_play */

				case 1:
					/* Multiball modules like to know when single ball play resumes. */
					callset_invoke (single_ball_play);
					break;
				default:
					break;
			}
		}
	}
}


/** Sets the desired number of balls to be in play. */
#ifdef DEVNO_TROUGH
void device_multiball_set (U8 count)
{
	device_t *dev = device_entry (DEVNO_TROUGH);
	U8 kicks = count - (live_balls + dev->kicks_needed);
	while (kicks > 0)
	{
#ifdef MACHINE_LAUNCH_SOLENOID
		device_request_kick (dev);
#else
#ifdef MACHINE_TZ
		extern void autofire_add_ball (void);
		autofire_add_ball ();
#endif
#endif
		kicks--;
	}
}
#endif


void locating_balls_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 11, "LOCATING BALLS");
	font_render_string_center (&font_fixed6, 64, 22, "PLEASE WAIT...");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


/** Called at game start time to see if it is OK to
 * start a game.  This routine should check that all
 * balls are accounted for, and at least 1 ball is
 * in the trough. */
bool device_check_start_ok (void)
{
	/* Reset any kickout locks, just in case */
	kickout_locks = 0;

	/* If any balls are missing, don't allow the game to start
	 * without first trying a device probe. */
 	if (task_find_gid (GID_DEVICE_PROBE)) 
		return FALSE;
	else if (switch_poll_logical (MACHINE_SHOOTER_SWITCH))
		return TRUE;
	else if (missing_balls > 0)
	{
		task_recreate_gid (GID_DEVICE_PROBE, device_probe);
		/* TODO : a ball search here is probably needed also. */
		return FALSE;
	}
	/* OK to start game */
	return TRUE;
}

void device_unlock_ball (device_t *dev)
{
	if (dev->max_count > 0)
	{
		dbprintf ("Unlock ball in devno %d\n", dev->devno);
		device_disable_lock (dev);
		live_balls++;
		device_request_kick (dev);
	}
	else
		fatal (ERR_UNLOCK_EMPTY_DEVICE);
}


void device_lock_ball (device_t *dev)
{
#ifdef DEVNO_TROUGH
	device_t *trough = device_entry (DEVNO_TROUGH);
#endif

	if (dev->max_count >= dev->size)
		fatal (ERR_LOCK_FULL_DEVICE);

	dbprintf ("Lock ball in devno %d\n", dev->devno);
	device_enable_lock (dev);
	live_balls--;
#ifdef DEVNO_TROUGH
	if (trough->actual_count > 0)
	{
		device_request_kick (trough);
	}
	else
#endif
	{
		if (!callset_invoke_boolean (empty_trough_kick))
			device_unlock_ball (dev);
	}
}

CALLSET_ENTRY (device, start_game)
{
	live_balls = 0;
	kickout_locks = 0;
}


void device_init (void)
{
	device_t *dev;
	U8 i;

	device_ss_state = 0;
	max_balls = MACHINE_TROUGH_SIZE;
	counted_balls = MACHINE_TROUGH_SIZE;
	missing_balls = 0;
	live_balls = 0;
	kickout_locks = 0;

	device_count = 0;
	for (i=0; i < NUM_DEVICES; i++)
	{
		dev = device_entry (i);
		device_clear (dev);
		device_register (i, &device_properties_table[i]);
		device_call_op (dev, power_up);
	}
}


