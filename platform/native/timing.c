
#include <freewpc.h>
#include <simulation.h>

#define RING_COUNT 32

#define ring_later(ticks) ((ring_now + (ticks)) % RING_COUNT)

unsigned int ring_now = 0;

struct time_handler *time_handler_ring[RING_COUNT] = { NULL, };

static struct time_handler *ring_malloc (void)
{
	return malloc (sizeof (struct time_handler));
}

static void ring_free (struct time_handler *elem)
{
	free (elem);
}

/* Register a function to be called after N_TICKS have elapsed. */
void sim_time_register (int n_ticks, int periodic_p, time_handler_t fn, void *data)
{
	unsigned int ring = ring_later (n_ticks);

	struct time_handler *elem = ring_malloc ();
	if (!elem)
		simlog (SLC_DEBUG, "can't alloc ring");

	elem->next = time_handler_ring[ring];
	elem->periodicity = periodic_p ? n_ticks : 0;
	elem->fn = fn;
	elem->data = data;
	time_handler_ring[ring] = elem;
}


/* Advance the simulation time by 1 tick. */
void sim_time_step (void)
{
	struct time_handler *elem, *elem_next;

	/* Atomically get and clear the list of timers to
	 * be executed on this tick */
	elem = time_handler_ring[ring_now];
	time_handler_ring[ring_now] = NULL;

	/* Call each timer function */
	while (elem != NULL)
	{
		(*elem->fn) (elem->data);

		if (elem->periodicity)
		{
			/* If periodic, just requeue it rather than free/alloc */
			elem_next = elem->next;
			elem->next = time_handler_ring[ring_later (elem->periodicity)];
			time_handler_ring[ring_later (elem->periodicity)] = elem;
			elem = elem_next;
		}
		else
		{
			elem_next = elem->next;
			ring_free (elem);
			elem = elem_next;
		}
	}
	ring_now = ring_later (1);
}

