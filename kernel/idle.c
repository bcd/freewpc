
#include <freewpc.h>

/** The number of 8ms ticks that have elapsed since the last
 * idle check */
U8 idle_8ms_ticks;

/** The current 100ms timer.  This value counts up, basically
 * tracking idle_8ms_ticks above.  However, this may contain
 * residual ticks from previous calls that need to be remembered,
 * so that even if a particular idle call runs a bit late, overall
 * it will continue to track the desired 100ms interval. */
volatile U8 idle_100ms_timer;

/** The time at which the next 100ms event should be thrown.
 * Because the underlying tick is 8ms, we should do this every
 * 12.5 ticks... the approach is to alternate between calling
 * at 12 then 13 ticks. */
U8 idle_100ms_expire_time;

U8 idle_second_timer;


/** Increment the number of idle ticks, every 8ms. */
void idle_rtt (void)
{
	idle_8ms_ticks++;
}


/** Runs the idle function */
void do_idle (void)
{
	/* See how much time has elapsed since the last idle call.
	 * Beware of overflow here.  These are all 8-bit counters.
	 * Idle state should happen at least once every two seconds
	 * to avoid the IRQ tick overflow. */
	disable_irq ();
	idle_100ms_timer += idle_8ms_ticks;
	idle_8ms_ticks = 0;
	enable_irq ();

	/* Throw the 100ms event if that much time has elapsed */
	if (idle_100ms_timer >= idle_100ms_expire_time)
	{
		idle_100ms_timer -= idle_100ms_expire_time;
		if (idle_100ms_expire_time >= 13)
			idle_100ms_expire_time--;
		else
			idle_100ms_expire_time++;
		callset_invoke (idle_every_100ms);

		/* Throw the 1 second event if that has elapsed */
		idle_second_timer++;
		if (idle_second_timer >= 10)
		{
			idle_second_timer -= 10;
			callset_invoke (idle_every_second);
		}
	}
}


CALLSET_ENTRY (idle, init)
{
	idle_8ms_ticks = 0;
	idle_100ms_expire_time = 12;
	idle_100ms_timer = 0;
	idle_second_timer = 0;
}
