
#include <freewpc.h>
#include <test.h>

S8 clock_test_setting;


void tz_clock_test_update (void)
{
	switch (clock_test_setting)
	{
		case -2:
			tz_clock_set_speed (1);
			tz_clock_start_backward ();
			break;
		case -1:
			tz_clock_set_speed (2);
			tz_clock_start_backward ();
			break;
		case 0:
			tz_clock_stop ();
			break;
		case 1:
			tz_clock_set_speed (2);
			tz_clock_start_forward ();
			break;
		case 2:
			tz_clock_set_speed (1);
			tz_clock_start_forward ();
			break;
	}
}


void tz_clock_test_init (void)
{
	tz_clock_stop ();
	clock_test_setting = 0;
	tz_clock_test_update ();
}


void tz_clock_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 2, "CLOCK MECH. TEST");
	switch (clock_test_setting)
	{
		case -2: sprintf ("BACKWARD FAST"); break;
		case -1: sprintf ("BACKWARD SLOW"); break;
		case 0: sprintf ("STOPPED"); break;
		case 1: sprintf ("FORWARD SLOW"); break;
		case 2: sprintf ("FORWARD FAST"); break;
	}
	font_render_string_center (&font_5x5, 64, 11, sprintf_buffer);
	dmd_show_low ();
}


void tz_clock_test_down (void)
{
	if (clock_test_setting > -2)
		clock_test_setting--;
	tz_clock_test_update ();
}


void tz_clock_test_up (void)
{
	if (clock_test_setting < 2)
		clock_test_setting++;
	tz_clock_test_update ();
}


void tz_clock_test_enter (void)
{
	/* Start/stop the clock */
}


struct window_ops tz_clock_test_window = {
	DEFAULT_WINDOW,
	.init = tz_clock_test_init,
	.draw = tz_clock_test_draw,
	.up = tz_clock_test_up,
	.down = tz_clock_test_down,
	.exit = tz_clock_stop,
};


struct menu tz_clock_test_item = {
	.name = "CLOCK MECH. TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_clock_test_window, NULL } },
};

