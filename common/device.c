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
/* TODO : is this needed?  Can use sys_init_complete instead? */
U8 device_ss_state;

/** The maximum number of balls that this machine should have installed
in the standard devices */
U8 max_balls;

/** The number of balls accounted for, the last time a full recount was
done. */
U8 counted_balls;

/** The number of balls that have gone missing.  This is always the
difference between max_balls and counted_balls.  Note that missing does
not really mean missing... it means that the balls are not held up.
They could be legitimately in play, or truly missing. */
U8 missing_balls;

/** The number of unaccounted balls that are assumed to be on
 * the table.  This includes balls in the plunger lane.  Normally,
 * missing == live.  If missing > live, then something is wrong. */
U8 live_balls;

/** The number of balls that are held in a container, other than the trough,
 * excluding locked balls.  These are balls "in transition" which are
 * pending kickout. */
U8 held_balls;

/** The number of kickout locks currently held.
 * Normally this is zero, and kickouts occur as soon as possible.
 * When nonzero, kickouts are delayed, e.g. to allow an effect to
 * run.  The lock is then released and things continue. */
U8 kickout_locks;


#ifdef DEBUGGER
void device_debug (void)
{
	devicenum_t devno;

	for (devno = 0; devno < NUM_DEVICES; devno++)
	{
		device_t *dev = &device_table[devno];
		
		dbprintf ("%d) %s: %d -> %d, ", 
			devno, dev->props->name,
			dev->previous_count, dev->actual_count);

		dbprintf ("max %d, need %d kicks, %s\n",
			dev->max_count, dev->kicks_needed,
			(dev->state == DEV_STATE_IDLE) ? "idle" : "releasing");
	}

	dbprintf ("Counted %d Missing %d Live %d Heldup %d\n", 
		counted_balls, missing_balls, live_balls, held_balls);
}
#else
#define device_debug()
#endif


/** Clears the state of a device to power on settings. */
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


/** Register a device in the device table.  This initializes the
device state with information from the device properties structure. */
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
}

