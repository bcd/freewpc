
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
 * Examples include ejects (single ball), poppers, scoops, etc.
 *
 * Devices are managed by the kernel for the most part.  Each
 * implementation of a device declares "operation" routines that act
 * as callbacks and get called at the appropriate times for device-
 * specific behavior.
 */

#include <freewpc.h>

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

U8 live_balls;


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
	dev->state = DEV_STATE_IDLE;
	dev->props = NULL;
}

void device_register (devicenum_t devno, device_properties_t *props)
{
	device_t *dev = &device_table[devno];
	dev->devno = devno;
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
		int level = switch_poll_logical (sw);
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
			 */
			db_puts ("Idle but ball lost\n");
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
			 * should retry */

			/* TODO : infinite retries are being done now.
			 * At some point, we must give up... */
			db_puts ("Kick did not change anything\n");
			device_call_op (dev, kick_failure);
		}
		else if (dev->actual_count < dev->previous_count)
		{
			/* The count decreased as expected.  Hopefully by 1... */
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
			}
		}
		else if (dev->actual_count > dev->previous_count)
		{
			/* The count went up during a kick cycle.
			 * kicks_needed is presumably still nonzero, so
			 * the code below should attempt the kick again. */
			db_puts ("After kick, count increased\n");
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
		else 
		{
			/* Container has balls ready to kick */
			db_puts ("About to call kick_attempt\n");

			/* Mark state as releasing if still idle */
			if (dev->state == DEV_STATE_IDLE)
				dev->state = DEV_STATE_RELEASING;

			device_call_op (dev, kick_attempt);
		
			sol_pulse (dev->props->sol);
			goto wait_and_recount;
		}
	}

	device_debug ();
	task_exit ();
}


/** Request that a device eject 1 ball */
void device_request_kick (device_t *dev)
{
	dbprintf ("Request to kick from %s\n", dev->props->name);
	
	if ((dev->actual_count - dev->kicks_needed) > 0)
	{
		task_gid_t gid = DEVICE_GID (device_devno (dev));
		
		dev->kicks_needed++;
		dbprintf ("Creating task with gid=%02x\n", gid);
		task_create_gid1 (gid, device_update);
	}
	else
	{
		dbprintf ("No balls to kick!\n");
	}
}


/** Request that a device ejects all balls */
void device_request_empty (device_t *dev)
{
	if ((dev->actual_count - dev->kicks_needed) > 0)
	{
		task_gid_t gid = DEVICE_GID (device_devno (dev));

		dev->kicks_needed += dev->actual_count;
		task_create_gid1 (gid, device_update);
	}
}


/** Update the global state of the machine.  This happens nearly
 * anytime a change happens locally within a particular device. */
void device_update_globals (void)
{
	devicenum_t devno;

	/* Recount the number of balls that are held */
	counted_balls = 0;
	for (devno = 0; devno < device_count; devno++)
	{
		device_t *dev = device_entry (devno);
		counted_balls += dev->actual_count;
	}

	/* Count how many balls are missing */
	missing_balls = max_balls - counted_balls;
	if (missing_balls == live_balls)
	{
		/* Number of balls not accounted for is what we expect */
	}
	else
	{
		/* TODO : Number of balls not accounted for is NOT what we expect */
		db_puts ("Missing != Live\n");
	}
}


void device_probe (void)
{
	devicenum_t devno;

	task_sleep_sec (1);

	for (devno = 0; devno < device_count; devno++)
	{
		device_t *dev = device_entry (devno);
		device_recount (dev);
		dev->previous_count = dev->actual_count;
	}

	device_update_globals ();

	device_ss_state = 1;
	sys_init_pending_tasks--;

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
	}
}


void device_remove_live (void)
{
	if (live_balls > 0)
	{
		live_balls--;
		if ((live_balls == 0) && in_game)
		{
			end_ball ();
		}
	}
}


/** Sets the desired number of balls to be in play. */
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


void device_init (void)
{
	device_t *dev;

	device_ss_state = 0;
	max_balls = MACHINE_TROUGH_SIZE;
	counted_balls = MACHINE_TROUGH_SIZE;
	missing_balls = 0;
	live_balls = 0;

	device_count = 0;
	for (dev=device_entry(0); dev < device_entry(NUM_DEVICES); dev++)
	{
		device_clear (dev);
		device_call_op (dev, power_up);
	}
}


