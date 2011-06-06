/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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

U8 fastlock_mode_timer;
U8 fastlock_award;
/* Stored for Deff */
U8 fastlock_award_stored;
U8 fastlocks_collected;
U8 display_loop_time;

void fastlock_mode_init (void);
void fastlock_mode_exit (void);

extern U8 loop_time;

struct timed_mode_ops fastlock_mode = {
	DEFAULT_MODE,
	.init = fastlock_mode_init,
	.exit = fastlock_mode_exit,
	.gid = GID_FASTLOCK_MODE_RUNNING,
	.music = MUS_FASTLOCK_ADDAMS_FAMILY,
	.deff_running = DEFF_FASTLOCK_MODE,
	.prio = PRI_GAME_MODE5,
	.init_timer = 40,
	.timer = &fastlock_mode_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

/* Fudge loop time in ms into
 * something semi-meaningful to display */
static inline void calc_display_loop_time (void)
{
	display_loop_time = 100;
	display_loop_time -= loop_time;
	if (display_loop_time < 1)
		display_loop_time = 1;
}


void fastlock_mode_deff (void)
{
	U16 fno;

	for (;;)
	{
		for (fno = IMG_FASTLOCK_START; fno <= IMG_FASTLOCK_END; fno += 2)
		{
			dmd_alloc_pair_clean ();
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_var5, 64, 5, "FASTLOOPS BUILD JACKPOT");
			if (timer_find_gid (GID_FASTLOCK_LOOP_AWARDED))
			{
				calc_display_loop_time ();
				sprintf ("%d MPH", display_loop_time);
				font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
				font_render_string_center (&font_var5, 64, 27, "LAST LOOP");
			}
			else
			{	
				sprintf("%d MILLION", fastlock_award);
				font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
				font_render_string_center (&font_var5, 64, 27, "SHOOT LOCK TO COLLECT");
			}
			sprintf ("%d", fastlock_mode_timer);
			font_render_string (&font_var5, 2, 16, sprintf_buffer);
			font_render_string_right (&font_var5, 126, 16, sprintf_buffer);
			dmd_text_outline ();
			
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
}

void fastlock_award_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 5, "FASTLOCK JACKPOT");
	sprintf("%d MILLION", fastlock_award_stored);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void fastlock_mode_init (void)
{
	fastlock_award = 5;
	fastlocks_collected = 1;
	magnet_reset ();
}

void fastlock_mode_expire (void)
{
	if (fastlocks_collected == 1)
		callset_invoke (start_hurryup);
}

void fastlock_mode_exit (void)
{
	lamp_off (LM_LOCK_ARROW);
}

bool fastlock_running (void)
{
	if (timed_mode_running_p (&fastlock_mode))
		return TRUE;
	else
		return FALSE;
}

void fastlock_award_task (void)
{
	fastlock_award_stored = fastlock_award;
	deff_start_sync (DEFF_FASTLOCK_AWARD);
	score_multiple (SC_1M, fastlock_award);
	fastlocks_collected++;
	fastlock_award = (fastlocks_collected * 5);
	timed_mode_add (&fastlock_mode, 20);
	task_exit ();
}

void fastlock_lock_entered (void)
{
	if (fastlock_running ())
		task_create_anon (fastlock_award_task);
}

void fastlock_loop_completed (void)
{
	if (!fastlock_running ())
		return;

	if (loop_time < 40)
	{
		fastlock_award += 10;
		sound_send (SND_CRASH);
	}
	else if (loop_time < 60)
	{
		fastlock_award += 5;
		sound_send (SND_FIVE);
	}
	else
	{
		fastlock_award += 1;
		sound_send (SND_ONE);
	}
}

CALLSET_ENTRY (fastlock, lamp_update)
{
	if (fastlock_running ())
		lamp_tristate_flash (LM_LOCK_ARROW);
}

CALLSET_ENTRY (fastlock, end_ball)
{
	timed_mode_end (&fastlock_mode);
}

CALLSET_ENTRY (fastlock, display_update)
{
	timed_mode_display_update (&fastlock_mode);
}

CALLSET_ENTRY (fastlock, music_refresh)
{
	timed_mode_music_refresh (&fastlock_mode);
}

CALLSET_ENTRY (fastlock, door_start_fast_lock)
{
	timed_mode_begin (&fastlock_mode);
}

