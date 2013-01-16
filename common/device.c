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
 * for servicing requests from the rest of the system.  This module
 * exposes several per-device events that can be caught by the game
 * code, for example, when a ball enters a device or a kickout is
 * successful.
 *
 * The ball device code is also responsible for determining when
 * end-of-ball occurs, and keeps track of the total number of balls
 * in play at all times.
 */

#include <freewpc.h>
#include <search.h>
#include <diag.h>

/** Pointer to the device properties table generated from the md */
extern device_properties_t device_properties_table[];

extern U8 switch_stress_enable;

/** Runtime status about each device */
device_t device_table[NUM_DEVICES];

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

/** The set of kickout locks currently held.
 * Normally this is zero, and kickouts occur as soon as possible.
 * When nonzero, kickouts are delayed, e.g. to allow an effect to
 * run.  The lock is then released and things continue. */
/* TODO - no way to lock only a particular device, which could be
useful */
U8 kickout_locks;

/** The number of times in a row that game start was tried,
but failed due to missing balls.  After so many errors, the
game is allowed to start anyway. */
U8 device_game_start_errors;


#ifdef DEBUGGER

void device_debug (device_t *dev)
{
	dbprintf ("%d) %s: %d -> %d, ",
		dev->devno, dev->props->name,
		dev->previous_count, dev->actual_count);

	dbprintf ("max %d, need %d kicks, %d errors, %s\n",
		dev->max_count, dev->kicks_needed, dev->kick_errors,
		(dev->state == DEV_STATE_IDLE) ? "idle" : "releasing");
}

void device_debug_all (void)
{
	devicenum_t devno;

	for (devno = 0; devno < NUM_DEVICES; devno++)
	{
		device_t *dev = &device_table[devno];
		device_debug (dev);
	}
	task_runs_long ();
	dbprintf ("Found: %d   ", counted_balls);
	dbprintf ("Lost: %d   ", missing_balls - live_balls + held_balls);
	dbprintf ("Live: %d   ", live_balls);
	dbprintf ("Held: %d\n", held_balls);
}
#else
#define device_debug(dev)
#define device_debug_all()
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
	dev->virtual_count = 0;
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
	dev->devno_mask = single_bit_set (devno);
	dev->props = props;
	dev->size = props->sw_count;
	dev->max_count = props->init_max_count;
}

/* Return the number of balls currently present in the device */
U8 device_recount (device_t *dev)
{
	U8 i;
	U8 count = 0;

	/* Everytime a recount occurs, we remember the previous
	value that was counted.  By comparing these two, we can
	tell if something changed. */
	dev->previous_count = dev->actual_count;

	for (i=0; i < dev->size; i++)
	{
		switchnum_t sw = dev->props->sw[i];
		U8 level = switch_poll_logical (sw);
		if (level)
			count++;
	}

	/* Each device keeps a 'virtual count' of balls that it knows
	are in the device but which are not seen by any switches.
	The core system cannot determine what this count should be, but
	just includes it in the overall count.  See APIs for below for
	game code to update the virtual count. */
	count += dev->virtual_count;

	dev->actual_count = count;
	return (count);
}


/** Gets the current task's GID and makes sure (optionally) that it is
 * in the range of valid GIDs for devices. */
static inline U8 device_getgid (void)
{
	U8 gid = task_getgid ();
#ifdef PARANOID
	if ((gid < DEVICE_GID_BASE) || (gid >= DEVICE_GID_BASE + NUM_DEVICES))
	{
		dbprintf ("Bad GID %02X for device\n", gid);
		abort ();
	}
#endif
	return gid;
}


/** Returns TRUE if any devices have pending kicks. */
bool device_kicks_pending (void)
{
	devicenum_t devno;

	for (devno = 0; devno < NUM_DEVICES; devno++)
		if (device_table[devno].kicks_needed > 0)
			return TRUE;
	return FALSE;
}


/** The core function for handling a device.
 * This function is invoked (within its own task context) whenever
 * a switch closure occurs on a device, or when a request is made to
 * kick a ball from a device.
 * The update task will get killed and restarted anytime a switch
 * closure occurs on the device, so it may be interrupted, but only
 * while it is sleeping.
 */
