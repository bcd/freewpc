
#include <freewpc.h>

/** The frequency to run the idle checks */
#define IDLE_PERIOD (TIME_100MS * IRQS_PER_TICK)

/** The next time at which we should run the idle event.
This is calculated as 100ms from the last timeout. */
U16 idle_ready_time;

/** The number of 100ms events generated.  When this
reaches 10, we clear it and consider that one second
has passed. */
U8 idle_second_timer;

/** The number of 1sec events generated.  When this
reaches 10, we clear it and consider that 10 seconds
have passed. */
U8 idle_10second_timer;


/** Runs the idle functions. */
void do_idle (void)
{
	/* See if at least 100ms has elapsed.
	If so, we advance the timeout for the next check.
	If more than 200ms elapsed, we will only process
	1 'tick' on the current call, and do it again
	on the next run. */
	if (time_reached_p (idle_ready_time))
		idle_ready_time += IDLE_PERIOD;
	else
		return;

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
	idle_ready_time = get_sys_time () + IDLE_PERIOD;
	idle_second_timer = 0;
	idle_10second_timer = 0;
}
