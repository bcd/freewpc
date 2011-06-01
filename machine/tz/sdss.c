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

/* CALLSET_SECTION (sdss, __machine4__) */
#include <freewpc.h>

/* Super Duper Skill Shot rules:
 * Hold down right flipper and launch ball straight into slot
 * You then get 4 seconds to shoot the left ramp
 */

extern bool skill_shot_enabled;
bool sdss_enabled;
__local__ U8 sdss_level;

static void sdss_enable (void)
{
	sound_send (SND_LIGHT_SLOT_TIMED);
	sdss_enabled = TRUE;
	//disable_skill_shot (); ??
}

static void sdss_disable (void)
{
	sdss_enabled = FALSE;
	deff_stop (DEFF_SDSS_READY);
	task_kill_gid (GID_SDSS_SWITCH_MONITOR);
	task_kill_gid (GID_SDSS_BUTTON_MONITOR);
}

static void flash_text_deff (U8 flash_count, task_ticks_t flash_delay)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (flash_count, flash_delay);
}

void sdss_ready_deff (void)
{
	sprintf ("SUPER");
	flash_text_deff (10, TIME_33MS);
	sprintf ("DUPER");
	flash_text_deff (10, TIME_33MS);
	sprintf ("SKILL");
	flash_text_deff (10, TIME_33MS);
	sprintf ("SHOT");
	flash_text_deff (10, TIME_33MS);
	deff_exit ();
}

void sdss_awarded_deff (void)
{	
	sprintf ("SUPER");
	flash_text_deff (5, TIME_33MS);
	sprintf ("DUPER");
	flash_text_deff (5, TIME_33MS);
	sprintf ("SKILL");
	flash_text_deff (5, TIME_33MS);
	sprintf ("SHOT");
	flash_text_deff (5, TIME_33MS);
	sprintf ("%d0 MILLION", sdss_level);
	flash_text_deff (10, TIME_200MS);
	deff_exit ();
}

static void sdss_awarded (void)
{
	sdss_disable ();
	task_kill_gid (GID_SDSS_READY);
	bounded_increment (sdss_level, 5);
	score_multiple (SC_10M, sdss_level);
	deff_start (DEFF_SDSS_AWARDED);
}

CALLSET_ENTRY (sdss, left_ramp_exit)
{
	if (task_kill_gid (GID_SDSS_READY))
		sdss_awarded ();
}

CALLSET_ENTRY (sdss, sw_skill_top)
{
	if (sdss_enabled && skill_shot_enabled)
	{
		sdss_enabled = FALSE;
		timer_restart_free (GID_SDSS_READY, TIME_10S);
		task_kill_gid (GID_SDSS_BUTTON_MONITOR);
		task_kill_gid (GID_SDSS_SWITCH_MONITOR);
	}
}

static void sdss_switch_monitor (void)
{
	task_sleep_sec (2);
	sdss_disable ();
	task_exit ();
}

/* Ball is rolling back down, kill sdss */
CALLSET_ENTRY (sdss, sw_skill_center, sw_skill_bottom)
{
	task_recreate_gid (GID_SDSS_SWITCH_MONITOR, sdss_switch_monitor);
}

static void sdss_monitor_button_task (void)
{
	U8 timer = 0;

	while (switch_poll_logical (SW_RIGHT_BUTTON) 
			&& !sdss_enabled && ++timer < 254)
	{
		if (timer == 5)
			sound_send (SND_ROBOT_WALK);
		if (timer == 10)
			sound_send (SND_ROBOT_WALK_2);
		if (timer == 15)
			sdss_enable ();
		task_sleep (TIME_200MS);
	}
	task_exit ();
}

CALLSET_ENTRY (sdss, sw_right_button)
{
	if (skill_shot_enabled && !task_find_gid (GID_SDSS_BUTTON_MONITOR) &&
			!sdss_enabled)
		task_create_gid (GID_SDSS_BUTTON_MONITOR, sdss_monitor_button_task);
}

CALLSET_ENTRY (sdss, start_player)
{
	sdss_level = 0;
}

CALLSET_ENTRY (sdss, start_ball)
{
	sdss_enabled = FALSE;
}
