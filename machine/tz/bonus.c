/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
//TODO Deff to add up score?
// Handle replay during/after bonus

#include <freewpc.h>
extern void flipper_override_pulse (U8 switches);

/* Total bonus score */
score_t total_bonus;
/* Temp variable used to calculate bonus per door/loop etc */
score_t bonus_scored;
bool buttons_held;

/* Speed up the bonus if both flipper buttons are pressed */
void bonus_button_monitor (void)
{
	for (;;)
	{
		if ((switch_poll_logical (SW_LEFT_BUTTON) && switch_poll_logical (SW_RIGHT_BUTTON)) && buttons_held == FALSE)
		{	
			buttons_held = TRUE;
			sound_send (SND_CUCKOO);
		}
		task_sleep (TIME_100MS);
	}
}

void bonus_pause (void)
{
	if (buttons_held)
		task_sleep (TIME_100MS);
	else
		task_sleep_sec (1);
}

void bonus_deff (void)
{
	extern U8 door_panels_started;
	extern U8 loops;
	extern U8 jets_bonus_level;
	extern U8 jets_scored;
	extern U8 left_ramps;
	extern U8 gumball_collected_count;
	extern U8 spiralawards_collected;
	extern U8 dead_end_count;
	extern U8 hitch_count;
	extern bool backdoor_award_collected;
	
	/* Clear the bonus score */
	score_zero (total_bonus);
	buttons_held = FALSE;
	task_recreate_gid (GID_BONUS_BUTTON_MONITOR, bonus_button_monitor);
	/* Show Initial bonus screen */
	sample_start (MUS_FADE_BONUS, SL_500MS);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_times10, 64, 16, "BONUS");
	//dmd_sched_transition (&trans_random_boxfade);
	dmd_show_low ();
	bonus_pause ();

	if (door_panels_started > 0)
	{
		dmd_alloc_low_clean ();
		score_multiple (SC_1M, door_panels_started);
		psprintf ("%d DOOR PANEL", "%d DOOR PANELS", door_panels_started);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_1M]);
		score_mul (bonus_scored, door_panels_started); 
		score_add (total_bonus, bonus_scored);
		sprintf_score (bonus_scored);	
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		//sprintf ("%d x 1,000,000", door_panels_started);
		dmd_sched_transition (&trans_scroll_down);
		dmd_show_low ();
		sound_send (SND_GREED_ROUND_BOOM);
		bonus_pause ();
	}
	
	if (loops > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_100K]);
		score_mul (bonus_scored, loops); 
		score_add (total_bonus, bonus_scored);
		psprintf ("%d LOOP", "%d LOOPS", loops);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf_score (bonus_scored);	
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("%d X 100,000", loops);
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		dmd_sched_transition (&trans_scroll_down);
		dmd_show_low ();
		sound_send (SND_GREED_ROUND_BOOM);
		bonus_pause ();
	}

	if (jets_scored > 0)
	{
		U8 total_jets = (jets_scored * jets_bonus_level);
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_100K]);
		score_mul (bonus_scored, total_jets); 
		score_add (total_bonus, bonus_scored);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("TOWNSQUARE JETS");
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf ("%d X 100,000", (total_jets));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		dmd_sched_transition (&trans_scroll_down);
		dmd_show_low ();
		sound_send (SND_GREED_ROUND_BOOM);
		bonus_pause ();
	}
	
	if (left_ramps > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_100K]);
		score_mul (bonus_scored, left_ramps); 
		score_add (total_bonus, bonus_scored);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("LEFT RAMPS");
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf ("%d X 100,000", (left_ramps));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		dmd_sched_transition (&trans_scroll_down);
		dmd_show_low ();
		sound_send (SND_GREED_ROUND_BOOM);
		bonus_pause ();
	}
	
	if (gumball_collected_count > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_1M]);
		score_mul (bonus_scored, gumball_collected_count); 
		score_add (total_bonus, bonus_scored);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		psprintf ("%d GUMBALL", "%d GUMBALLS", gumball_collected_count);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf ("%d X 1,000,000", (gumball_collected_count));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		dmd_sched_transition (&trans_scroll_down);
		dmd_show_low ();
		sound_send (SND_GREED_ROUND_BOOM);
		bonus_pause ();
	}

	if (spiralawards_collected > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_1M]);
		score_mul (bonus_scored, spiralawards_collected); 
		score_add (total_bonus, bonus_scored);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		psprintf ("%d SPIRAL AWARD", "%d SPIRAL AWARDS", spiralawards_collected);
		font_render_string_center (&font_mono5, 64, 6, sprintf_buffer);
		sprintf ("%d X 1,000,000", (spiralawards_collected));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		dmd_sched_transition (&trans_scroll_down);
		dmd_show_low ();
		sound_send (SND_GREED_ROUND_BOOM);
		bonus_pause ();
	}	
	
	if (dead_end_count > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_1M]);
		score_mul (bonus_scored, dead_end_count); 
		score_add (total_bonus, bonus_scored);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		psprintf ("%d DEAD END", "%d DEAD ENDS", dead_end_count);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf ("%d X 1,000,000", (dead_end_count));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		dmd_sched_transition (&trans_scroll_down);
		dmd_show_low ();
		sound_send (SND_GREED_ROUND_BOOM);
		bonus_pause ();
	}

	if (hitch_count > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_1M]);
		score_mul (bonus_scored, hitch_count); 
		score_add (total_bonus, bonus_scored);
		psprintf ("%d HITCHHIKER", "%d HITCHHIKERS", hitch_count);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("%d X 1,000,000", (hitch_count));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		dmd_sched_transition (&trans_scroll_down);
		dmd_show_low ();
		sound_send (SND_GREED_ROUND_BOOM);
		bonus_pause ();
	}
	
	if (backdoor_award_collected == TRUE)
	{
		dmd_alloc_low_clean ();
		score_add (total_bonus, score_table[SC_20M]);
		sprintf ("BACKDOOR AWARD");
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf ("20 MILLION");
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		sound_send (SND_SURVIVAL_IS_EVERYTHING);
		task_sleep_sec (2);
		bonus_pause ();
	}

	task_kill_gid (GID_BONUS_BUTTON_MONITOR);

	/* Show total Bonus */	
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 6, "TOTAL BONUS");
	sprintf_score (total_bonus);
	font_render_string_center (&font_fixed10, 64, 26, sprintf_buffer);
	dmd_sched_transition (&trans_scroll_down);
	dmd_show_low ();
	sound_send (SND_GREED_ROUND_BOOM);
	task_sleep_sec (1);
	/* Add to player score */
	score_long (total_bonus);
	
	/* Show final score */
	dmd_alloc_low_clean ();
	scores_draw ();
	dmd_sched_transition (&trans_scroll_up);
	dmd_show_low ();
	sample_start (MUS_FADE_EXIT, SL_2S);
	task_sleep (TIME_16MS);
	sample_start (SND_GREED_ROUND_BOOM, SL_1S);

	task_sleep_sec (2);
	task_sleep (TIME_500MS);
	deff_exit ();
}
