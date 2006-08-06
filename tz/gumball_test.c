
#include <freewpc.h>
#include <test.h>


U8 gumball_op;


void tz_gumball_test_update (void)
{
}


void tz_gumball_test_init (void)
{
	gumball_op = 0;
}


void tz_gumball_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "GUMBALL TEST");
	switch (gumball_op)
	{
		case 0:
			sprintf ("LOAD"); break;
		case 1:
			sprintf ("RELEASE"); break;
	}
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	dmd_show_low ();
}


void tz_gumball_test_down (void)
{
	gumball_op = 1 - gumball_op;
}


void tz_gumball_test_up (void)
{
	gumball_op = 1 - gumball_op;
}


void tz_gumball_test_enter (void)
{
	switch (gumball_op)
	{
		case 0:
			gumball_load_from_trough ();
			break;

		case 1:
			gumball_release ();
			break;
	}
}


struct window_ops tz_gumball_test_window = {
	DEFAULT_WINDOW,
	.init = tz_gumball_test_init,
	.draw = tz_gumball_test_draw,
	.up = tz_gumball_test_up,
	.down = tz_gumball_test_down,
	.enter = tz_gumball_test_enter,
};


struct menu tz_gumball_test_item = {
	.name = "GUMBALL TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_gumball_test_window, NULL } },
};

