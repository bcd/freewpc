
#include <freewpc.h>

#define IRQS_PER_100MS 96

U16 idle_ready_time;

U8 idle_second_timer;

U8 idle_10second_timer;


/** Runs the idle function */
void do_idle (void)
{
	/* See if 100ms has elapsed */
	if (time_reached_p (idle_ready_time))
	{
		idle_ready_time += IRQS_PER_100MS;
	}
	else
	{
		return;
	}

	/* Throw the 100ms event */
	callset_invoke (idle_every_100ms);

	/* Throw the 1 second event every 10 calls */
	idle_second_timer++;
	if (idle_second_timer >= 10)
	{
		idle_second_timer -= 10;
		callset_invoke (idle_every_second);

		/* Throw the 10 second event if that has elapsed */
		idle_10second_timer++;
		if (idle_10second_timer >= 10)
		{
			idle_10second_timer -= 10;
			callset_invoke (idle_every_ten_seconds);
		}
	}
}


CALLSET_ENTRY (idle, init)
{
	idle_ready_time = get_sys_time () + IRQS_PER_100MS;
	idle_second_timer = 0;
	idle_10second_timer = 0;
}
