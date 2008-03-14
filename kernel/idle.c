
#include <freewpc.h>

U8 idle_8ms_ticks;

U8 idle_100ms_expire_time;

volatile U8 idle_100ms_timer;

U8 idle_second_timer;


void idle_rtt (void)
{
	idle_8ms_ticks++;
}


void do_idle (void)
{
	/* See how much time has elapsed since the last idle call. */
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
