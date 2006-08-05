/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>

void gi_cycle_leff (void)
{
	U8 i;
	for (;;)
	{
		for (i=0; i < 5; i++)
		{
			triac_disable (TRIAC_GI_MASK);
			triac_enable (TRIAC_GI_STRING (i));
			task_sleep (TIME_33MS);
		}
	}
}


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
	task_sleep_sec (1);
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


void slot_kickout_leff (void)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_200MS);
	}
	leff_exit ();
}


void gumball_strobe_leff (void)
{
	int i;
	for (i = 0; i < 6 ; i++)
	{
		flasher_pulse (FLASH_GUMBALL_HIGH);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_GUMBALL_MID);
		task_sleep (TIME_100MS);
		flasher_pulse (FLASH_GUMBALL_LOW);
		task_sleep (TIME_100MS * 2);
	}
	leff_exit ();
}


void clock_target_leff (void)
{
	int i;

	task_create_child (gi_cycle_leff);
	for (i = 0; i < 12; i++)
	{
		flasher_pulse (FLASH_CLOCK_TARGET);
		task_sleep (TIME_200MS);
	}
	task_kill_gid (task_getgid ());
	leff_exit ();
}


