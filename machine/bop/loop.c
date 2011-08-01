/*
 * Copyright 2010 by Ewan Meadows (sonny_jim@hotmail.com)
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

#define NUM_LOOP_LEVELS 4

/* Delay in seconds before the loop level decreases by one */
#define LOOP_LEVEL_DELAY 10
#include <freewpc.h>

U8 left_loop_level;
U8 right_loop_level;
U8 left_loop_timer;
U8 right_loop_timer;
U8 last_left_loop_level;
U8 last_right_loop_level;

U8 total_loops;

score_id_t loop_score_table[] = {
	SC_25K, SC_50K, SC_100K, SC_500K
};

lampnum_t left_loop_lamps[] = {
	LM_LEFT_LOOP_25K, LM_LEFT_LOOP_50K, LM_LEFT_LOOP_100K, LM_LEFT_LOOP_500K
};

lampnum_t right_loop_lamps[] = {
	LM_RIGHT_LOOP_25K, LM_RIGHT_LOOP_50K, LM_RIGHT_LOOP_100K, LM_RIGHT_LOOP_500K
};

void loop_deff (void)
{
	seg_alloc_clean ();
	psprintf ("1 LOOP", "%d LOOPS", total_loops);
	seg_write_row_center (0, sprintf_buffer);
	sprintf_score (score_deff_get ());
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

/* task to take the loop levels down by one every 10 seconds */
/* TODO This needs to be changed so it'll handle system wide
 * pausing properly */
static void loop_level_timer_task (void)
{
	while (in_live_game)
	{
		if (left_loop_timer == 0 && left_loop_level != 0)
		{
			bounded_decrement (left_loop_level, 0);
			left_loop_timer = LOOP_LEVEL_DELAY;
		}
		else 
		{
			bounded_decrement (left_loop_timer, 0);
		}
		
		if (right_loop_timer == 0 && right_loop_level != 0)
		{
			bounded_decrement (right_loop_level, 0);
			right_loop_timer = LOOP_LEVEL_DELAY;
		}
		else 
		{
			bounded_decrement (right_loop_timer, 0);
		}
		task_sleep_sec (1);
	}
	task_exit ();
}

static void award_loop (void)
{
	/* Start a timer to go down a level after 10 seconds */
	if (!task_find_gid (GID_LOOP_LEVEL_TIMER))
		task_create_gid (GID_LOOP_LEVEL_TIMER, loop_level_timer_task);
	
	bounded_increment (total_loops, 254);
	deff_start (DEFF_LOOP);	
}

static void award_left_loop (void)
{
	/* This causes a compiler warning as I don't think it 
	 * knows about bounded_increment when checking for
	 * array bounds */
	bounded_increment (left_loop_level, NUM_LOOP_LEVELS - 1);
	/* Reset the loop level timer */
	left_loop_level = LOOP_LEVEL_DELAY;
	score (loop_score_table[left_loop_level]);
	award_loop ();
}

static void award_right_loop (void)
{
	bounded_increment (right_loop_level, NUM_LOOP_LEVELS - 1);
	right_loop_level = LOOP_LEVEL_DELAY;
	score (loop_score_table[right_loop_level]);
	award_loop ();
}

CALLSET_ENTRY (loop, sw_left_loop)
{
	award_left_loop ();
}

CALLSET_ENTRY (loop, sw_right_loop_top)
{
	if (event_did_follow (right_loop_bottom, right_loop_top))
	{
		/* Ball came in from the bottom 
		 * score differently? */
		award_right_loop ();
	}
	event_should_follow (right_loop_top, right_loop_bottom, TIME_2S);
}

CALLSET_ENTRY (loop, sw_right_loop_bottom)
{
	if (event_did_follow (right_loop_top, right_loop_bottom))
	{
		award_right_loop ();
	}
	else if (event_did_follow (right_loop_bottom, right_loop_bottom))
	{
		/* Ball came out the bottom, no loop */
	}
	event_should_follow (right_loop_bottom, right_loop_top, TIME_2S);
	event_can_follow (right_loop_bottom, right_loop_bottom, TIME_2S);
}

void light_left_loop_lamps (void)
{
	/* Don't bother doing anything if nothing has changed */
	if (last_left_loop_level == left_loop_level)
		return;

	/* Turn off all lamps */
	lamplist_apply (LAMPLIST_LEFT_LOOPS, lamp_off);
	
	/* Light up previous lamps and flash the current level */
	U8 i;
	for (i = 0; i < left_loop_level; i++)
	{
		lamp_tristate_on (left_loop_lamps[i]);
	}
	//lamp_tristate_flash (left_loop_lamps[left_loop_level]);
	
	/* Store left_loop_level */
	last_left_loop_level = left_loop_level;
}

void light_right_loop_lamps (void)
{
	/* Turn off all lamps */
	lamplist_apply (LAMPLIST_RIGHT_LOOPS, lamp_off);
	
	/* Light up previous lamps and flash the current level */
	U8 i;
	for (i = 0; i < right_loop_level; i++)
	{
		lamp_tristate_on (right_loop_lamps[i]);
	}
	//lamp_tristate_flash (right_loop_lamps[right_loop_level]);
	
	/* Store right_loop_level */
	last_right_loop_level = right_loop_level;
}

CALLSET_ENTRY (loop, start_ball)
{
	total_loops = 0;
	right_loop_level = 0;
	left_loop_level = 0;
	last_right_loop_level = 1;
	last_left_loop_level = 1;
	lamplist_apply (LAMPLIST_RIGHT_LOOPS, lamp_off);
}

CALLSET_ENTRY (loop, end_ball)
{
	task_kill_gid (GID_LOOP_LEVEL_TIMER);
}

CALLSET_ENTRY (loop, lamp_update)
{
	/* Don't bother doing anything if nothing has changed */
	if (last_left_loop_level != left_loop_level)
		light_left_loop_lamps ();

	if (last_right_loop_level != right_loop_level)
		light_right_loop_lamps ();
}
