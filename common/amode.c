/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Common attract mode module
 * This file implements a basic attract mode that can be used by any game.
 * Machines can override this with their own amode if desired.
 */

bool amode_page_delay (U8 secs)
{
	U8 amode_flippers;
	U8 amode_flippers_start;

	/* Convert secs to 66ms ticks */
	secs <<= 4;

	amode_flippers_start = switch_poll_logical (SW_LL_FLIP_SW);
	while (secs != 0)
	{
		task_sleep (TIME_66MS);
		amode_flippers = switch_poll_logical (SW_LL_FLIP_SW);

		if ((amode_flippers != amode_flippers_start) &&
			 (amode_flippers != 0))
		{
			return TRUE;
		}
		amode_flippers_start = amode_flippers;
		secs--;
	}
	return FALSE;
}


static void amode_flipper_sound_debounce_timer (void)
{
	task_sleep_sec (10);
	task_sleep_sec (10);
	task_sleep_sec (10);
	task_exit ();
}


static void amode_flipper_sound (void)
{
	if (!task_find_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE))
	{
		task_create_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE,
			amode_flipper_sound_debounce_timer);
		sound_send (SND_THUD);
	}
}


void amode_left_flipper (void)
{
	amode_flipper_sound ();
}


void amode_right_flipper (void)
{
	amode_flipper_sound ();
}


void amode_deff (void)
{
#ifdef MACHINE_AMODE_INIT
	MACHINE_AMODE_INIT ();
#endif
	for (;;)
	{
		/** Display last set of player scores **/
		dmd_alloc_low_clean ();
		scores_draw ();
		dmd_show_low ();
		if (amode_page_delay (5) && system_config.tournament_mode)
			continue;

		/** Display FreeWPC logo **/
		dmd_alloc_low_high ();
		dmd_draw_fif (fif_freewpc);
		dmd_sched_transition (&trans_random_boxfade);
		dmd_show2 ();
		if (amode_page_delay (5) && system_config.tournament_mode)
			continue;

		/** Display credits message **/
		credits_draw ();

#ifdef MACHINE_AMODE_HOOK_1
		MACHINE_AMODE_HOOK_1 ();
#endif

		/** Display high scores **/
		high_score_amode_show ();

#ifdef MACHINE_AMODE_HOOK_2
		MACHINE_AMODE_HOOK_2 ();
#endif

		/* Display date/time */
		rtc_show_date_time ();
		if (amode_page_delay (5) && system_config.tournament_mode)
			continue;

		/* Kill music if it is running */
		music_set (MUS_OFF);

#ifdef MACHINE_AMODE_HOOK_3
		MACHINE_AMODE_HOOK_3 ();
#endif
	}
}

