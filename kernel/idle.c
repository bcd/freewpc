
#include <freewpc.h>

#define IRQS_PER_100MS 96

U8 idle_second_timer;

U16 idle_ready_time;


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
	}
}


CALLSET_ENTRY (idle, init)
{
	idle_ready_time = get_sys_time () + IRQS_PER_100MS;
	idle_second_timer = 0;
}
