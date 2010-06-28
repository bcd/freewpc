/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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


U8 clock_millions_mode_timer;
__local__ U8 clock_mode_hits;

__local__ U8 clock_default_hits;

U8 clock_millions_timer;

void clock_millions_mode_init (void);
void clock_millions_mode_exit (void);

struct timed_mode_ops clock_millions_mode = {
	DEFAULT_MODE,
	.init = clock_millions_mode_init,
	.exit = clock_millions_mode_exit,
	.gid = GID_CLOCK_MILLIONS_MODE_RUNNING,
	.music = MUS_TOWN_SQUARE_MADNESS,
	.deff_running = DEFF_CLOCK_MILLIONS_MODE,
	.prio = PRI_GAME_MODE1,
	.init_timer = 20,
	.timer = &clock_millions_mode_timer,
	.grace_timer = 3,
	.pause = system_timer_pause,
};

void clock_millions_hit_deff (void)
{
	generic_deff ("CLOCK MILLIONS", "5,000,000");
}

void clock_default_hit_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("CLOCK HIT %d", clock_default_hits);
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, 21, "50,000");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void clock_millions_mode_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_var5, 64, 5, "CLOCK MILLIONS");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		font_render_string_center (&font_var5, 64, 27, "SHOOT CLOCK");
		sprintf ("%d", clock_millions_mode_timer);
		font_render_string (&font_var5, 2, 2, sprintf_buffer);
		font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}

CALLSET_ENTRY (clock_millions, sw_clock_target)
{
	if (!lamp_flash_test (LM_CLOCK_MILLIONS))
	{
		callset_invoke (sw_jet_noflash);
		score (SC_50K);
		sound_send (SND_NO_CREDITS);
	}
	else
	{
		sound_send (SND_CLOCK_BELL);
		leff_start (LEFF_CLOCK_TARGET);
		deff_start (DEFF_CLOCK_MILLIONS_HIT);
		score (SC_5M);
	
		if (clock_mode_hits <= 4)
		{
			clock_mode_hits++;
		}
	}
}
void clock_millions_mode_init (void)
{
	lamp_tristate_flash (LM_CLOCK_MILLIONS);
}

void clock_millions_mode_expire (void)
{
	lamp_tristate_off (LM_CLOCK_MILLIONS);
}

void clock_millions_mode_exit (void)
{
	lamp_tristate_off (LM_CLOCK_MILLIONS);
}

CALLSET_ENTRY (clock_millions, start_ball)
{
	lamp_tristate_off (LM_CLOCK_MILLIONS);
}

CALLSET_ENTRY (clock_millions, end_ball)
{
	timed_mode_end (&clock_millions_mode);
}

CALLSET_ENTRY (clock_millions, display_update)
{
	timed_mode_display_update (&clock_millions_mode);
}

CALLSET_ENTRY (clock_millions, music_refresh)
{
	timed_mode_music_refresh (&clock_millions_mode);
}

CALLSET_ENTRY (clock_millions, door_start_clock_millions)
{
	timed_mode_begin (&clock_millions_mode);
}