void device_update (void)
{
	device_t *dev = &device_table[device_getgid () - DEVICE_GID_BASE];

wait_and_recount:
	/* We are really interested in the total count of the
	 * device, not which switches contributed to it.
	 * Since multiple switch transitions occur as a ball
	 * "slides through", don't act on a transition right
	 * away.  Instead, wait awhile until no further transitions
	 * occur, so that the count is stable.  If another closure on
	 * this device happens while we sleep here, this task will
	 * be killed and restarted.
	 */
	task_sleep (dev->props->settle_delay);

start_update:
	task_sleep (TIME_50MS);

	/* The device is probably stable now.  Poll all of the
	 * switches and recount */
	device_recount (dev);

	device_update_globals ();
	device_debug (dev);

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
			 * Also, when transitioning back to idle after a device kick,
			 * we repoll the switches one extra time and if all is well,
			 * we'll end up here.
			 */
		}
		else if (dev->actual_count < dev->previous_count)
		{
			/* Also unusual in that a ball came out of the device without
			 * explicitly kicking it.  (Although this can happen in test mode.)
			 * Note that the number of balls in play went up */
			if (in_game)
			{
				device_call_op (dev, surprise_release);
				device_add_live ();
			}
		}
		else if (dev->actual_count > dev->previous_count)
		{
			/* More typical : when idle, the count should only go up.
			 * Treat this as an enter event (or multiple events, if the
			 * count goes up by more than 1). */
			U8 enter_count = dev->actual_count - dev->previous_count;
			if (!trough_dev_p (dev))
				set_valid_playfield ();
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
		if (unlikely (dev->actual_count >= dev->previous_count))
		{
			/* After attempting a release, the count did not go down ... the kick
			 * probably failed, and we should retry up to a point.  Since dev->state
			 * is unchanged below, the kick attempt will get reinvoked. */

			/* Note: during multiball, it is possible for a second ball to enter
			the device immediately after the kick.  The kick didn't
			really fail, but there's no way to tell the difference. */

			dev->kick_errors++;
			dbprintf ("Kick error %d\n", dev->kick_errors);
			device_call_op (dev, kick_failure);

			if (dev->kick_errors == (trough_dev_p (dev) ? 20 : 7))
			{
				/* OK, we tried too many times and still no ball came out.
				 * Cancel all kick requests for this device unless it is
				 * the trough */
				nonfatal (ERR_FAILED_KICK);
				dev->kicks_needed = 0;
				dev->state = DEV_STATE_IDLE;
			}
			else
			{
				/* Wait awhile before trying again.  The more we fail, the longer
				the delay in between tries. */
				if (dev->kick_errors <= 3)
					task_sleep_sec (1);
				else
					task_sleep_sec (5);
			}
		}
		else if (dev->actual_count < dev->previous_count)
		{
			/* The count decreased as expected.
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

			/* Go back to idle state.  If there are more kicks left, we will
			switch back to DEV_STATE_RELEASING again later.  The point is not to
			stay in DEV_STATE_RELEASING when we have not actually kicked the ball;
			if the request is held up for some reason, we want switch closures to
			be processed correctly. */
			dev->state = DEV_STATE_IDLE;
		}
	}

	/*****************************************
	 * Handle global count changes
	 *****************************************/
	device_update_globals ();

	/************************************************
	 * Handle counts larger than the device supports
	 ************************************************/
	if (dev->kicks_needed == 0 && dev->actual_count > dev->max_count)
	{
		/* When there are more balls in the device than we normally want
		to keep here, we must kick one of them out.  If multiple kicks
		are needed, this check will occur again in the future. */
		dev->kicks_needed++;
		dev->kick_errors = 0;
		/* TODO - device_request_kick (dev); would be more appropriate,
		 * but that doesn't work when called from device context due
		 * to live balls getting bumped */
	}

	/************************************************
	 * Handle kicking out balls
	 ************************************************/
	if (dev->kicks_needed > 0)
	{
		if (dev->actual_count == 0)
		{
			/* Container has fewer balls in it than we
			 * would like */
			dbprintf ("Can't kick when no balls available!\n");
			dev->kicks_needed = 0;
		}
		else if (kickout_locked_p () && !trough_dev_p (dev))
		{
			/* Container ready to kick, but 1 or more
			 * locks are held so we must wait. */
			goto start_update;
		}
		else if (!device_call_boolean_op (dev, kick_request))
		{
			/* Inform other modules that a kick was requested.
			These handlers can return FALSE to delay (but not
			cancel) the kick. */
			goto start_update;
		}
			/* TODO - if multiple devices want to kick at the same time,
			 * they should be staggered a bit.  Another case should be
			 * added here. */
		else
		{
			/* The container is ready to kick */

			/* Mark state as releasing if still idle */
			if (dev->state == DEV_STATE_IDLE)
				dev->state = DEV_STATE_RELEASING;

			/* TODO - keep track of all pending kick attempts.  Use a
			bit variable per device.  If other devices are in the
			process of kicking, wait */

			/* Generate events that a kick attempt is coming */
			callset_invoke (any_kick_attempt);
			device_call_op (dev, kick_attempt);

			/* Pulse the solenoid. */
			sol_request (dev->props->sol);

			/* In timed games, a device kick will pause the game timer.
			 * TODO : this should be a global event that other modules
			 * can catch as well.  Deal with this like we do slowtimers. */
			timed_game_pause (TIME_1S);

			/* We don't know if the kick was successful or not yet, so wait
			and see what happens. */
			goto wait_and_recount;
		}
	}

	/* Just before exiting this task, poll the switches one more time,
	and see if something has changed during the update.  It is important
	that no task context switches take place here, otherwise there would
	be a race condition where a switch closure gets missed. */
	device_recount (dev);
	if (dev->actual_count != dev->previous_count)
		goto start_update;

	task_exit ();
}


