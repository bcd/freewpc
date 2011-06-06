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

/* CALLSET_SECTION (spiral, __machine3__) */

#include <freewpc.h>


U8 spiral_mode_timer;
score_t spiral_mode_total;

U8 spiral_loops;

void spiral_mode_init (void);
void spiral_mode_exit (void);

struct timed_mode_ops spiral_mode = {
	DEFAULT_MODE,
	.init = spiral_mode_init,
	.exit = spiral_mode_exit,
	.gid = GID_SPIRAL_MODE_RUNNING,
	.music = MUS_SPIRAL_MODE,
	.deff_running = DEFF_SPIRAL_MODE,
	.deff_ending = DEFF_SPIRAL_MODE_TOTAL,
	.prio = PRI_GAME_MODE3,
	.init_timer = 30,
	.timer = &spiral_mode_timer,
	.grace_timer = 0,
	.pause = system_timer_pause,
};

void spiral_loop_deff (void)
{
	U16 fno;
	U8 i;
	dmd_alloc_pair_clean ();
	for (i=0; i < 3; i++)
	{
		for (fno = IMG_PINWHEEL_END; fno >= IMG_PINWHEEL_START; fno -= 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
	
			psprintf ("1 SPIRAL", "%d SPIRALS", spiral_loops);
			font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
			sprintf_score (spiral_mode_total);	
			font_render_string_center (&font_fixed6, 64, 18, sprintf_buffer);
			sprintf ("MODE TOTAL");
			font_render_string_center (&font_var5, 64, 26, sprintf_buffer);
			dmd_text_outline ();
			
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_33MS);
		}
	}
	deff_exit ();
}

CALLSET_ENTRY (spiral, award_spiral_loop)
{
	if (!timed_mode_running_p (&spiral_mode))
		return;
	bounded_increment (spiral_loops, 50);
	/* Score it */
	if (spiral_loops < 3)
	{
		sound_send (SND_SPIRAL_AWARDED);
		score (SC_10M);
		score_add (spiral_mode_total, score_table[SC_10M]);
	}
	else
	{
		sound_send (SND_SPIRAL_BREAKTHRU);
		score (SC_20M);
		score_add (spiral_mode_total, score_table[SC_20M]);
	}
	deff_start (DEFF_SPIRAL_LOOP);
}

void spiral_mode_deff (void)
{
	U16 fno;
	dmd_alloc_pair_clean ();
//	while (spiral_mode_timer > 0)
	for (;;)
	{
		for (fno = IMG_PINWHEEL_START; fno <= IMG_PINWHEEL_END; fno += 2)
		{
			dmd_map_overlay ();
			dmd_clean_page_low ();
		
			font_render_string_center (&font_fixed6, 64, 5, "SPIRAL");
			sprintf_current_score ();
			font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
			font_render_string_center (&font_var5, 64, 27, "SHOOT LOOPS FOR BIG POINTS");
			sprintf ("%d", spiral_mode_timer);
			font_render_string (&font_var5, 2, 2, sprintf_buffer);
			font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
			
			dmd_text_outline ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_overlay_outline ();
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
}

void spiral_mode_total_deff (void)
{
	dmd_alloc_pair_clean ();
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 64, 5, "SPIRAL OVER");
	sprintf_score (spiral_mode_total);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 27, "POINTS EARNED FROM MODE");
	dmd_text_outline ();
	dmd_alloc_pair ();
	frame_draw (IMG_PINWHEEL_START);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep_sec (4);
	deff_exit ();
}


void spiral_mode_init (void)
{
	lamp_tristate_flash (LM_RIGHT_SPIRAL);
	lamp_tristate_flash (LM_LEFT_SPIRAL);
	score_zero (spiral_mode_total);	
	magnet_reset ();
}

void spiral_mode_expire (void)
{
	if (spiral_loops == 0)
		callset_invoke (start_hurryup);
}

void spiral_mode_exit (void)
{
	lamp_tristate_off (LM_RIGHT_SPIRAL);
	lamp_tristate_off (LM_LEFT_SPIRAL);
}

CALLSET_ENTRY (spiral, start_ball)
{
	lamp_tristate_off (LM_RIGHT_SPIRAL);
	lamp_tristate_off (LM_LEFT_SPIRAL);
	spiral_loops = 0;
}

CALLSET_ENTRY (spiral, display_update)
{
	timed_mode_display_update (&spiral_mode);
}

CALLSET_ENTRY (spiral, music_refresh)
{
	timed_mode_music_refresh (&spiral_mode);
}

CALLSET_ENTRY (spiral, end_ball)
{
	timed_mode_end (&spiral_mode);
}

CALLSET_ENTRY (spiral, door_start_spiral)
{
	timed_mode_begin (&spiral_mode);
}
