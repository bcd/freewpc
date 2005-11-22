
#include <freewpc.h>


void flasher_happy_leff (void)
{
	int i;
	for (i=0; i < 8; i++)
	{
		flasher_pulse (FLASH_CLOCK_TARGET);
		flasher_pulse (FLASH_RAMP1);
		flasher_pulse (FLASH_GUMBALL_HIGH);
		task_sleep (TIME_100MS);

		flasher_pulse (FLASH_RAMP2);
		flasher_pulse (FLASH_GUMBALL_MID);
		task_sleep (TIME_100MS);

		flasher_pulse (FLASH_GUMBALL_LOW);
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_100MS);
	}
	leff_exit ();
}


void left_ramp_leff (void)
{
	int i;
	for (i=0; i < 4; i++)
	{
		flasher_pulse (FLASH_RAMP1);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_RAMP2);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_100MS * 2);
	}
	leff_exit ();
}


void no_lights_leff (void)
{
	triac_disable (TRIAC_GI_MASK);
	for (;;)
		task_sleep_sec (5);
}


void no_gi_leff (void)
{
	triac_disable (TRIAC_GI_MASK);
	task_sleep_sec (2);
	triac_enable (TRIAC_GI_MASK);
	leff_exit ();
}


void flash_all_leff (void)
{
	int i;

	lampset_set_apply_delay (0);
	triac_enable (TRIAC_GI_MASK);
	lampset_apply_leff_alternating (LAMPSET_AMODE_ALL, 0);
	for (i=0; i < 32; i++)
	{
		lampset_apply_leff_toggle (LAMPSET_AMODE_ALL);
		task_sleep (TIME_66MS);
	}
	lampset_apply_leff_on (LAMPSET_AMODE_ALL);
	task_sleep_sec (1);
	leff_exit ();
}

