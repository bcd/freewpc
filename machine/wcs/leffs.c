
#include <freewpc.h>

void color_cycle_leff (void)
{
	U8 list;
	for (;;)
	{
		for (list = LAMPLIST_RED_LAMPS; list <= LAMPLIST_PURPLE_LAMPS; list++)
		{
			lamplist_apply_nomacro (list, leff_on);
			task_sleep (TIME_100MS);
			lamplist_apply_nomacro (list, leff_off);
		}
	}
}

void build_up_task (void)
{
	lamplist_apply (LAMPLIST_BUILD_UP, leff_toggle);
	task_exit ();
}

void build_up_leff (void)
{
	lamplist_set_apply_delay (TIME_16MS);
	leff_create_peer (build_up_task);
	task_sleep (TIME_500MS);
	leff_create_peer (build_up_task);
	task_sleep (TIME_500MS);
	task_kill_peers ();
	leff_exit ();
}

