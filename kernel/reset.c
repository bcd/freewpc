
#include <freewpc.h>


static char gcc_version[] = C_STRING(GCC_VERSION);

static char build_date[] = __DATE__;


void system_reset (void)
{
	dmd_alloc_low_clean ();

	font_render_string (&font_5x5, 0, 0, MACHINE_NAME);

	sprintf ("SY %1x.%02x GCC %s", 
		FREEWPC_VERSION_MAJOR, FREEWPC_VERSION_MINOR, gcc_version);
	font_render_string (&font_5x5, 0, 7, sprintf_buffer);

	sprintf ("BUILT %s", build_date);
	font_render_string (&font_5x5, 0, 14, sprintf_buffer);

#ifdef USER_TAG
	font_render_string (&font_5x5, 0, 21, C_STRING(USER_TAG));
#endif

	dmd_show_low ();

	task_sleep_sec (3);

	dmd_alloc_low_clean ();
	dmd_show_low ();

	amode_start ();

	task_exit ();
}

