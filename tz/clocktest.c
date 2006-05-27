
#include <freewpc.h>

void tz_clock_test_init (void)
{
	tz_clock_stop ();
}


void tz_clock_test_down (void)
{
	/* Go backward */
	tz_clock_start_backward ();
}


void tz_clock_test_up (void)
{
	/* Go forward */
	tz_clock_start_forward ();
}


void tz_clock_test_enter (void)
{
	/* Start/stop the clock */
}


struct window_ops tz_clock_test_window = {
	.init = tz_clock_test_init,
	.exit = tz_clock_stop,
};


struct menu tz_clock_test_item = {
	.name = "CLOCK MECH. TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_clock_test_window, NULL } },
};

