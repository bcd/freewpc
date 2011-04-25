/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (mball_restart, __machine2__) */
#include <freewpc.h>

U8 mball_restart_timer;

void mball_restart_mode_init (void);
void mball_restart_mode_exit (void);

struct timed_mode_ops mball_restart_mode = {
	DEFAULT_MODE,
	.init = mball_restart_mode_init,
	.exit = mball_restart_mode_exit,
	.gid = GID_MBALL_RESTART_MODE,
	.music = MUS_FASTLOCK_COUNTDOWN,
	.deff_running = NULL,
	.prio = PRI_MULTIBALL,
	.init_timer = 15,
	.timer = &mball_restart_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

void mball_restart_mode_init (void)
{
	callset_invoke (stop_hurryup);
}

void mball_restart_mode_exit (void)
{
}

static void call_number (U8 number)
{
	switch (number)
	{
		default:
		case 5:
			sound_send (SND_FIVE);
			break;
		case 4:
			sound_send (SND_FOUR);
			break;
		case 3:
			sound_send (SND_THREE);
			break;
		case 2:
			sound_send (SND_TWO);
			break;
		case 1:
			sound_send (SND_ONE);
			break;
	}
}

void mball_restart_countdown_task (void)
{
	U8 last_number_called = 6;
	while (mball_restart_timer != 0)
	{
		if (last_number_called != mball_restart_timer)
		{
			last_number_called = mball_restart_timer;
			call_number (last_number_called);	
		}
		task_sleep (TIME_800MS);
	}
	task_exit ();
}

void mball_restart_deff (void)
{
	U16 fno;
	U8 j = 0;
	dmd_alloc_pair_clean ();
	while (mball_restart_timer > 0)
	{
		for (fno = IMG_BOLT_TESLA_START; fno < IMG_BOLT_TESLA_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
			font_render_string_center (&font_var5, 64, 16, "SHOOT LOCK TO RESTART");
			j++;
			if (j > 255)
				j = 0;
			if (j % 2 != 0)
			{
			font_render_string_center (&font_fixed6, 64, 4, "MULTIBALL");
			}
			sprintf ("%d", mball_restart_timer);
			font_render_string_center (&font_fixed6, 64, 25, sprintf_buffer);
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
}

CALLSET_ENTRY (mball_restart, idle_every_second)
{
	if (timed_mode_running_p (&mball_restart_mode)
			&& mball_restart_timer == 5)
		task_create_gid (GID_MBALL_RESTART_MODE, mball_restart_countdown_task);
}

CALLSET_ENTRY (mball_restart, mball_restart_stop)
{
	if (timed_mode_running_p (&mball_restart_mode))
		timed_mode_end (&mball_restart_mode);
}

CALLSET_ENTRY (mball_restart, mball_restart_start)
{
	timed_mode_begin (&mball_restart_mode);
}
