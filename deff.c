
#include <freewpc.h>

typedef uint8_t deffnum_t;

typedef void (*deff_function_t) (void) __taskentry__;

#define D_NORMAL	0x0

/** A running deff is long-lived and continues to be active
 * until it is explicitly stopped. */
#define D_RUNNING 0x1

typedef struct
{
	uint8_t flags;
	uint8_t prio;
	deff_function_t fn;
} deff_t;

#define MAX_QUEUED_DEFFS 16

#define DEFF_NULL		0

static const deff_t deff_table[] = {
	[DEFF_NULL] = { D_NORMAL, 250, NULL },
};

static uint8_t deff_prio;
static uint8_t deff_queue[MAX_QUEUED_DEFFS];


static void deff_add_queue (deffnum_t dn)
{
	uint8_t i;
	for (i=0; i < MAX_QUEUED_DEFFS; i++)
		if (deff_queue[i] == 0)
		{
			deff_queue[i] = dn;
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
			deff_t *deff = &deff_table[deff_queue[i]];
			if (deff->prio > prio)
			{
				prio = deff->prio;
				best = i;
			}
		}
		else
			break;

	return best;
}


void deff_start (deffnum_t dn)
{
	deff_t *deff = &deff_table[dn];

	if (deff->flags & D_RUNNING)
	{
		deff_add_queue (dn);
		if (deff->prio > deff_get_highest_priority ())
		{
		}
	}
	else
	{
	}
}


void deff_stop (deffnum_t dn)
{
}


void deff_exit (void) __noreturn__
{
	task_exit ();
}


void deff_init (void)
{
	deff_prio = 0;
	memset (deff_queue, 0, MAX_QUEUED_DEFFS);
}


