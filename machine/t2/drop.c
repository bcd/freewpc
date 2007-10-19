
#include <freewpc.h>

#define drop_poll() switch_poll_logical (SW_DROP_TARGET)
#define drop_down_p() drop_poll ()
#define drop_up_p() (!drop_poll ())


void drop_set_up (void)
{
	sol_start (SOL_SKULL_DROP_UP, SOL_DUTY_50, TIME_1S);
}

void drop_set_down (void)
{
	sol_start (SOL_SKULL_DROP_DOWN, SOL_DUTY_50, TIME_66MS);
}

void drop_is_down (void)
{
	/* TODO - debounce drops */
	callset_invoke (drop_hit);
}

void drop_is_up (void)
{
}


CALLSET_ENTRY (drop, sw_drop_target)
{
	if (drop_down_p ())
	{
		dbprintf ("Drop is down\n");
		drop_is_down ();
	}
	else
	{
		dbprintf ("Drop is up\n");
		drop_is_up ();
	}
}


CALLSET_ENTRY (drop, drop_hit)
{
	sound_send (SND_SPLAT);
	score (SC_100K);
	task_sleep (TIME_500MS);
	drop_set_up ();
}


CALLSET_ENTRY (drop, start_ball)
{
	drop_set_up ();
}

