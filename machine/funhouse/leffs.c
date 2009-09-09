
#include <freewpc.h>

static U8 lamplist;

void amode_leff1 (void)
{
	register U8 my_lamplist = lamplist;
	lamplist_set_apply_delay (TIME_66MS);
	for (;;)
		lamplist_apply (my_lamplist, leff_toggle);
}

void amode_leff (void)
{
	triac_leff_enable (TRIAC_GI_MASK);

	for (lamplist = LAMPLIST_GANGWAYS; lamplist <= LAMPLIST_LOWER_LANES; lamplist++)
	{
		leff_create_peer (amode_leff1);
		task_sleep (TIME_166MS);
	}
	task_exit ();
}

void circle_out_leff (void)
{
	lamplist_set_apply_delay (TIME_16MS);
	lamplist_apply (LAMPLIST_CIRCLE_OUT, leff_on);
	lamplist_apply (LAMPLIST_CIRCLE_OUT, leff_off);
	leff_exit ();
}

