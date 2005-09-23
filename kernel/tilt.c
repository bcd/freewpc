
#include <freewpc.h>


void tilt_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 13, "TILT");
	dmd_show_low ();
	task_sleep_sec (5);
	deff_exit ();
}


void tilt_warning_deff (void) __taskentry__
{
	deff_exit ();
}


void slam_tilt_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 13, "SLAM TILT");
	dmd_show_low ();
	task_sleep_sec (5);
	deff_exit ();
}


void sw_tilt_handler (void)
{
}


void sw_slam_tilt_handler (void)
{
}

DECLARE_SWITCH_DRIVER (sw_tilt)
{
	.fn = sw_tilt_handler,
};

DECLARE_SWITCH_DRIVER (sw_slam_tilt)
{
	.fn = sw_slam_tilt_handler,
};

