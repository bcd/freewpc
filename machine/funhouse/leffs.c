
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
	lamplist_set_apply_delay (TIME_33MS);
	lamplist_apply (LAMPLIST_CIRCLE_OUT, leff_on);
	lamplist_apply (LAMPLIST_CIRCLE_OUT, leff_off);
	leff_exit ();
}

void jackpot_subleff (void)
{
	lamplist_apply_nomacro (LAMPLIST_CIRCLE_OUT, leff_off);
	lamplist_set_apply_delay (TIME_33MS);
	lamplist_apply (LAMPLIST_CIRCLE_OUT, leff_on);
	leff_exit ();
}

void jackpot_leff (void)
{
	//task_recreate_gid (GID_JACKPOT_SUBLEFF, jackpot_subleff);
}

void clock_vibrate_leff1 (void)
{
	for (;;)
	{
		lamplist_apply (LAMPLIST_CLOCK_HOURS_MINUTES, leff_toggle);
		task_sleep (TIME_100MS);
	}
}

void clock_vibrate_leff (void)
{
	lamplist_apply_leff_alternating (LAMPLIST_CLOCK_HOURS, 0);
	lamplist_apply_leff_alternating (LAMPLIST_CLOCK_MINUTES, 0xFF);
	leff_create_peer (clock_vibrate_leff1);
	task_sleep_sec (1);
	leff_exit ();
}


void gangway_strobe_leff1 (void)
{
	for (;;)
	{
		lamplist_step_increment (LAMPLIST_GANGWAYS,
			matrix_lookup (LMX_EFFECT1_LAMPS));
		task_sleep (TIME_66MS);
	}
}

void gangway_strobe_leff (void)
{
	leff_on (lamplist_index (LAMPLIST_GANGWAYS, 0));
	leff_create_peer (gangway_strobe_leff1);
	task_sleep_sec (3);
	leff_exit ();
}

