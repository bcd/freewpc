
#include <stdlib.h>
#include <freewpc.h>
#include "/usr/include/sys/time.h"


/** The frequency of the realtime loop, in millseconds */
#define RT_FREQ 16


unsigned long realtime_counter;


unsigned long realtime_read (void)
{
	return realtime_counter;
}


/**
 * Implement a realtime loop on a non-realtime OS.
 */
void realtime_loop (void)
{
	struct timeval prev_time, curr_time;
	int msecs;

	/* TODO - boost priority of this process, so that it always
	 * takes precedence over higher priority stuff. */

	gettimeofday (&prev_time, NULL);
	for (;;)
	{
		/* Sleep before the next iteration of the loop */
		task_sleep ((RT_FREQ * TIME_16MS) / 16);

		/* Now see how long we actually slept.  This takes into account the
		actual sleep time, which is typically longer on a multitasking OS,
		and also the length of time that the previous iteration took. */
		gettimeofday (&curr_time, NULL);
		msecs = (curr_time.tv_usec - prev_time.tv_usec) / 1000;
		if (msecs < 0)
			msecs += 1000;
		prev_time = curr_time;

		/* For each elapsed millsecond, invoke all of the realtime_tick
		handlers. */
		while (msecs-- > 0)
		{
			realtime_counter++;
			callset_invoke (realtime_tick);
		}
	}
}


CALLSET_ENTRY (native_realtime, init)
{
	realtime_counter = 0;
}

