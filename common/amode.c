/*
 * Copyright 2006, 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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
#include <coin.h>
#include <highscore.h>
#include <replay.h>

/**
 * \file
 * \brief Common attract mode module
 * This file implements a basic attract mode that can be used by any game.
 */

U8 amode_page;

U8 amode_page_changed;

void amode_page_change (S8 delta);


void system_amode_leff (void)
{
	for (;;)
		task_sleep_sec (1);
}


void amode_flipper_sound_debounce_timer (void)
{
	task_sleep_sec (30);
	task_exit ();
}


void amode_flipper_sound (void)
{
	if (!task_find_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE))
	{
		task_create_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE,
			amode_flipper_sound_debounce_timer);
#ifdef MACHINE_AMODE_FLIPPER_SOUND_CODE
		sound_send (MACHINE_AMODE_FLIPPER_SOUND_CODE);
#endif
	}
}


void amode_sleep_sec (U8 secs)
{
	if (secs > 0)
	{
		amode_page_changed = 0;
		while (secs > 0)
		{
			task_sleep (TIME_250MS);
			if (amode_page_changed)
				return;
			task_sleep (TIME_250MS);
			if (amode_page_changed)
				return;
			task_sleep (TIME_250MS);
			if (amode_page_changed)
				return;
			task_sleep (TIME_250MS);
			if (amode_page_changed)
				return;
			secs--;
		}
	}
}



void amode_page_end (U8 secs)
{
	amode_sleep_sec (secs);
	amode_page_change (1);
}


void amode_score_page (void)
{
	dmd_alloc_low_clean ();
	scores_draw ();
	dmd_show_low ();
	amode_page_end (5);
}


#if (MACHINE_DMD == 1)
void amode_logo_page (void)
{
	U8 fno;

	for (fno = IMG_FREEWPC_SMALL; fno <= IMG_FREEWPC; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	amode_page_end (3);
}
#endif

void amode_credits_page (void)
{
	credits_draw ();
	dmd_show_low ();
	amode_page_end (3);
}

void amode_freeplay_page (void)
{
	if (system_config.replay_award != FREE_AWARD_OFF)
	{
		replay_draw ();
		amode_sleep_sec (3);
	}
	amode_page_end (0);
}

void amode_high_score_page (void)
{
	if (hstd_config.highest_scores == ON)
	{
		high_score_draw_gc ();
		amode_sleep_sec (3);
		high_score_draw_12 ();
		amode_sleep_sec (3);
		high_score_draw_34 ();
		amode_sleep_sec (3);
	}
	amode_page_end (0);
}

void amode_date_time_page (void)
{
	if (system_config.show_date_and_time == YES)
	{
		rtc_show_date_time ();
		amode_page_end (3);
	}
}

void amode_kill_music (void)
{
	music_set (MUS_OFF);
	amode_page_end (0);
}


#ifdef MACHINE_TZ
void amode_tz_page (void)
{
	U8 n;

	dmd_map_lookaside (0);
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 22, "THE ZONE");
	dmd_text_blur ();
	font_render_string_center (&font_fixed6, 64, 7, "BACK TO");

	for (n = 0; n < 40; n++)
	{
		dmd_dup_mapped ();
		dmd_overlay (wpc_dmd_get_mapped (), 0);
		star_draw ();
		dmd_show2 ();
		task_sleep (TIME_100MS);
		dmd_map_lookaside (0);
	}
	amode_page_end (0);
}
#endif


void (*amode_page_table[]) (void) = {
	amode_score_page,
#if (MACHINE_DMD == 1)
	amode_logo_page,
#endif
#ifdef MACHINE_TZ
	amode_tz_page,
#endif
	amode_credits_page,
	amode_freeplay_page,
	amode_high_score_page,
	amode_date_time_page,
	amode_kill_music,
#ifdef MACHINE_AMODE_EFFECTS
	MACHINE_AMODE_EFFECTS
#endif
};


__attribute__((noinline)) void amode_page_change (S8 delta)
{
	for (;;)
	{
		amode_page += delta;

		/* Check for boundary cases */
		if (amode_page >= 0xF0)
		{
			amode_page = (sizeof (amode_page_table) / sizeof (void *)) - 1;
		}
		else if (amode_page >= sizeof (amode_page_table) / sizeof (void *))
		{
			amode_page = 0;
		}

		/* Check for pages that should be skipped */

		/* All done */
		break;
	}
	amode_page_changed = 1;
}


CALLSET_ENTRY (amode, sw_left_button)
{
	if (deff_get_active () == DEFF_AMODE)
	{
		amode_flipper_sound ();
		if (amode_page_changed == 0)
			amode_page_change (-1);
	}
}


CALLSET_ENTRY (amode, sw_right_button)
{
	if (deff_get_active () == DEFF_AMODE)
	{
		amode_flipper_sound ();
		if (amode_page_changed == 0)
			amode_page_change (1);
	}
}


__attribute__((noreturn)) void system_amode_deff (void)
{
	if (system_config.tournament_mode == YES)
	{
		/* Hold the scores up for a while longer than usual
		 * in tournament mode. */
	}

	/* When amode is started, diagnostic are also being re-run.  Give that
	some time to finish, so that the score screen will show the credit
	dot correctly. */
	task_sleep (TIME_100MS);

	amode_page = 0;
	for (;;)
	{
		if (amode_page == 1)
			callset_invoke (amode_page);
		amode_page_table[amode_page] ();
	}
}