/** Returns the number of balls that can be kicked out of
 * a device.  This is the number of balls in the device,
 * minus any pending kicks. */
static inline U8 device_kickable_count (device_t *dev)
{
	if (switch_stress_enable == YES)
		return 0;
	else
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
		/* TODO - this logic probably belongs somewhere else.
		We're increment live balls very early here, before the
		balls are actually added to play. */
		if (!trough_dev_p (dev))
			live_balls++;
	}
	else if (switch_stress_enable && trough_dev_p (dev))
	{
		missing_balls++;
		device_add_live ();
	}
	else
	{
		dbprintf ("Kick request invalid\n");
	}
	if (gid != task_getgid ())
		task_recreate_gid_while (gid, device_update, TASK_DURATION_INF);
}


/** Request that a device ejects all balls */
void device_request_empty (device_t *dev)
{
	U8 can_kick;

	task_gid_t gid = DEVICE_GID (device_devno (dev));
	task_kill_gid (gid);

	/* See how many balls are in the device, and schedule that many kicks. */
	can_kick = device_kickable_count (dev);
	dev->kicks_needed += can_kick;
	dev->kick_errors = 0;

	/* TODO - this logic probably belongs somewhere else */
	if (!trough_dev_p (dev))
		live_balls += can_kick;

	/* Reset count of locked balls */
	dev->max_count = dev->props->init_max_count;
	if (gid != task_getgid ())
		task_recreate_gid_while (gid, device_update, TASK_DURATION_INF);
}


/** Update the global state of the machine.  This happens nearly
 * anytime a change happens locally within a particular device. */
void device_update_globals (void)
{
	devicenum_t devno;
	U8 held_balls_now = 0;

	/* TODO - mostly this is called from the context of a single device.
	Could simplify all the recounting of other devices... */

	/* Recount the total number of balls that are held,
	excluding those that are locked and those in the trough. */
	counted_balls = 0;
	held_balls_now = 0;
	for (devno = 0; devno < NUM_DEVICES; devno++)
	{
		device_t *dev = device_entry (devno);
		counted_balls += dev->actual_count;

		if (!trough_dev_p (dev))
			if (dev->actual_count > dev->max_count)
				held_balls_now += dev->actual_count - dev->max_count;
	}

	/* Update held_balls atomically */
	held_balls = held_balls_now;

	/* Update count of how many balls are missing */
	missing_balls = max_balls - counted_balls;

	/* If 'missing' went negative, this means there are more
	balls detected than expected. */
	if (missing_balls > 0xF0)
	{
		missing_balls = 0;
	}

	dbprintf ("Counted %d Missing %d Live %d Heldup %d\n",
		counted_balls, missing_balls, live_balls, held_balls);

	/* If any balls are held up temporarily (more than "max" are
	 * in the device presently), then delay timers */
	if (in_live_game)
	{
		if (held_balls > 0)
			timed_game_suspend ();
		else
			timed_game_resume ();
	}
}


