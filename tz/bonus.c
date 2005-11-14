
#include <freewpc.h>

void bonus_leff (void)
{
	triac_disable (TRIAC_GI_MASK);
	for (;;)
		task_sleep_sec (5);
}

void bonus_deff (void)
{
	music_set (MUS_BONUS_START);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_5x5, 64, 10, "BONUS");
	font_render_string_center (&font_5x5, 64, 22, "TBD");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}
