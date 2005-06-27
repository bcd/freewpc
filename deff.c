
#include <freewpc.h>

extern void coin_deff (void) __taskentry__;
extern void test_menu_deff (void) __taskentry__;
extern void rtc_print_deff (void) __taskentry__;

static const deff_t deff_table[] = {
	[DEFF_NULL] = { D_NORMAL, 0, NULL },
	[DEFF_COIN_INSERT] = { D_NORMAL, 10, coin_deff },
	[DEFF_TEST_MENU] = { D_RUNNING, 1, test_menu_deff },
	[DEFF_PRINT_RTC] = { D_NORMAL, 250, rtc_print_deff },
};


/** Indicates the id of the deff currently active */
static uint8_t deff_active;

/** Indicates the priority of the deff currently running */
static uint8_t deff_prio;

/** Queue of all deffs that have been scheduled to run, even
 * if they are low in priority.  The actively running deff
 * is also in this queue. */
static uint8_t deff_queue[MAX_QUEUED_DEFFS];


static void deff_add_queue (deffnum_t dn)
{
	uint8_t i;
	for (i=0; i < MAX_QUEUED_DEFFS; i++)
		if (deff_queue[i] == 0)
		{
			deff_queue[i] = dn;
			return;
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
		if (deff_queue[i] != 0)
		{
			const deff_t *deff = &deff_table[deff_queue[i]];
			if (deff->prio > prio)
			{
				prio = deff->prio;
				best = deff_queue[i];
			}
		}
		else
			break;

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
			task_recreate_gid (GID_DEFF, deff->fn, 0);
			deff_active = dn;
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
			task_recreate_gid (GID_DEFF, deff->fn, 0);
			deff_active = dn;
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
}


void deff_restart (deffnum_t dn)
{
	const deff_t *deff = &deff_table[dn];
	if ((deff->flags & D_RUNNING) && (dn == deff_active))
	{
		task_recreate_gid (GID_DEFF, deff->fn, 0);
	}
	else
	{
		deff_start (dn);
	}
}


void deff_exit (void) __noreturn__
{
	deffnum_t dn = deff_get_highest_priority ();
	const deff_t *deff = &deff_table[dn];
	task_setgid (GID_DEFF_EXITING);
	task_recreate_gid (GID_DEFF, deff->fn, 0);
	task_exit ();
}


void deff_init (void)
{
	deff_prio = 0;
	memset (deff_queue, 0, MAX_QUEUED_DEFFS);
}


