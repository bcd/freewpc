
#include <freewpc.h>

extern void coin_deff (void) __taskentry__;
extern void test_menu_deff (void) __taskentry__;
extern void rtc_print_deff (void) __taskentry__;
extern void credit_added_deff (void) __taskentry__;

static const deff_t deff_table[] = {
	[DEFF_NULL] = { D_NORMAL, 0, NULL },
	[DEFF_TEST_MENU] = { D_RUNNING, 1, test_menu_deff },
	[DEFF_COIN_INSERT] = { D_NORMAL, 10, coin_deff },
	[DEFF_CREDIT_ADDED] = { D_NORMAL, 100, credit_added_deff },
	[DEFF_PRINT_RTC] = { D_NORMAL, 250, rtc_print_deff },
};


/** Indicates the id of the deff currently active */
static uint8_t deff_active;

/** Indicates the priority of the deff currently running */
uint8_t deff_prio;

/** Queue of all deffs that have been scheduled to run, even
 * if they are low in priority.  The actively running deff
 * is also in this queue. */
static uint8_t deff_queue[MAX_QUEUED_DEFFS];


static void deff_add_queue (deffnum_t dn)
{
	uint8_t i;
	for (i=0; i < MAX_QUEUED_DEFFS; i++)
	{
		if (deff_queue[i] == dn)
			return;
		else if (deff_queue[i] == 0)
		{
			deff_queue[i] = dn;
			return;
		}
	}
	fatal (ERR_DEFF_QUEUE_FULL);
}


static void deff_remove_queue (deffnum_t dn)
{
	uint8_t i;
	for (i=0; i < MAX_QUEUED_DEFFS; i++)
		if (deff_queue[i] == dn)
			deff_queue[i] = 0;
}


static deffnum_t deff_get_highest_priority (void)
{
	uint8_t i;
	uint8_t prio = 0;
	uint8_t best = 0;

	for (i=0; i < MAX_QUEUED_DEFFS; i++)
	{
		if (deff_queue[i] != 0)
		{
			const deff_t *deff = &deff_table[deff_queue[i]];
			if (deff->prio > prio)
			{
				prio = deff->prio;
				best = deff_queue[i];
			}
		}
	}

	/* Save the priority of the best deff here */
	deff_prio = prio;

	/* Return the deffnum of the best deff to the caller */
	return best;
}


void deff_start (deffnum_t dn)
{
	const deff_t *deff = &deff_table[dn];

	if (deff->flags & D_RUNNING)
	{
		deff_add_queue (dn);
		if (dn == deff_get_highest_priority ())
		{
			/* This is the new active running deff */
			deff_active = dn;
			task_recreate_gid (GID_DEFF, deff->fn);
		}
		else
		{
			/* This deff cannot run now, because there is a
			 * higher priority deff running. */
		}
	}
	else
	{
		if (deff->prio > deff_prio)
		{
			deff_active = dn;
			task_recreate_gid (GID_DEFF, deff->fn);
		}
	}
}


void deff_stop (deffnum_t dn)
{
	const deff_t *deff = &deff_table[dn];
	if (deff->flags & D_RUNNING)
	{
		/* If this deff is active, then stop it */
		deff_remove_queue (dn);
	}
	fatal (ERR_NOT_IMPLEMENTED_YET);
}


void deff_restart (deffnum_t dn)
{
	const deff_t *deff = &deff_table[dn];
	if ((deff->flags & D_RUNNING) && (dn == deff_active))
		task_kill_gid (GID_DEFF);
	deff_start (dn);
}


void deff_exit (void) __noreturn__
{
	const deff_t *deff;

	deff_remove_queue (deff_active);

	deff_active = deff_get_highest_priority ();
	if (deff_active != 0)
	{
		deff = &deff_table[deff_active];
		task_setgid (GID_DEFF_EXITING);
		task_recreate_gid (GID_DEFF, deff->fn);
	}
	task_exit ();
}


void deff_delay_and_exit (task_ticks_t ticks)
{
	task_sleep (ticks);
	deff_exit ();
}


void deff_swap_low_high (int8_t count, task_ticks_t delay)
{
	while (--count >= 0)
	{
		dmd_show_other ();
		task_sleep (delay);
	}
}


void deff_init (void)
{
	deff_prio = 0;
	memset (deff_queue, 0, MAX_QUEUED_DEFFS);
}


