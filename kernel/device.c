
#include <freewpc.h>


device_t device_table[MAX_DEVICES];

static uint8_t device_ss_state;
static uint8_t device_count;
static uint8_t max_balls;
static uint8_t counted_balls;
static uint8_t missing_balls;
static uint8_t live_balls;


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
		db_puts (", desire ");
		db_puti (dev->desired_count); 
		db_puts (", state ");
		db_puts ((dev->state == DEV_STATE_IDLE) ? "IDLE" : "RELEASING");
		db_putc ('\n');
	}

	db_puts ("Counted ");
	db_puti (counted_balls);
	db_putc (' ');
	
	db_puts ("Missing ");
	db_puti (missing_balls);
	db_putc (' ');

	db_puts ("Live ");
	db_puti (live_balls);
	db_putc ('\n');
}

void device_clear (device_t *dev)
{
	dev->size = 0;
	dev->actual_count = 0;
	dev->desired_count = 0;
	dev->state = DEV_STATE_IDLE;
	dev->props = NULL;
}

void device_register (devicenum_t devno, device_properties_t *props)
{
	device_t *dev = &device_table[devno];
	dev->devno = devno;
	dev->props = props;
	dev->size = props->sw_count;
	device_count++;

	db_puts ("Registering device, total count is now ");
	db_puti (device_count);
	db_putc ('\n');
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


void device_update (void)
{
	uint8_t devno;
	device_t *dev;

	db_puts ("In device update\n");
	devno = task_getgid () - DEVICE_GID_BASE;
	dev = &device_table[devno];

wait_and_recount:
	task_sleep_sec (1);
	device_recount (dev);
	device_update_globals ();
	device_debug ();

	db_puts ("Updating device ");
	db_puts (dev->props->name);
	db_putc ('\n');
	db_puti (dev->previous_count);
	db_putc (' ');
	db_puti (dev->actual_count);
	db_putc (' ');
	db_puti (dev->desired_count);
	db_putc ('\n');

	/*****************************************
	 * Handle counts that are different from
	 * what the system wants
	 *****************************************/
	if (dev->actual_count < dev->desired_count)
	{
		/* Container has fewer balls in it than we
		 * would like */
	}
	else if (dev->actual_count > dev->desired_count)
	{
		/* Container has more balls than desired, so
		 * we need to kick one.
		 */
		if (dev->state == DEV_STATE_IDLE)
		{
			/* Device is idle, so kick is allowed */
			dev->state = DEV_STATE_RELEASING;
			device_call_op (dev, kick_attempt);
	
			sol_on (dev->props->sol);
			task_sleep (TIME_100MS);
			sol_off (dev->props->sol);
			goto wait_and_recount;
		}
		else
		{
			/* Device is already kicking, so don't do
			 * anything; we'll come around eventually
			 * and do this later. */
		}
	}
	else
	{
		/* Container has exactly how many balls we
		 * want it to */
		if (dev->state == DEV_STATE_RELEASING)
		{
			/* Kick operation must have succeeded */
			device_call_op (dev, kick_success);
		}
	}

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
	else if (dev->state == DEV_STATE_RELEASING)
	{
		/* Device is in the middle of a release cycle */
		if (dev->actual_count == dev->previous_count)
		{
			/* After attempting a release, the count did not
			 * change ... the kick must have failed, and we
			 * should retry */
		}
		else if (dev->actual_count < dev->previous_count)
		{
			/* The count decreased as expected.  Hopefully by 1... */
			if (dev->actual_count == dev->previous_count - 1)
			{
				/* Well done */
				db_puts ("Kick succeeded\n");
				dev->state = DEV_STATE_IDLE;
			}
			else
			{
				/* More than one ball was released */
				db_puts ("Kick succeeded, but an extra ball came out\n");
			}
		}
		else if (dev->actual_count > dev->previous_count)
		{
			/* The count went up during a kick cycle ... */
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
	if (dev->actual_count == 0)
		device_call_op (dev, empty);
	else if (dev->actual_count == dev->size)
		device_call_op (dev, full);

#if 0
	while (dev->actual_count != dev->desired_count)
	{
		if (dev->actual_count > dev->desired_count)
		{
			/* Count increased, need to kick a ball */
			db_puts ("Kicking a ball\n");

			device_call_op (dev, kick_attempt);
			sol_on (dev->props->sol);
			task_sleep (TIME_100MS);
			sol_off (dev->props->sol);
			
			task_sleep (TIME_1S);

			device_recount (dev);
			if (dev->actual_count == dev->desired_count)
			{
				db_puts ("Kick succeeded\n");
				device_call_op (dev, kick_success);
				goto done;
			}
			else
			{
				db_puts ("Kick failed\n");
				device_call_op (dev, kick_failure);
				break;
			}
		}
		else
		{
			/* Count decreased, enable a lock or kick successful
			 * or ball magically disappeared from device */
		}
	}
#endif

	device_debug ();
	task_exit ();
}


/* Request that a device eject 1 ball */
void device_request_kick (device_t *dev)
{
	if (dev->desired_count > 0)
	{
		task_gid_t gid = DEVICE_GID (device_devno (dev));
		
		db_puts ("Request to kick from ");
		db_puts (dev->props->name);
		db_putc ('\n');

		dev->desired_count--;

		task_create_gid1 (gid, device_update);
	}
	else
	{
	}
}


void device_update_globals (void)
{
	devicenum_t devno;
	uint8_t previous_counted_balls = counted_balls;

	counted_balls = 0;
	for (devno = 0; devno < device_count; devno++)
	{
		device_t *dev = device_entry (devno);
		counted_balls += dev->actual_count;
	}

	if (counted_balls == previous_counted_balls)
	{
		/* No change in total count, so nothing to update */
	}
	else if (counted_balls > previous_counted_balls)
	{
		/* Counted went up */
		uint8_t add_count = counted_balls - previous_counted_balls;
		while (add_count > 0)
		{
			missing_balls--;
			add_count--;
		}
	}
	else if (counted_balls < previous_counted_balls)
	{
		/* Counted went down */
		uint8_t down_count = previous_counted_balls - counted_balls;
		while (down_count > 0)
		{
			missing_balls++;
			down_count--;
		}
	}

	/* missing_balls = max_balls - counted_balls - live_balls; */
}


void device_probe (void)
{
	devicenum_t devno;

	task_sleep_sec (1);

	for (devno = 0; devno < device_count; devno++)
	{
		device_t *dev = device_entry (devno);
		device_recount (dev);
		dev->desired_count = dev->previous_count = dev->actual_count;
	}

	device_update_globals ();

	device_ss_state = 1;

	device_debug ();
	task_exit ();
}


/* Called from a switch handler to do the common processing */
void device_sw_handler (uint8_t devno)
{
	device_t *dev = device_entry (devno);

	db_puts ("Device switch handler for ");
	db_puts (dev->props->name);
	db_putc ('\n');

	if (device_ss_state == 0)
	{
		db_puts ("Aborting because haven't probed yet\n");
		return;
	}

	if (task_find_gid (DEVICE_GID (devno)))
	{
		db_puts ("Aborting because update already running\n");
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
		missing_balls++;
		if (live_balls == 0)
		{
			end_ball ();
		}
	}
}


void device_init (void)
{
	device_t *dev;

	device_ss_state = 0;
	max_balls = 3;
	counted_balls = 3;
	missing_balls = 0;
	live_balls = 0;

	device_count = 0;
	for (dev=device_entry(0); dev < device_entry(MAX_DEVICES); dev++)
		device_clear (dev);
}


