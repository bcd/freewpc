
#include <freewpc.h>

U8 tilt_warnings;


void tilt_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 13, "TILT");
	dmd_show_low ();
	while (in_tilt)
		task_sleep_sec (1);
	deff_exit ();
}


void tilt_warning_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	dmd_alloc_high_clean ();
	font_render_string_center (&font_5x5, 64, 13, "DANGER");
	dmd_show_low ();
	deff_swap_low_high (32, TIME_66MS);
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
	extern bool in_tilt;

	if (in_tilt)
		return;
	else if (++tilt_warnings == 3)
	{
		deff_start (DEFF_TILT);
		in_tilt = TRUE;
		flipper_disable ();
		call_hook (tilt);
	}
	else
	{
		deff_start (DEFF_TILT_WARNING);
		call_hook (tilt_warning);
	}
}


void sw_slam_tilt_handler (void)
{
	deff_start (DEFF_SLAM_TILT);
}


DECLARE_SWITCH_DRIVER (sw_tilt)
{
	.fn = sw_tilt_handler,
	.flags = SW_IN_GAME,
};

DECLARE_SWITCH_DRIVER (sw_slam_tilt)
{
	.fn = sw_slam_tilt_handler,
	.flags = SW_IN_GAME,
};


void tilt_start_ball (void)
{
	tilt_warnings = 0;
}

