
#include <freewpc.h>

/* The new timer functions provide a better way to create
dynamic timers that are more space efficient, using malloc()
instead of dedicating an entire task block. */

/* The size of 1 'timer tick' */
#define TIMER_GRAN TIME_100MS

typedef struct {
	struct dll_header dll;
	U8 ticks;
	U8 gid;
} timer_t;


/* A list of all running timers, in no particular order */
timer_t *timer_runlist;

U8 timer_min_ticks;


void update_timer_min_ticks (void)
{
	timer_min_ticks = 100;

	timer_t *timer = timer_runlist;
	if (timer)
	{
		do {
			if (timer->ticks < ticks)
				timer_min_ticks = timer->ticks;
			timer = timer->dll.next;
		} while (timer != timer_runlist);
	}
}


void new_timer_driver (void)
{
	timer_t *t;

	task_set_flags (TASK_PROTECTED);
	dll_init (&timer_runlist);

	for (;;)
	{
		if ((t = timer_runlist) != NULL)
		{
			do {
				t->ticks -= timer_min_ticks;
				if (t->ticks == 0)
				{
					/* Timer has expired */
				}
				t = t->dll.next;
			} while (t != timer_runlist);
		}
		update_timer_min_ticks ();
		task_sleep (TIMER_GRAN * timer_min_ticks);
	}
}



static inline timer_t *timer_alloc (U16 task_ticks)
{
	timer_t *t = malloc (sizeof (timer_t));
	t->ticks = task_ticks / TIMER_GRAN;
	return t;
}

static inline void timer_free (timer_t *t)
{
	free (t);
}


void xtimer_restart (task_gid_t gid, U16 ticks)
{
}

void xtimer_start1 (task_gid_t gid, U16 ticks)
{
}

void xtimer_start (task_gid_t gid, U16 ticks)
{
}

