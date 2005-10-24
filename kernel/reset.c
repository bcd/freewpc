
#include <freewpc.h>


void system_reset (void)
{
	dmd_alloc_low_clean ();
	sprintf ("FREEWPC V%1x.%02x", FREEWPC_VERSION_MAJOR, FREEWPC_VERSION_MINOR);
	font_render_string (&font_5x5, 0, 0, sprintf_buffer);
	dmd_show_low ();

	task_sleep_sec (3);

	amode_start ();

	task_exit ();
}

