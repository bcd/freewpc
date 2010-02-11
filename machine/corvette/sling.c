/*
 * Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
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

__local__ U8 sling_sound;
__local__ U8 sling_goal;
__local__ U8 sling_count;
//__local__ U8 super_slings;

void sling_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 96, 5, "BABES");
	sprintf ("%d", sling_count);
	font_render_string_center (&font_fixed10, 96, 16, sprintf_buffer);

	if (sling_count == sling_goal)
		sprintf ("BONUS AWARDED");
	else
		sprintf ("BONUS AT %d", sling_goal);

	font_render_string_center (&font_var5, 64, 26, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void sling_flasher (void)
{
	flasher_pulse (FLASH_CATCH_ME);
	task_sleep (TIME_500MS);
	task_exit ();
}

#define MAX_SLING_GOAL 50

void sling_goal_award( void ) {

	if (sling_goal < MAX_SLING_GOAL) {
		sample_start(SND_AWARD_01, SL_4S);
		score (SC_1M);
	} else {
		sample_start(SND_DITTY_04, SL_4S);
		score (SC_5M);
	}

	sling_goal += 5;
	if (sling_goal > MAX_SLING_GOAL) {
		sling_goal = MAX_SLING_GOAL; // Don't allow goal over 25
	}
}

void sling_goal_reset( void ) {
	sling_count = 0;
	sling_goal = 10;
}

CALLSET_ENTRY (slings, start_ball)
{
	sling_sound = 0;
	sling_goal_reset();
}

CALLSET_ENTRY (slings, sw_left_slingshot, sw_right_slingshot)
{
	sling_count++;
	score (SC_1K);
	deff_start (DEFF_SLING);
	task_sleep (TIME_16MS);
	task_create_gid1 (GID_SLING_FLASHER, sling_flasher);
	if (sling_count == sling_goal) {
		sling_count = 0;
		sling_goal_award();
		return;
	}

	sling_sound++;
	if (sling_sound > 2) {
		sling_sound = 0;
	}
	switch (sling_sound) {
		case 0:
			sample_start (SND_GUITAR_01, SL_100MS);
		break;
		case 1:
			sample_start (SND_GUITAR_02, SL_100MS);
		break;
		case 2:
			sample_start (SND_GUITAR_03, SL_100MS);
		break;
	}
}
