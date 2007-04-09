/*
 * Copyright 2007 by Brian Dominy <brian@oddchange.com>
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

extern U8 last_nonfatal_error_code;
extern task_gid_t last_nonfatal_error_gid;
extern __nvram__ U8 current_volume;
extern const audio_track_t volume_change_music_track;

/** Display effect when locating missing balls prior to game start */
void locating_balls_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 11, "LOCATING BALLS");
	font_render_string_center (&font_fixed6, 64, 22, "PLEASE WAIT...");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


/** The display effect that appears immediately at the end of a game,
before returning to attract mode */
void game_over_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "GAME OVER");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}


void volume_change_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("VOLUME %d", current_volume);
	font_render_string_center (&font_fixed6, 64, 13, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (5);
	bg_music_stop (&volume_change_music_track);
	deff_exit ();
}

void nonfatal_error_deff (void)
{
#ifdef DEBUGGER
	dmd_alloc_low_clean ();
	sprintf ("NONFATAL %ld", system_audits.non_fatal_errors);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	sprintf ("ERRNO %i GID %i", last_nonfatal_error_code, last_nonfatal_error_gid);
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);
	dmd_show_low ();
	sound_send (SND_TEST_ALERT);
	task_sleep (TIME_200MS);
	sound_send (SND_TEST_ALERT);
	task_sleep (TIME_200MS);
	sound_send (SND_TEST_ALERT);
	task_sleep_sec (4);
#endif
	deff_exit ();
}

