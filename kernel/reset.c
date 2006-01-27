
#include <freewpc.h>

#define ACCEPT_1	0x19
#define ACCEPT_2	0x75
#define ACCEPT_3	0xB9

volatile const char gcc_version[] = C_STRING(GCC_VERSION);

static char build_date[] = BUILD_DATE;

__nvram__ U8 freewpc_accepted[3];


void system_accept_freewpc (void)
{
	extern void adj_reset_all (void);

	if ((freewpc_accepted[0] == ACCEPT_1) &&
		 (freewpc_accepted[1] == ACCEPT_2) &&
		 (freewpc_accepted[2] == ACCEPT_3))
		return;

	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 3, "FREEWPC");
	font_render_string_center (&font_5x5, 64, 9, "WARNING... BALLY WMS");
	font_render_string_center (&font_5x5, 64, 15, "DOES NOT SUPPORT");
	font_render_string_center (&font_5x5, 64, 21, "THIS SOFTWARE");
	font_render_string_center (&font_5x5, 64, 27, "PRESS ENTER");
	dmd_show_low ();
	while (!switch_poll (SW_ENTER)) task_sleep (TIME_66MS);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 3, "FREEWPC");
	font_render_string_center (&font_5x5, 64, 9, "NO WARRANTY EXISTS");
	font_render_string_center (&font_5x5, 64, 15, "ROM MAY CAUSE DAMAGE");
	font_render_string_center (&font_5x5, 64, 21, "TO YOUR MACHINE");
	font_render_string_center (&font_5x5, 64, 27, "PRESS ENTER");
	dmd_show_low ();
	while (!switch_poll (SW_ENTER)) task_sleep (TIME_66MS);

	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 3, "FREEWPC");
	font_render_string_center (&font_5x5, 64, 9, "IF YOU ARE SURE YOU");
	font_render_string_center (&font_5x5, 64, 15, "WANT TO CONTINUE");
	font_render_string_center (&font_5x5, 64, 21, "PRESS ENTER TWICE");
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

	call_far (TEST_PAGE, adj_reset_all ());
}


void system_reset (void)
{
	system_accept_freewpc ();
	dmd_alloc_low_clean ();

	font_render_string_center (&font_5x5, 64, 4, MACHINE_NAME);

#ifdef DEBUGGER
	sprintf ("D%1x.%02x", MACHINE_MAJOR_VERSION, MACHINE_MINOR_VERSION);
#else
	sprintf ("R%1x.%02x", MACHINE_MAJOR_VERSION, MACHINE_MINOR_VERSION);
#endif
	font_render_string_center (&font_5x5, 32, 12, sprintf_buffer);
	font_render_string_center (&font_5x5, 96, 12, build_date);

#ifdef USER_TAG
	font_render_string_center (&font_5x5, 64, 20, C_STRING(USER_TAG));
	font_render_string_center (&font_5x5, 64, 28, "TESTING...");
#else
	font_render_string_center (&font_5x5, 64, 24, "TESTING...");
#endif

	dmd_show_low ();

	task_sleep_sec (2);
	while (sys_init_pending_tasks != 0)
		task_sleep (TIME_66MS);

	sys_init_complete++;

	/* TODO : ??? why is this here ? */
	dmd_alloc_low_clean ();
	dmd_show_low ();

	amode_start ();

	task_exit ();
}

