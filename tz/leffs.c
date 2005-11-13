
#include <freewpc.h>

void flash_all_leff (void)
{
	int i;

	// dbprintf ("flash_all_leffs running\n");
	lampset_set_apply_delay (0);
	triac_enable (TRIAC_GI_MASK);
	lampset_apply_leff_alternating (LAMPSET_AMODE_ALL, 0);
	for (i=0; i < 32; i++)
	{
		lampset_apply_leff_toggle (LAMPSET_AMODE_ALL);
		task_sleep (TIME_66MS);
	}
	lampset_apply_leff_on (LAMPSET_AMODE_ALL);
	task_sleep_sec (2);
	leff_exit ();
}

