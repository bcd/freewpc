
#include <freewpc.h>

U8 amode_leff_subset;

void amode_leff_subset_task (void)
{
	register U8 lamplist = amode_leff_subset;
	lamplist_set_apply_delay (TIME_100MS);
	for (;;)
		lamplist_apply (lamplist, leff_toggle);
}

void amode_leff (void)
{
	U8 i;
	gi_leff_enable (PINIO_GI_STRINGS);

	for (amode_leff_subset = LAMPLIST_THEATRE_SPELL;
		amode_leff_subset <= LAMPLIST_TRUNK_VERTICAL;
		amode_leff_subset++)
	{
		leff_create_peer (amode_leff_subset_task);
		task_sleep (TIME_33MS);
	}
	while (1)
		task_sleep_sec (4);
}