/** Returns the number of balls held up temporarily.
 *
 * This is the number of balls that are seen, or about to be
 * seen in ball devices, but which will not be kept here.
 * This count is queried by timers to determine when they
 * should be paused.
 */
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
 * take some time and this function waits for any kicked
 * balls to successfully exit before it returns.
 */
void device_probe (void)
{
	devicenum_t devno;
	U8 kicks;

	dbprintf ("Probing devices\n");

	/* Keep track of the number of times we actually had to kick
	a device; equivalently, this is the number of times we sleep
	waiting for the kick to have an effect.  After so long, if
	things aren't right, just give up. */
	kicks = 0;

probe_from_beginning:
	if (kicks >= 10)
		goto probe_exit;

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
		 * schedule the extras to be emptied.   Then rescan from
		 * the beginning again. */
		dev->max_count = dev->props->init_max_count;
		if (dev->actual_count > dev->max_count)
		{
			kicks++;
			device_request_kick (dev);
			task_sleep_sec (2);
			goto probe_from_beginning;
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

probe_exit:
	/* At this point, all kicks have been made, but balls may be
	on the playfield heading for the trough.  We still should wait
	until 'missing_balls' goes (hopefully) to zero.
	We'll wait for up to 10 seconds, but exit sooner if we find all
	balls before then. */
	task_sleep_sec (4);
	if (missing_balls != 0)
	{
		task_sleep_sec (2);
		if (missing_balls != 0)
		{
			task_sleep_sec (2);
			if (missing_balls != 0)
			{
				task_sleep_sec (2);
			}
		}
	}

	dbprintf ("Checking globals after probe\n");
	device_update_globals ();

	dbprintf ("\nDevices initialized.\n");
	device_debug_all ();
	task_exit ();
}


/** Called from a switch handler to do the common processing.
 * The input is the device number.  The actual switch that
 * transitioned is unknown, as we don't really care. */
void device_sw_handler (U8 devno)
{
	/* Ignore device switches until initialization is complete */
	if (!sys_init_complete)
	{
		dbprintf ("Device switch ignored during init.\n");
		return;
	}

	/* Ignore device switches if the device update task is already running.
	 * It is polling the device count and will deal with this event. */
	if (task_find_gid (DEVICE_GID (devno)))
		return;

	timer_kill_gid (GID_DEVICE_SWITCH_WILL_FOLLOW);
	task_recreate_gid_while (DEVICE_GID (devno), device_update, TASK_DURATION_INF);
}


/** Called when the number of 'live balls' in play should be increased. */
void device_add_live (void)
{
	if (missing_balls > 0)
	{
		missing_balls--;
		live_balls++;
		if (in_game && !in_bonus)
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
		if (in_game && !in_bonus)
		{
			/* Notify that the ball count changed */
			callset_invoke (ball_count_change);

			/* See if this qualifies as a ball drain.  Any event receiver
			can return FALSE here if it is not to be treated as a drain;
			e.g., when a ballsaver is active.  In these cases, the
			event function is also responsible for putting the ball
			back into play. */
			if (!callset_invoke_boolean (ball_drain))
				return;

			/* OK, at this point, it is a true ball drain event.
			See how many balls are in play now. */
			switch (live_balls
#ifdef DEVNO_TROUGH
				 + device_entry (DEVNO_TROUGH)->kicks_needed
#endif
				)
			{
				case 0:
					/* With zero balls in play, this is end of ball.
					This function usually does not return; it will stop just about
					every task running to reset for the next ball. */
					end_ball ();
					return;

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


/** Add a virtual ball to the device, not seen by any switches.
 * For example, TZ's gumball machine does not have individual switches
 * counting how many balls are inside it.  The machine code has other
 * means of knowing when a ball enters the device.
 */
void device_add_virtual (device_t *dev)
{
	dev->virtual_count++;

	/* After updating the virtual count, always invoke the
	device switch handler, as if some counting switch changed.
	This will take care of throwing the 'enter' event and updating
	the number of live balls, serving another ball if it is locked
	here, etc. */
	device_sw_handler (dev->devno);
}


/** Remove a virtual ball from the device, not seen by any switches. */
void device_remove_virtual (device_t *dev)
{
	if (dev->virtual_count > 0)
	{
		/* Update counts.  We avoid calling device_sw_handler here
		because there is no possibility for any error or need for retry.
		The change is instantaneous.  The device state remains idle. */
		dev->virtual_count--;
		dev->actual_count--;
		dev->previous_count--;

		/* Throw the usual events on releases */
		device_update_globals ();
		device_call_op (dev, kick_success);
	}
	else
	{
		nonfatal (ERR_VIRTUAL_DEVICE_COUNT);
	}
}


/** Called at game start time to see if it is OK to
 * start a game.  This routine should check that all
 * balls are accounted for, and at least 1 ball is
 * in the trough. */
bool device_check_start_ok (void)
{
	U8 truly_missing_balls;

	/* Reset any kickout locks, just in case */
	kickout_unlock_all ();

	/* If any balls are missing, don't allow the game to start
	 * without first trying a device probe.
	 *
	 * If the device probe is already in progress, then just
	 * return right away. */
	if (task_find_gid (GID_DEVICE_PROBE))
		return FALSE;

	truly_missing_balls = missing_balls;
#ifdef MACHINE_SHOOTER_SWITCH
	if (switch_poll_logical (MACHINE_SHOOTER_SWITCH))
		truly_missing_balls--;
#endif

	/* If some balls are unaccounted for, and not on the shooter,
	 * then start a device probe and a ball search.  Alert the user
	 * by displaying a message.
	 *
	 * After 3 probes, allow the game to start anyway.  However,
	 * if no balls are accounted for anywhere, then don't do that.
	 */
	if (truly_missing_balls > 0)
	{
		dbprintf ("%d balls missing.\n", truly_missing_balls);
		if ((++device_game_start_errors <= 3) || (counted_balls == 0))
		{
			task_recreate_gid (GID_DEVICE_PROBE, device_probe);
			ball_search_now ();
			deff_start (DEFF_LOCATING_BALLS);
			return FALSE;
		}
		else
			return TRUE;
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
		device_request_kick (dev);
	}
	else
		nonfatal (ERR_UNLOCK_EMPTY_DEVICE);
}


/** Requests that a device allow a ball lock. */
void device_lock_ball (device_t *dev)
{
#ifdef DEVNO_TROUGH
	device_t *trough = device_entry (DEVNO_TROUGH);
#endif

	/* If the device is already locking as many balls
	as it can hold, then trying to lock another ball here
	is an error. */
	if (dev->max_count >= dev->size)
		fatal (ERR_LOCK_FULL_DEVICE);

	dbprintf ("Lock ball in devno %d\n", dev->devno);

	/* Update count of balls that will be held here. */
	device_enable_lock (dev);

	/* Say that there is one less active ball on the
	playfield now, assuming a ball is still physically present */
	if (device_kickable_count (dev))
		live_balls--;

	/* If the trough is not empty, we can serve another ball from the
	trough to continue play.  Otherwise, it will have to come from
	somewhere else.  The default is to serve it from the same device,
	but this can be overriden by the 'empty_trough_kick' event. */
#ifdef DEVNO_TROUGH
	if (trough->actual_count > 0)
	{
		serve_ball ();
	}
	else
#endif
	{
		if (!callset_invoke_boolean (empty_trough_kick))
			device_unlock_ball (dev);
	}
}

CALLSET_ENTRY (device, start_ball)
{
	live_balls = 0;
	kickout_unlock_all ();
}

/* TODO - at the start of each ball, see how many balls in the trough
constitutes a drain.  Balls could be physically missing, or they could
be held up in some other playfield device that didn't empty after the
last game, or they could be in play already if the game was restarted. */

CALLSET_ENTRY (device, amode_start)
{
	live_balls = 0;
	device_game_start_errors = 0;
}


CALLSET_ENTRY (device, diagnostic_check)
{
	device_update_globals ();
	if (missing_balls != 0)
		diag_post_error ("PINBALL MISSING\n", COMMON_PAGE);
}


CALLSET_ENTRY (device, init_complete)
{
	task_recreate_gid (GID_DEVICE_PROBE, device_probe);
	task_sleep (TIME_500MS);
}


/** Initialize the device subsystem */
void device_init (void)
{
	device_t *dev;
	U8 i;

	max_balls = MACHINE_MAX_BALLS;
	counted_balls = MACHINE_TROUGH_SIZE;
	missing_balls = 0;
	live_balls = 0;
	kickout_unlock_all ();
	held_balls = 0;
	device_game_start_errors = 0;

	for (i=0; i < NUM_DEVICES; i++)
	{
		dev = device_entry (i);
		device_clear (dev);
		device_register (i, &device_properties_table[i]);
	}
}

