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

time_audit_t burnin_duration;

__nvram__ time_audit_t burnin_total_duration;

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

#ifdef CONFIG_GI
void burnin_gi_thread (void)
{
	for (;;)
	{
		U8 gi;
		for (gi = (1 << 0); gi <= (1 << 4); gi <<= 1)
		{
			gi_enable (gi);
			task_sleep (TIME_500MS);
			gi_disable (gi);
		}
	}
}
#endif

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
		task_sleep (TIME_500MS);
	}
}


void burnin_lamp_thread (void)
{
#ifdef CONFIG_TEST
	SECTION_VOIDCALL (__test__, all_lamp_test_thread);
#else
/* TODO */
#endif
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

#ifdef CONFIG_DMD_OR_ALPHA
void burnin_draw (void)
{
	time_audit_format (&burnin_duration);
	font_render_string_left (&font_mono5, 4, 20, sprintf_buffer);
}
#endif

void burnin_time_audit_thread (void)
{
	for (;;)
	{
		time_audit_add_sec (&burnin_duration, 1);
#ifdef CONFIG_DMD_OR_ALPHA
		SECTION_VOIDCALL (__test__, window_redraw);
#endif
		task_sleep_sec (1);
	}
}

void burnin_thread (void)
{
	for (;;)
	{
		task_create_peer (burnin_lamp_thread);
		task_create_peer (burnin_sound_thread);
#ifdef CONFIG_GI
		task_create_peer (burnin_gi_thread);
#endif
		task_create_peer (burnin_flasher_thread);
		task_create_peer (burnin_time_audit_thread);
#if (MACHINE_FLIPTRONIC == 1)
		task_create_peer (burnin_flipper_thread);
#endif
		task_sleep_sec (60);
		task_kill_peers ();
	}
}

void burnin_init (void)
{
	time_audit_clear (&burnin_duration);
	task_create_gid (GID_WINDOW_THREAD, burnin_thread);
	flipper_enable ();
}

void burnin_exit (void)
{
	time_audit_add (&burnin_total_duration, &burnin_duration);
	lamp_all_off ();
	sound_reset ();
#ifdef CONFIG_GI
	gi_leff_free (PINIO_GI_STRINGS);
#endif
	flipper_disable ();
}

CALLSET_ENTRY (burnin, factory_reset)
{
	time_audit_clear (&burnin_total_duration);
}

