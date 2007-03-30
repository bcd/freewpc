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
			triac_leff_disable (TRIAC_GI_STRING (i));
			task_sleep (TIME_33MS);
			triac_leff_enable (TRIAC_GI_STRING (i));
		}
	}
}


void flasher_happy_leff (void)
{
	U8 i;
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
	U8 i;
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



void no_gi_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	task_sleep_sec (1);
	leff_exit ();
}


void flash_all_leff (void)
{
	U8 i;

	lampset_set_apply_delay (0);
	triac_leff_enable (TRIAC_GI_MASK);
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
	U8 i;
	for (i = 0; i < 6; i++)
	{
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_200MS);
	}
	leff_exit ();
}


void gumball_strobe_leff (void)
{
	U8 i;
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
	U8 i;

	leff_create_peer (gi_cycle_leff);
	for (i = 0; i < 12; i++)
	{
		flasher_pulse (FLASH_CLOCK_TARGET);
		task_sleep (TIME_200MS);
	}
	task_kill_gid (task_getgid ());
	leff_exit ();
}


void game_timeout_leff (void)
{
	U8 i;
	for (i=0; i < 3; i++)
	{
		task_sleep (TIME_500MS);
		triac_leff_enable (GI_POWERFIELD+GI_CLOCK);
		task_sleep (TIME_100MS);
		triac_leff_disable (GI_POWERFIELD+GI_CLOCK);
	}
	leff_exit ();
}


void clock_round_started_leff (void)
{
	U8 i;
	for (i=0; i < 3; i++)
	{
		triac_leff_enable (GI_CLOCK);
		task_sleep (TIME_100MS);
		triac_leff_disable (GI_CLOCK);
		task_sleep (TIME_200MS);
	}
	leff_exit ();
}


void multiball_running_leff (void)
{
	leff_on (LM_GUM);
	leff_off (LM_BALL);
	leff_off (LM_LOCK1);
	leff_on (LM_LOCK2);
	for (;;)
	{
		lampset_apply_leff_toggle (LAMPSET_DOOR_LOCKS_AND_GUMBALL);
		task_sleep (TIME_200MS);
	}
	leff_exit ();
}

void pf_strobe_up_subtask (void)
{
	for (;;) lampset_apply_leff_toggle (LAMPSET_SORT1);
}

void strobe_up_leff (void)
{
	lampset_set_apply_delay (TIME_16MS);
	leff_create_peer (pf_strobe_up_subtask);
	task_sleep (TIME_200MS);
	leff_create_peer (pf_strobe_up_subtask);
	task_sleep_sec (1);
	task_kill_peers ();
	leff_exit ();
}

void multi_strobe1_subtask (void)
{ for (;;) { lampset_set_apply_delay (TIME_33MS); lampset_apply_leff_toggle (LAMPSET_SORT1); task_sleep (TIME_500MS); } }
void multi_strobe2_subtask (void)
{ for (;;) { lampset_set_apply_delay (TIME_33MS); lampset_apply_leff_toggle (LAMPSET_SORT2); task_sleep (TIME_500MS); } }
void multi_strobe3_subtask (void)
{ for (;;) { lampset_set_apply_delay (TIME_33MS); lampset_apply_leff_toggle (LAMPSET_SORT3); task_sleep (TIME_500MS); } }

void multi_strobe_leff (void)
{
	U8 i;

	leff_create_peer (multi_strobe1_subtask);
	task_sleep (TIME_300MS);
	leff_create_peer (multi_strobe2_subtask);
	task_sleep (TIME_300MS);
	leff_create_peer (multi_strobe3_subtask);
	task_sleep (TIME_300MS);

	for (i=0; i< 2; i++)
	{
		lampset_set_apply_delay (TIME_33MS);
		lampset_apply_leff_toggle (LAMPSET_SORT4);
		task_sleep (TIME_500MS);
	}
	task_kill_peers ();
	leff_exit ();
}

void door_strobe_subtask (void)
{
	lampset_set_apply_delay (TIME_16MS);
	for (;;)
		lampset_apply_leff_toggle (LAMPSET_DOOR_PANELS);
}

void door_strobe_leff (void)
{
	triac_leff_disable (TRIAC_GI_MASK);
	lampset_apply_leff_off (LAMPSET_DOOR_PANELS);
	lampset_set_apply_delay (TIME_33MS);
	leff_create_peer (door_strobe_subtask);
	task_sleep (TIME_100MS);
	leff_create_peer (door_strobe_subtask);
	task_sleep_sec (2);
	task_kill_peers ();
	leff_exit ();
}

void right_loop_leff (void)
{
	lampset_set_apply_delay (TIME_16MS);
	lampset_apply_leff_toggle (LAMPSET_SORT4);
	lampset_apply_leff_toggle (LAMPSET_SORT4);
	leff_exit ();
}

void left_loop_leff (void)
{
	lampset_set_apply_delay (TIME_16MS);
	lampset_apply_leff_toggle (LAMPSET_SORT3);
	lampset_apply_leff_toggle (LAMPSET_SORT3);
	leff_exit ();
}


void jets_active_leff (void)
{
	lampset_set_apply_delay (TIME_200MS);
	for (;;)
		lampset_leff_step_increment (LAMPSET_JETS);
}

