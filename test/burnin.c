/*
 * Copyright 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

timestamp_t burnin_duration;

__nvram__ timestamp_t burnin_total_duration;

void burnin_sound_thread (void)
{
	for (;;)
	{
		music_set (1);
		task_sleep_sec (5);
		music_set (2);
		task_sleep_sec (5);
		music_set (3);
		task_sleep_sec (5);
	}
}

void burnin_gi_thread (void)
{
	triac_leff_allocate (TRIAC_GI_MASK);
	for (;;)
	{
		U8 gi;
		for (gi = (1 << 0); gi <= (1 << 4); gi <<= 1)
		{
			triac_leff_enable (gi);
			task_sleep (TIME_500MS);
			triac_leff_disable (gi);
		}
	}
}

void burnin_flasher_thread (void)
{
	for (;;)
	{
		U8 sol;
		for (sol=0; sol < NUM_POWER_DRIVES; sol++)
		{
			if (MACHINE_SOL_FLASHERP (sol))
			{
				flasher_pulse (sol);
				task_sleep (TIME_200MS);
			}
		}
	}
}

void burnin_sol_thread (void)
{
	for (;;)
	{
		U8 sol;
		for (sol=0; sol < NUM_POWER_DRIVES; sol++)
		{
			if (!MACHINE_SOL_FLASHERP (sol))
			{
				sol_request_async (sol);
				task_sleep_sec (2);
			}
		}
	}
}

void burnin_lamp_thread (void)
{
	SECTION_VOIDCALL (__test__, all_lamp_test_thread);
}

#if (MACHINE_FLIPTRONIC == 1)
void burnin_flipper_thread (void)
{
	for (;;)
	{
		fliptronic_ball_search ();
		task_sleep_sec (8);
	}
}
#endif

void burnin_draw (void)
{
	timestamp_format (&burnin_duration);
	font_render_string_left (&font_mono5, 4, 20, sprintf_buffer);
}

void burnin_timestamp_thread (void)
{
	for (;;)
	{
		timestamp_add_sec (&burnin_duration, 1);
		SECTION_VOIDCALL (__test__, window_redraw);
		task_sleep_sec (1);
	}
}

void burnin_thread (void)
{
	for (;;)
	{
		task_create_peer (burnin_lamp_thread);
		task_create_peer (burnin_sound_thread);
		task_create_peer (burnin_gi_thread);
		task_create_peer (burnin_flasher_thread);
		//task_create_peer (burnin_sol_thread);
		task_create_peer (burnin_timestamp_thread);
#if (MACHINE_FLIPTRONIC == 1)
		task_create_peer (burnin_flipper_thread);
#endif
		task_sleep_sec (60);
		task_kill_peers ();
	}
}

void burnin_init (void)
{
	timestamp_clear (&burnin_duration);
	task_create_gid (GID_WINDOW_THREAD, burnin_thread);
	flipper_enable ();
}

void burnin_exit (void)
{
	timestamp_add (&burnin_total_duration, &burnin_duration);
	lamp_all_off ();
	sound_reset ();
	triac_leff_free (TRIAC_GI_MASK);
	flipper_disable ();
}

CALLSET_ENTRY (burnin, factory_reset)
{
	timestamp_clear (&burnin_total_duration);
}

