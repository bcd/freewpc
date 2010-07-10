/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (deadend, __machine2__) */
//TODO Slot is being ignored when hit straight after kicking
// device_switch_can_follow needs tweaking

#include <freewpc.h>
#include <eb.h>

U8 dead_end_count;
extern __local__ U8 gumball_enable_count;
extern void award_unlit_shot (U8 unlit_called_from);
bool __local__ extra_ball_lit_from_deadend;

void inlane_lights_dead_end_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_right);	
	font_render_string_center (&font_fixed6, 64, 6, "RIGHT INLANE");
	font_render_string_center (&font_fixed6, 64, 22, "LIGHTS DEAD END");
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
}

void dead_end_deff (void)
{

	dmd_alloc_low_clean();
	sound_send (SND_DEAD_END_SCREECH);
	dmd_sched_transition (&trans_scroll_right);	
	frame_draw (IMG_CAR);
	dmd_show2 ();
	task_sleep_sec (1);
	sound_send (SND_DEAD_END_CRASH);
	
	U8 i = 0;
	do {
	U8 x = random_scaled (4);
	U8 y = random_scaled (4);
	dmd_alloc_low_clean ();
	psprintf ("1 DEAD END", "%d DEAD ENDS", dead_end_count);
	font_render_string_center (&font_fixed6, 64+x, 7+y, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_33MS);
	} while (i++ < 8);
	
	dmd_alloc_low_clean ();
	psprintf ("1 DEAD END", "%d DEAD ENDS", dead_end_count);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	dmd_show_low ();
	
	if (extra_ball_lit_from_deadend == FALSE && can_award_extra_ball())
	{
		if (dead_end_count < 3 )
			sprintf ("EXTRA BALL AT 3");
		else if (dead_end_count == 3)
		{
			sound_send (SND_GET_THE_EXTRA_BALL);
			sprintf ("EXTRA BALL LIT");
		}
	}
	else if (dead_end_count < 3)
	{
		sprintf ("10M AT 3");
	}
	else if (dead_end_count == 3)
	{
		sprintf ("10 MILLION");
	}

	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}
	
CALLSET_ENTRY (deadend, start_player)
{
	/* Don't allow lighting extra ball if impossible
	 * or if previously collected */
	if (system_config.max_ebs == 0)
		extra_ball_lit_from_deadend = TRUE;
	else
		extra_ball_lit_from_deadend = FALSE;
}

CALLSET_ENTRY (deadend, start_ball)
{
	dead_end_count = 0;
	lamp_off (LM_DEAD_END);
}

CALLSET_ENTRY (deadend, sw_dead_end)
{
	device_switch_can_follow (dead_end, slot, TIME_2S + TIME_500MS);
	event_can_follow (dead_end, camera, TIME_2S);

	if (lamp_test (LM_DEAD_END))
	{
		deff_start (DEFF_DEAD_END);
		dead_end_count++;
		gumball_enable_count++;
		switch (dead_end_count)
		{
			case 1:
				score (SC_250K);
				timed_game_extend (15);
				break;
			case 2:
				score (SC_500K);
				timed_game_extend (20);
				break;
			case 3:
				timed_game_extend (30);
				if (extra_ball_lit_from_deadend == FALSE && can_award_extra_ball ())
				{
					score (SC_1M);
					light_easy_extra_ball ();
					extra_ball_lit_from_deadend = TRUE;
				}
				else
				{
					score (SC_10M);
				}
				break;
			default:
				score (SC_2M);
				timed_game_extend (30);
				break;
		}
		lamp_off (LM_DEAD_END);
	}
	else
	{
		deff_start (DEFF_INLANE_LIGHTS_DEAD_END);
		award_unlit_shot (SW_DEAD_END);
		score (SC_100K);
		sound_send (SND_TOWN_SQUARE_AWARD);
		timed_game_pause (TIME_3S);
	}
}
