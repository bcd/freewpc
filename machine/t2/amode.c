
#include <freewpc.h>

static U8 lamplist;


void amode_leff1 (void)
{
	register U8 my_lamplist = lamplist;
	lamplist_set_apply_delay (TIME_66MS);
	for (;;)
		lamplist_apply (my_lamplist, leff_toggle);
}

void amode_leff_eyes (void)
{
	U8 n;

	for (;;)
	{
		task_sleep_sec (4);
		for (n=0; n < 4; n++)
		{
			leff_toggle (LM_EYES_LOWER);
			task_sleep (TIME_100MS);
		}

		task_sleep_sec (6);
		for (n=0; n < 6; n++)
		{
			leff_toggle (LM_EYES_LOWER);
			task_sleep (TIME_66MS);
		}
	}
}

void amode_leff (void)
{
	triac_leff_enable (TRIAC_GI_MASK);

	for (lamplist = LAMPLIST_BONUS_X; lamplist <= LAMPLIST_RIGHT_LOOP; lamplist++)
	{
		leff_create_peer (amode_leff1);
		task_sleep (TIME_33MS);
	}
	leff_create_peer (amode_leff_eyes);
	task_exit ();
}


void amode_deff (void)
{
	dmd_alloc_low_high ();
	dmd_draw_fif (fif_freewpc_logo);
	dmd_show2 ();
	for (;;)
		task_sleep_sec (5);
}
