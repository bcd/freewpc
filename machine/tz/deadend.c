/*
 * Copyright 2006, 2007, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <eb.h>

__local__ U8 dead_end_count;
extern __local__ U8 gumball_enable_count;
extern void award_unlit_shot (U8 unlit_called_from);

/*void car_deff (void)
{
	dmd_alloc_low_clean ();
	frame_draw (IMG_CAR);
	dmd_sched_transition (&trans_scroll_left);
	dmd_show_low;
	task_sleep_sec (2);	
	deff_exit ();
}
*/
/*
void animation_car1 (struct animation_object *obj)
{
	frame_draw(IMG_CAR1);
}

void animation_car2 (struct animation_object *obj)
{
	frame_draw(IMG_CAR2);
}

void animation_car3 (struct animation_object *obj)
{
	frame_draw(IMG_CAR3);
}

void animation_car4 (struct animation_object *obj)
{
	frame_draw(IMG_CAR4);
}

void animation_car5 (struct animation_object *obj)
{
	frame_draw(IMG_CAR5);
}
*/
void dead_end_deff (void)
{
	/*
	U8 n;
	
	animation_begin (AN_MONO+AN_CLEAN);
	animation_set_speed (TIME_66MS);
	animation_add_static (animation_car1);
	animation_add_static (animation_car2);
	animation_add_static (animation_car3);
	animation_add_static (animation_car4);
	animation_add_static (animation_car5);
	for (n=1; n <100; n++)
		animation_step ();
	animation_end ();
	*/
	dmd_alloc_low_clean();
	sound_send (SND_DEAD_END_SCREECH);
	dmd_sched_transition (&trans_scroll_right);	
	frame_draw (IMG_CAR);
	dmd_show2 ();
	task_sleep_sec (1);
	sound_send (SND_DEAD_END_CRASH);
	dmd_alloc_low_clean();
	psprintf ("1 DEAD END", "%d DEAD ENDS", dead_end_count);
	font_render_string_center (&font_fixed6, 64, 7, sprintf_buffer);
	
	if (dead_end_count < 3)
		sprintf ("EXTRA BALL AT 3");
	else
		sprintf ("");
	font_render_string_center (&font_mono5, 64, 21, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (deadend, start_player)
{
	dead_end_count = 0;
}

CALLSET_ENTRY (deadend, start_ball)
{
	lamp_off (LM_DEAD_END);
}

CALLSET_ENTRY (deadend, sw_dead_end)
{
	device_switch_can_follow (dead_end, slot, TIME_8S);
	event_can_follow (dead_end, camera, TIME_4S);

//	deff_start (DEFF_DEAD_END);
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
				score (SC_1M);
				timed_game_extend (30);
				sound_send (SND_GET_THE_EXTRA_BALL);
				light_easy_extra_ball ();
				break;
			default:
				score (SC_2M);
				timed_game_extend (30);
				break;
		}
		lamp_off (LM_DEAD_END);
		lamp_on (LM_RIGHT_INLANE);
	}
	else
	{
		award_unlit_shot (SW_DEAD_END);
		score (SC_100K);
		sound_send (SND_TOWN_SQUARE_AWARD);
		timed_game_pause (TIME_3S);
	}
}

