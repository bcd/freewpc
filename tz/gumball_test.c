
#include <freewpc.h>
#include <test.h>


void tz_gumball_test_update (void)
{
}


void tz_gumball_test_init (void)
{
}


void tz_gumball_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "GUMBALL TEST");
	dmd_show_low ();
}


void tz_gumball_test_down (void)
{
}


void tz_gumball_test_up (void)
{
}


void tz_gumball_test_enter (void)
{
}


struct window_ops tz_gumball_test_window = {
	DEFAULT_WINDOW,
	.init = tz_gumball_test_init,
	.draw = tz_gumball_test_draw,
	.up = tz_gumball_test_up,
	.down = tz_gumball_test_down,
	.exit = tz_clock_stop,
};


struct menu tz_gumball_test_item = {
	.name = "GUMBALL TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_gumball_test_window, NULL } },
};

