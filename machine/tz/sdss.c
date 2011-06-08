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
 * If sucessfull, shoot the right ramp to get another 10M * Level
 */

/* GID_SDSS_APPROACHING refers to the time between the skill shot and the slot
 * GID_SDSS_READY refers to when the SDSS is ready to be collected
 * GID_USDSS_APPROACHING refers to when the ball is travelling from the leftramp
 * to the inlane
 */
extern bool skill_shot_enabled;
bool sdss_enabled;
__local__ U8 sdss_level;
score_t sdss_score;

static void sdss_enable (void)
{
	score_zero (sdss_score);
	sound_send (SND_LIGHT_SLOT_TIMED);
	sdss_enabled = TRUE;
}

static void sdss_disable (void)
{
	sdss_enabled = FALSE;
	task_kill_gid (GID_SDSS_READY);
	task_kill_gid (GID_SDSS_BUTTON_MONITOR);
	lamp_tristate_off (LM_SUPER_SKILL);
}

static void flash_text_deff (U8 flash_count, task_ticks_t flash_delay)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fireball, 64, 16, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (flash_count, flash_delay);
}
static void flash_score_deff (U8 flash_count, task_ticks_t flash_delay)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_quadrit, 64, 16, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (flash_count, flash_delay);
}
void sdss_ready_deff (void)
{
	sound_send (SND_SUPER_SKILL);
	sprintf ("SUPER");
	flash_text_deff (5, TIME_33MS);
	sprintf ("DUPER");
	flash_text_deff (5, TIME_33MS);
	sound_send (SND_SUPER_SKILL);
	sprintf ("SKILL");
	flash_text_deff (10, TIME_33MS);
	sprintf ("SHOT");
	flash_text_deff (10, TIME_33MS);
	sprintf ("LEVEL %d", sdss_level + 1);
	flash_text_deff (15, TIME_33MS);
	deff_exit ();
}

void usdss_awarded_deff (void)
{	
	sound_send (SND_FAST_LOCK_STARTED);
	sound_send (SND_YES);
	sprintf ("ULTRA");
	flash_text_deff (7, TIME_100MS);
	
	sound_send (SND_YES);
	sprintf ("SUPER");
	flash_text_deff (5, TIME_66MS);
	
	sprintf ("DUPER");
	flash_text_deff (5, TIME_66MS);
	
	sound_send (SND_YES);
	sprintf ("SKILL");
	flash_text_deff (5, TIME_66MS);
	
	sprintf ("SHOT");
	flash_text_deff (5, TIME_66MS);
	
	sprintf_score (sdss_score);
	sound_send (SND_EXPLOSION_2);
	flash_score_deff (10, TIME_200MS);
	deff_exit ();
}


void sdss_awarded_deff (void)
{	
	sound_send (SND_FAST_LOCK_STARTED);
	sound_send (SND_EXPLOSION_1);
	sprintf ("SUPER DUPER");
	flash_text_deff (5, TIME_66MS);
	
	sound_send (SND_EXPLOSION_1);
	sprintf ("SKILL SHOT");
	flash_text_deff (5, TIME_66MS);
	
	sound_send (SND_EXPLOSION_2);
	sprintf_score (sdss_score);
	flash_score_deff (10, TIME_200MS);
	deff_exit ();
}

static void score_sdss (void)
{
	U8 i;
	for (i = 0; i < sdss_level; i++)
	{
		score (SC_10M);
		score_add (sdss_score, score_table[SC_10M]);
	}
}

static void sdss_awarded (void)
{
	sdss_disable ();
	bounded_increment (sdss_level, 5);
	score_sdss ();
	deff_start (DEFF_SDSS_AWARDED);
}

static void usdss_awarded (void)
{
	task_kill_gid (GID_USDSS_APPROACHING);
	task_kill_gid (GID_USDSS_READY);
	timer_restart_free (GID_USDSS_AWARDED, TIME_4S);
	score_sdss ();
	leff_start (LEFF_PIANO_JACKPOT_COLLECTED);
	deff_start (DEFF_USDSS_AWARDED);
}

