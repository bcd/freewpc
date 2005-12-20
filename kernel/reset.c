
#include <freewpc.h>

#define ACCEPT_1	0x19
#define ACCEPT_2	0x75
#define ACCEPT_3	0xB9

volatile const char gcc_version[] = C_STRING(GCC_VERSION);

static char build_date[] = __DATE__;

__nvram__ U8 freewpc_accepted[3];


#pragma long_branch
void system_accept_freewpc (void)
{
	if ((freewpc_accepted[0] == ACCEPT_1) &&
		 (freewpc_accepted[1] == ACCEPT_2) &&
		 (freewpc_accepted[2] == ACCEPT_3))
		return;

	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 0, "FREEWPC");
	font_render_string_center (&font_5x5, 64, 6, "WARNING... BALLY WMS");
	font_render_string_center (&font_5x5, 64, 12, "DOES NOT SUPPORT");
	font_render_string_center (&font_5x5, 64, 18, "THIS SOFTWARE");
	font_render_string_center (&font_5x5, 64, 24, "PRESS ENTER");
	dmd_show_low ();
	while (!switch_poll (SW_ENTER)) task_sleep (TIME_66MS);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 0, "FREEWPC");
	font_render_string_center (&font_5x5, 64, 6, "NO WARRANTY EXISTS");
	font_render_string_center (&font_5x5, 64, 12, "ROM MAY CAUSE DAMAGE");
	font_render_string_center (&font_5x5, 64, 18, "TO YOUR MACHINE");
	font_render_string_center (&font_5x5, 64, 24, "PRESS ENTER");
	dmd_show_low ();
	while (!switch_poll (SW_ENTER)) task_sleep (TIME_66MS);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 0, "FREEWPC");
	font_render_string_center (&font_5x5, 64, 6, "IF YOU ARE SURE YOU");
	font_render_string_center (&font_5x5, 64, 12, "WANT TO CONTINUE");
	font_render_string_center (&font_5x5, 64, 18, "PRESS ENTER TWICE");
	dmd_show_low ();
	while (!switch_poll (SW_ENTER)) task_sleep (TIME_66MS);
	while (switch_poll (SW_ENTER)) task_sleep (TIME_66MS);
	while (!switch_poll (SW_ENTER)) task_sleep (TIME_66MS);
	while (switch_poll (SW_ENTER)) task_sleep (TIME_66MS);

	dmd_alloc_low_clean ();
	dmd_show_low ();
	task_sleep_sec (1);

	wpc_nvram_get ();
	freewpc_accepted[0] = ACCEPT_1;
	freewpc_accepted[1] = ACCEPT_2;
	freewpc_accepted[2] = ACCEPT_3;
	wpc_nvram_put ();
}
#pragma short_branch


void system_reset (void)
{
	system_accept_freewpc ();
	dmd_alloc_low_clean ();

	font_render_string_center (&font_5x5, 64, 0, MACHINE_NAME);

	sprintf ("SY %1x.%02x   %s", 
		FREEWPC_VERSION_MAJOR, FREEWPC_VERSION_MINOR, build_date);
	font_render_string (&font_5x5, 0, 7, sprintf_buffer);

	font_render_string_center (&font_5x5, 64, 21, "TESTING...");

#ifdef USER_TAG
	font_render_string_center (&font_5x5, 64, 14, C_STRING(USER_TAG));
#endif

	dmd_show_low ();

	task_sleep_sec (2);
	while (sys_init_pending_tasks != 0)
		task_sleep (TIME_66MS);

	sys_init_complete++;

	dmd_alloc_low_clean ();
	dmd_show_low ();

	amode_start ();

	task_exit ();
}

