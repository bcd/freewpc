
#include <freewpc.h>

#define L_NOLAMPS		0x0
#define L_ALL_LAMPS	0xFF
#define L_NOGI			0

#define MACHINE_LAMP_EFFECTS \
	DECL_LEFF (LEFF_AMODE, L_RUNNING, 10, LAMPSET_AMODE_ALL, L_NOGI, amode_leff) \
	DECL_LEFF (LEFF_NO_GI, L_NORMAL, 20, L_NOLAMPS, L_NOGI, no_gi_leff) \
	DECL_LEFF (LEFF_LEFT_RAMP, L_NORMAL, 50, L_NOLAMPS, L_NOGI, left_ramp_leff) \
	DECL_LEFF (LEFF_FLASH_ALL, L_NORMAL, 100, LAMPSET_AMODE_ALL, L_NOGI, flash_all_leff) \
	DECL_LEFF (LEFF_FLASHER_HAPPY, L_NORMAL, 140, L_NOLAMPS, L_NOGI, flasher_happy_leff) \
	DECL_LEFF (LEFF_BONUS, L_RUNNING, 150, L_ALL_LAMPS, L_NOGI, bonus_leff) \
	DECL_LEFF (LEFF_TILT_WARNING, L_RUNNING, 200, L_ALL_LAMPS, L_NOGI, no_lights_leff) \
	DECL_LEFF (LEFF_TILT, L_RUNNING, 205, L_ALL_LAMPS, L_NOGI, no_lights_leff) \


/* Declare externs for all of the deff functions */
#define DECL_LEFF(num, flags, pri, b1, b2, fn) \
	extern void fn (void);

MACHINE_LAMP_EFFECTS


/* Now declare the deff table itself */
#undef DECL_LEFF
#define DECL_LEFF(num, flags, pri, b1, b2, fn) \
	[num] = { flags, pri, b1, b2, fn },

static const leff_t leff_table[] = {
	[LEFF_NULL] = { L_NORMAL, 0, 0, 0, NULL },
	MACHINE_LAMP_EFFECTS
};


/* Declare externs for lamp bit matrices used by leffs */
extern __fastram__ U8 lamp_leff1_allocated[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff1_matrix[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff2_allocated[NUM_LAMP_COLS];
extern __fastram__ U8 lamp_leff2_matrix[NUM_LAMP_COLS];


/** Indicates the id of the leff currently active */
static uint8_t leff_active;

/** Indicates the priority of the leff currently running */
uint8_t leff_prio;

/** Queue of all leffs that have been scheduled to run, even
 * if they are low in priority.  The actively running leff
 * is also in this queue. */
static uint8_t leff_queue[MAX_QUEUED_LEFFS];


uint8_t leff_get_active (void)
{
	return leff_active;
}

static void leff_add_queue (leffnum_t dn)
{
	uint8_t i;

	for (i=0; i < MAX_QUEUED_LEFFS; i++)
		if (leff_queue[i] == dn)
			return;

	for (i=0; i < MAX_QUEUED_LEFFS; i++)
		if (leff_queue[i] == 0)
		{
			leff_queue[i] = dn;
			return;
		}

	fatal (ERR_LEFF_QUEUE_FULL);
}


static void leff_remove_queue (leffnum_t dn)
{
	uint8_t i;
	for (i=0; i < MAX_QUEUED_LEFFS; i++)
		if (leff_queue[i] == dn)
			leff_queue[i] = 0;
}


static leffnum_t leff_get_highest_priority (void)
{
	uint8_t i;
	uint8_t prio = 0;
	uint8_t best = LEFF_NULL;

	for (i=0; i < MAX_QUEUED_LEFFS; i++)
	{
		if (leff_queue[i] != 0)
		{
			const leff_t *leff = &leff_table[leff_queue[i]];
			if (leff->prio > prio)
			{
				prio = leff->prio;
				best = leff_queue[i];
			}
		}
	}

	/* Save the priority of the best leff here */
	leff_prio = prio;

	/* Return the leffnum of the best leff to the caller */
	return best;
}


void leff_create_handler (const leff_t *leff)
{
	/* Allocate lamps needed by the lamp effect */
	if (leff->lampset != 0)
	{
		if (leff->lampset == 0xFF)
		{
			lamp_leff1_allocate_all ();
		}
		else
		{
			lamp_leff1_free_all ();
			lampset_set_apply_delay (0);
			lampset_apply (leff->lampset, lamp_leff_allocate);
		}
		lamp_leff1_erase ();
	}

	/* Allocate general illumination needed by the lamp effect */
	if (leff->gi != 0)
	{
		triac_leff_allocate (leff->gi);
	}

	task_recreate_gid (GID_LEFF, leff->fn);
}


void leff_start (leffnum_t dn)
{
	const leff_t *leff = &leff_table[dn];

	db_puts ("Leff start request for # "); db_puti (dn); db_putc ('\n');

	if (leff->flags & L_RUNNING)
	{
		db_puts ("Adding running leff to queue\n");
		leff_add_queue (dn);
		if (dn == leff_get_highest_priority ())
		{
			/* This is the new active running leff */
			db_puts ("Requested leff is now highest priority\n");
			leff_active = dn;
			leff_create_handler (leff);
		}
		else
		{
			/* This leff cannot run now, because there is a
			 * higher priority leff running. */
			db_puts ("Can't run because higher priority active\n");
		}
	}
	else
	{
		if (leff->prio > leff_prio)
		{
			db_puts ("Restarting quick leff with high pri\n");
			leff_active = dn;
			leff_create_handler (leff);
		}
		else
		{
			db_puts ("Quick leff lacks pri to run\n");
		}
	}
}


void leff_stop (leffnum_t dn)
{
	const leff_t *leff = &leff_table[dn];

	db_puts ("Stopping leff # "); db_puti (dn); db_putc ('\n');

	if (leff->flags & L_RUNNING)
	{
		db_puts ("Remove running leff from queue\n");
		leff_remove_queue (dn);
		lamp_leff1_free_all ();
		triac_leff_free (TRIAC_GI_MASK);

		leff_start_highest_priority ();
	}
	else
	{
		fatal (ERR_NOT_IMPLEMENTED_YET);
	}
}


void leff_restart (leffnum_t dn)
{
	if (dn == leff_active)
	{
		const leff_t *leff = &leff_table[dn];
		leff_create_handler (leff);
	}
	else
	{
		leff_start (dn);
	}
}


void leff_default (void)
{
	for (;;)
		task_sleep_sec (10);
}


void leff_start_highest_priority (void)
{
	db_puts ("Restarting highest priority leff\n");

	leff_active = leff_get_highest_priority ();
	if (leff_active != LEFF_NULL)
	{
		const leff_t *leff = &leff_table[leff_active];
		db_puts ("Recreating leff task\n");
		leff_create_handler (leff);
	}
	else
	{
		lamp_leff1_erase ();
		lamp_leff1_free_all ();
		task_recreate_gid (GID_LEFF, leff_default);
	}
}


__noreturn__ void leff_exit (void)
{
	db_puts ("Exiting leff\n");
	task_setgid (GID_LEFF_EXITING);
	leff_remove_queue (leff_active);
	leff_start_highest_priority ();
	task_exit ();
}


void leff_init (void)
{
	leff_prio = 0;
	leff_active = LEFF_NULL;
	memset (leff_queue, 0, MAX_QUEUED_LEFFS);
}