CALLSET_ENTRY (sdss, sw_left_inlane_2)
{
	if (task_kill_gid (GID_USDSS_APPROACHING))
		timer_restart_free (GID_USDSS_READY, TIME_5S);
}

CALLSET_ENTRY (sdss, left_ramp_exit)
{
	if (task_kill_gid (GID_SDSS_READY))
	{
		timer_restart_free (GID_USDSS_APPROACHING, TIME_5S);
		sdss_awarded ();
	}
}

CALLSET_ENTRY (sdss, sw_right_ramp)
{
	sdss_disable ();
	if (task_kill_gid (GID_USDSS_READY))
		usdss_awarded ();
}

static void sdss_ready_task (void)
{
	lamp_tristate_flash (LM_SUPER_SKILL);
	deff_start (DEFF_SDSS_READY);
	/* Wait for ten seconds and then disable */
	task_sleep_sec (10);
	sdss_disable ();
	task_exit ();
}

/* called from slot.c */
CALLSET_ENTRY (sdss, sdss_ready)
{
	task_create_gid (GID_SDSS_READY, sdss_ready_task);
}

CALLSET_ENTRY (sdss, sw_skill_top)
{
	if (sdss_enabled && skill_shot_enabled)
	{
		//sdss_enabled = FALSE;
		timer_restart_free (GID_SDSS_APPROACHING, TIME_4S);
		task_kill_gid (GID_SDSS_BUTTON_MONITOR);
	}
}

/* Ball is rolling back down, kill sdss */
CALLSET_ENTRY (sdss, sw_skill_center, sw_skill_bottom)
{
	if (task_kill_gid (GID_SDSS_APPROACHING))
		sdss_disable ();
}

static bool sdss_ready_to_enable (void)
{
	if (skill_shot_enabled && !task_find_gid (GID_SKILL_SWITCH_TRIGGER) 
		&& !sdss_enabled
		&& in_live_game
		&& !task_find_gid (GID_SDSS_LEFT_BUTTON))
		return TRUE;
	else
		return FALSE;
}

static void sdss_monitor_right_button_task (void)
{
	U8 timer = 0;

	while (switch_poll_logical (SW_RIGHT_BUTTON)
			&& sdss_ready_to_enable ()
			&& ++timer < 254)
	{
		switch (timer)
		{
			case 5:
				sound_send (SND_ROBOT_WALK);
				break;
			case 10:
				sound_send (SND_ROBOT_WALK_2);
				break;
			case 15:
				sound_send (SND_ROBOT_WALK);
				timer_restart_free (GID_SDSS_LEFT_BUTTON, TIME_2S);
				break;
			default:
				break;
		}
		task_sleep (TIME_200MS);
	}
	task_exit ();
}

static void sdss_monitor_left_button_task (void)
{
	U8 timer = 0;
	while (++timer < 254 && sdss_ready_to_enable ()
			&& switch_poll_logical (SW_LEFT_BUTTON)
			&& !switch_poll_logical (SW_RIGHT_BUTTON))
	{
		switch (timer)
		{
			case 6:
				sound_send (SND_ROBOT_WALK);
				break;
			case 1:
			case 11:
				sound_send (SND_ROBOT_WALK_2);
				break;
			case 16:
				sdss_enable ();
				break;
			default:
				break;
		}
		task_sleep (TIME_200MS);
	}
	task_exit ();
}

CALLSET_ENTRY (sdss, sw_right_button)
{
	if (skill_shot_enabled && !task_find_gid (GID_SDSS_BUTTON_MONITOR) 
			&& !task_find_gid (GID_SDSS_READY)
			&& !sdss_enabled)
		task_create_gid (GID_SDSS_BUTTON_MONITOR, sdss_monitor_right_button_task);
}

CALLSET_ENTRY (sdss, sw_left_button)
{
	if (task_kill_gid (GID_SDSS_LEFT_BUTTON))
		task_create_gid (GID_SDSS_BUTTON_MONITOR, sdss_monitor_left_button_task);

}

CALLSET_ENTRY (sdss, start_player)
{
	sdss_level = 0;
}

CALLSET_ENTRY (sdss, start_ball)
{
	sdss_enabled = FALSE;
}