/* Return the number of balls currently present in the device */
U8 device_recount (device_t *dev)
{
	U8 i;
	U8 count = 0;

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


/** The core function for handling a device.
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
			/* Switch closures were detected but in the end, after becoming
			 * stable, the count did not change.  This is OK, perhaps
			 * there is some vibration...
			 */
		}
		else if (dev->actual_count < dev->previous_count)
		{
			/* Also unusual in that a ball came out of the device without 
			 * explicitly kicking it.  (Although this can happen in test mode.)
			 * Throw a nonfatal if it happens during a game.
			 */
			if (!in_test)
				nonfatal (ERR_IDLE_BALL_LOST);
		}
		else if (dev->actual_count > dev->previous_count)
		{
			/* More typical : when idle, the count should only go up.
			 * Treat this as an enter event (or multiple events, if the
			 * count goes down by more than 1). */
			U8 enter_count = dev->actual_count - dev->previous_count;
			while (enter_count > 0)
			{
				callset_invoke (any_device_enter);
				device_call_op (dev, enter);
				enter_count--;
			}
		}
	}
	else if ((dev->state == DEV_STATE_RELEASING) && (dev->kicks_needed > 0))
	{
		/* Device is in the middle of a release cycle.
		 * See if the count changed. */
		if (dev->actual_count >= dev->previous_count)
		{
			/* After attempting a release, the count did not go down ... the kick 
			 * probably failed, and we should retry up to a point.  Since no state
			 * is changed, the code will get reinvoked. */

			/* Note: during multiball, it is possible for a second ball to enter
			the device immediately after the kick.  The kick didn't
			really fail, but there's no way to tell the difference. */

			dbprintf ("Kick failed\n");
			device_call_op (dev, kick_failure);

			if (++dev->kick_errors == 5)
			{
				/* OK, we tried 5 times and still no ball came out.
				 * Cancel all kick requests for this device. */
				nonfatal (ERR_FAILED_KICK);
				dev->kicks_needed = 0;
				dev->state = DEV_STATE_IDLE;
			}
		}
		else if (dev->actual_count < dev->previous_count)
		{
			/* The count decreased as expected.  Hopefully by the
			 * same number as the number of kicks requested.
			 * As we only kick 1 ball at a time, then really it
			 * only should have gone down by 1, but the logic
			 * should work even if more than 1 ball is ejected. */
			U8 kicked_balls = dev->previous_count - dev->actual_count;

			/* If too many balls were kicked, throw an error.
			Only process as many as were requested. */
			if (kicked_balls > dev->kicks_needed)
			{
				nonfatal (ERR_KICK_TOO_MANY); /* akin to ERR_IDLE_BALL_LOST */
				kicked_balls = dev->kicks_needed;
			}

			/* Throw a kick success event for each ball that was kicked */
			while (kicked_balls > 0)
			{
				device_call_op (dev, kick_success);
				dev->kicks_needed--;
				kicked_balls--;
			}

			/* If no more kicks are required, then go back to idle state. */
			if (dev->kicks_needed == 0)
				dev->state = DEV_STATE_IDLE;
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
			dbprintf ("Can't kick when no balls available!\n");
			dev->kicks_needed = 0;
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
			dbprintf ("About to call kick_attempt\n");

			/* Mark state as releasing if still idle */
			if (dev->state == DEV_STATE_IDLE)
				dev->state = DEV_STATE_RELEASING;

			/* Generate events that a kick attempt is coming */
			callset_invoke (any_kick_attempt);
			device_call_op (dev, kick_attempt);

			/* Pulse the solenoid */
			sol_pulse (dev->props->sol);

			/* In timed games, a device kick will pause the game timer.
			 * TODO : this should be a global event that other modules
			 * can catch as well.  Deal with this like we do slowtimers. */
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
		/* TODO - this logic probably belongs somewhere else.
		We're increment live balls very early here, before the
		balls are actually added to play. */
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
	U8 held_balls_now = 0;

	/* Recount the number of balls that are held, 
	excluding those that are locked. */
	counted_balls = 0;
	held_balls_now = 0;
	for (devno = 0; devno < NUM_DEVICES; devno++)
	{
		device_t *dev = device_entry (devno);
		counted_balls += dev->actual_count;

#ifdef DEVNO_TROUGH
		if (devno != DEVNO_TROUGH)
#endif
			if (dev->actual_count >= dev->max_count)
				held_balls_now += dev->actual_count - dev->max_count;
	}

	/* Update held_balls atomically */
	held_balls = held_balls_now;

	/* Count how many balls are missing */
	missing_balls = max_balls - counted_balls;

	if (missing_balls != live_balls)
	{
		/* Number of balls not accounted for is NOT what we expect */
		dbprintf ("Error: missing=%d, live=%d\n", missing_balls, live_balls);
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


/** Returns the number of balls held up temporarily. */
U8 device_holdup_count (void)
{
	return held_balls + 
		(timer_find_gid (GID_DEVICE_SWITCH_WILL_FOLLOW) ? 1 : 0);
}


/** Probes all devices to see if any balls are present that
 * shouldn't be.  Balls are kicked as necessary.
 *
 * For example, this clears out the balls of a lockup device
 * at the end of a game.
 *
 * This is always done as a background task, because it may
 * take some time, although this seems unnecessary...
 */
void device_probe (void)
{
	devicenum_t devno;

	task_sleep_sec (1);

	for (devno = 0; devno < NUM_DEVICES; devno++)
	{
		device_t *dev = device_entry (devno);

		/* Recount the number of balls in the device, and reset
		 * other device data. */
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
#if 0 /* TODO */
		else if (dev->actual_count < dev->max_count)
		{
			/* The device normally holds more balls than are present in
			it.  If possible, launch a ball here.  (For example, ST:TNG
			or Whodunnit.) */
		}
#endif
	}

	/* Update the global state information again based on the
	 * new counts */
	device_update_globals ();

	device_ss_state = 1;

	device_debug ();
	task_exit ();
}


/** Called from a switch handler to do the common processing */
void device_sw_handler (U8 devno)
{
#ifdef DEBUGGER
	device_t *dev = device_entry (devno);
#endif

	dbprintf ("Device switch handler for %s\n", dev->props->name);

	/* Ignore device switches until device SS is initialized. */
	if (device_ss_state == 0)
		return;

	/* Ignore device switches if the device update task is already running.
	 * It is polling the device count and will deal with this event. */
	if (task_find_gid (DEVICE_GID (devno)))
		return;

	timer_kill_gid (GID_DEVICE_SWITCH_WILL_FOLLOW);
	task_create_gid (DEVICE_GID (devno), device_update);
}


/** Called when the number of 'live balls' in play should be increased. */
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
					single_ball_play.  If the ball really ends, we won't
					come back here. */

				case 1:
					/* Multiball modes like to know when single ball play resumes. */
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

	/* Calculate the number of balls that need to be added to play,
	to reach the desired total count. */
	U8 kicks = count - (live_balls + dev->kicks_needed);

	/* Add that balls into play */
	while (kicks > 0)
	{
		device_request_kick (dev);
		kicks--;
	}
}
#endif


/** Called at game start time to see if it is OK to
 * start a game.  This routine should check that all
 * balls are accounted for, and at least 1 ball is
 * in the trough. */
bool device_check_start_ok (void)
{
	/* Reset any kickout locks, just in case */
	kickout_locks = 0;

	/* If any balls are missing, don't allow the game to start
	 * without first trying a device probe.
	 *
	 * If the device probe is alread in progress, then just
	 * return. */
 	if (task_find_gid (GID_DEVICE_PROBE)) 
		return FALSE;

	/* If a ball is on the shooter switch, then allow the
	 * game to start anyway. */
#ifdef MACHINE_SHOOTER_SWITCH
	else if (switch_poll_logical (MACHINE_SHOOTER_SWITCH))
		return TRUE;
#endif

	/* If some balls are unaccounted for, and not on the shooter,
	 * then start a device probe and a ball search. */
	else if (missing_balls > 0)
	{
		task_recreate_gid (GID_DEVICE_PROBE, device_probe);
		/* TODO : a ball search here is probably needed also. */
		return FALSE;
	}

	/* All checks pass : OK to start game now */
	return TRUE;
}


/** Requests that a device 'unlock' a ball. */
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


/** Requests that a device allow a ball lock. */
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


/** Initialize the device subsystem */
void device_init (void)
{
	device_t *dev;
	U8 i;

	device_ss_state = 0;
#ifdef MACHINE_MAX_BALLS
	max_balls = MACHINE_MAX_BALLS ();
#else
	max_balls = MACHINE_TROUGH_SIZE;
#endif
	counted_balls = MACHINE_TROUGH_SIZE;
	missing_balls = 0;
	live_balls = 0;
	kickout_locks = 0;
	held_balls = 0;

	for (i=0; i < NUM_DEVICES; i++)
	{
		dev = device_entry (i);
		device_clear (dev);
		device_register (i, &device_properties_table[i]);
		device_call_op (dev, power_up);
	}
}

