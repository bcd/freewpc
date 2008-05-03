/*
 * Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
#include <animation.h>
#include <highscore.h>


extern U8 last_nonfatal_error_code;
extern task_gid_t last_nonfatal_error_gid;
extern __nvram__ U8 current_volume;
extern const audio_track_t volume_change_music_track;


/** Display effect when locating missing balls prior to game start */
void locating_balls_deff (void)
{
	generic_deff ("LOCATING BALLS", "PLEASE WAIT...");
}


/** The display effect that appears immediately at the end of a game,
before returning to attract mode */
void game_over_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "GAME OVER");
	dmd_show_low ();
	task_sleep_sec (2);

	/* In tournament mode, need to show the scores briefly here */
	if (tournament_mode_enabled)
	{
		dmd_alloc_low_clean ();
		scores_draw ();
		dmd_show_low ();
		task_sleep_sec (60);
		deff_exit ();
	}
	deff_exit ();
}


void draw_volume_bar (U8 n)
{
	U8 *base = dmd_low_buffer + 22 * DMD_BYTE_WIDTH + 4;
	U8 val = 0x55;
	static const U8 volume_bar_data[] = { 0x0, 0x1, 0x5, 0x15, 0x55 };
	while (n >= 4)
	{
		base[0] = base[1 * DMD_BYTE_WIDTH] = base[2 * DMD_BYTE_WIDTH] = val;
		base++;
		n -= 4;
	}
	val = volume_bar_data[n];
	base[0] = base[1 * DMD_BYTE_WIDTH] = base[2 * DMD_BYTE_WIDTH] = val;
}


/**uThe display effect for a volume change */
void volume_change_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("VOLUME %d", current_volume);
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	draw_volume_bar (current_volume);
	dmd_show_low ();
	if (in_live_game)
		task_sleep_sec (3);
	else
		task_sleep_sec (5);
	music_stop (volume_change_music_track);
	deff_exit ();
}


/** The display effect for a non-fatal error */
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


/** The display effect for the final ball goal */
void score_goal_deff (void)
{
	/* Show the replay if it is enabled and hasn't been awarded yet. */
	if (replay_can_be_awarded ())
	{
		replay_draw ();
	}
	else
	{
		grand_champion_draw ();
	}
	task_sleep_sec (2);
	deff_exit ();
}


void animation_test1 (struct animation_object *obj)
{
	U8 *x = &obj->data.u8;
	sprintf ("TOP %d", *x);
	font_render_string_center (&font_var5, 64, 4, sprintf_buffer);
	(*x) ++;
}

void animation_test2 (struct animation_object *obj)
{
	U8 *x = &obj->data.u8;
	sprintf ("MIDDLE %d", *x);
	font_render_string_center (&font_var5, 64, 12, sprintf_buffer);
	(*x) += 2;
}

void animation_test3 (struct animation_object *obj)
{
	U8 *x = &obj->data.u8;
	sprintf ("BOTTOM %d", *x);
	font_render_string_center (&font_var5, 64, 20, sprintf_buffer);
	(*x) += 3;
}

void animation_test_deff (void)
{
	U8 n;
	animation_begin (AN_MONO+AN_CLEAN);
	animation_set_speed (TIME_66MS);
	animation_add_static (animation_test1);
	animation_add_static (animation_test2);
	animation_add_static (animation_test3);
	for (n=0; n < 100; n++)
		animation_step ();
	animation_end ();
	deff_exit ();
}

