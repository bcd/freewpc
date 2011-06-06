/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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
#include <eb.h>
#include <status.h>

/* Total bonus score */
score_t total_bonus;
/* 1 Ball Hi score values */
score_t start_ball_score;
score_t points_this_ball;
score_t temp_score;

/* Current single ball top score during this game */
score_t current_one_ball_hi_score;
extern U8 loop_master_hi;
extern U8 combo_master_hi;
extern U8 spawny_get_hi;

#define current_hi_score scores[find_player_ranked(1)]
#define current_hi_player find_player_ranked(1) + 1

/* Which player obtained the current 1 ball hi score*/
U8 current_one_ball_hi_player;
/* On which ball was the current 1 ball hi score */
U8 current_one_ball_hi_ball_number;
//U8 current_player_rankings[4];
U8 countup_pause_iterations;

bool buttons_held;
bool quickdeath_timer_running;
bool quickdeath_timer_already_run;
extern U8 loop_master_initial_enter;
extern U8 combo_master_initial_enter;

extern bool powerball_death;
extern U8 score_ranks[MAX_PLAYERS];
extern U8 door_panels_started;
extern U8 loops;
extern U8 jets_bonus_level;
extern U16 jets_scored;
extern U8 left_ramps;
extern U8 gumball_collected_count;
extern U8 spiralawards_collected;
extern U8 dead_end_count;
extern U8 hitch_count;
extern U8 rollover_count;
extern bool backdoor_award_collected;
extern U8 multidrain_count;
extern bool stdm_death;
extern bool unfair_death;
	
extern U8 two_way_combos;
extern U8 three_way_combos;
extern U8 lucky_bounces;

/* Function to find who holds what score position 
 * eg
 * find_player_ranked (N) returns the player ranked first
 * returns player number - 1 for score purposes
 */

static U8 find_player_ranked (U8 ranking)
{
	/* If out of range, return last place */
	if (ranking > MAX_PLAYERS)
		ranking = MAX_PLAYERS;
	U8 i = 0;
	while (score_ranks[i] != ranking)
		i++;
	return i;
}


/* Speed up the bonus if both flipper buttons are pressed */
void bonus_button_monitor (void)
{
	buttons_held = FALSE;
	while (in_bonus)
	{
		if ((switch_poll_logical (SW_LEFT_BUTTON) 
			&& switch_poll_logical (SW_RIGHT_BUTTON)) 
			&& buttons_held == FALSE)
		{	
			buttons_held = TRUE;
		}
		else
		{
			buttons_held = FALSE;
		}
		task_sleep (TIME_100MS);
	}
	task_exit ();
}

static void bonus_pause (void)
{
	if (buttons_held)
		task_sleep_sec (1);
	else
		task_sleep (TIME_100MS);
}

static void countup_pause (void)
{
	if (buttons_held)
	{
		score_add (temp_score, score_table[SC_1M]);
		if (countup_pause_iterations < 50)
			task_sleep (TIME_33MS);
		else
		{
			score_add (temp_score, score_table[SC_1M]);
			task_sleep (TIME_16MS);
		}
	}
	else
		task_sleep (TIME_33MS);
}

/* Function so we can call two different transistions
 * depending on whether the buttons were pressed */
static inline void bonus_sched_transition (void)
{
	if (!buttons_held)
		dmd_sched_transition (&trans_scroll_down_fast);
	else
		dmd_sched_transition (&trans_scroll_down);
}

static void bonus_talking_task (void)
{
	sound_send (SND_NOT_AN_ORDINARY_GAME);
	task_sleep_sec (2);
	sound_send (SND_OR_AN_ORDINARY_PLAYER);
	task_sleep_sec (2);
	
	switch (find_player_ranked (1) + 1)
	{
		case 1:
			sound_send (SND_PLAYER_ONE);
			break;
		case 2:
			sound_send (SND_PLAYER_TWO);
			break;
		case 3:
			sound_send (SND_PLAYER_THREE);
			break;
		case 4:
			sound_send (SND_PLAYER_FOUR);
			break;
		default:
			break;
	}
	task_exit ();
}

/* Used to announce if the player has played well */
inline bool check_for_big_score (void)
{
	if (score_compare (points_this_ball, score_table[SC_100M]) == 1)
		return TRUE;
	else
		return FALSE;
}

inline bool check_for_puny_score (void)
{
	if (score_compare (score_table[SC_10M], points_this_ball) == 1)
		return TRUE;
	else
		return FALSE;

}

void points_this_ball_sound_task (void)
{
	sound_send (SND_NOT_AN_ORDINARY_GAME);
	task_sleep_sec (2);
	sound_send (SND_OR_AN_ORDINARY_PLAYER);
	task_sleep_sec (10);
	task_exit ();
}

/* See if it's the players last ball */
inline bool check_if_last_ball_for_multiplayer (void)
{
	if (ball_up == system_config.balls_per_game && num_players > 1 && extra_balls == 0)
		return TRUE;
	else
		return FALSE;
}

inline bool check_if_last_ball_of_multiplayer_game (void)
{
	if (ball_up == system_config.balls_per_game && player_up == num_players && num_players > 1 && extra_balls == 0)
		return TRUE;
	else
		return FALSE;
}

static void draw_taunts (void)
{
	/* 
	 * Taunts.....
	 * */
	
//	task_sleep (TIME_500MS);
//
	if (multidrain_count >= 3)
	{
		dmd_alloc_low_clean ();
		sprintf ("MULTIDRAIN");
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		sound_send (SND_HEY_ITS_ONLY_PINBALL);
		task_sleep_sec (2);
	}
	else if (unfair_death)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed10, 64, 11, "BAD SHOW");
		font_render_string_center (&font_var5, 64, 26, "UNFAIR DEATH");
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		sound_send (SND_HAHA_POWERFIELD_EXIT);
		task_sleep_sec (2);
	}


	else if (stdm_death)
	{
		dmd_alloc_low_clean ();
		sprintf ("SDTM DEATH");
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		sound_send (SND_HEY_ITS_ONLY_PINBALL);
		task_sleep_sec (2);
	}
	
	else if (quickdeath_timer_running && !unfair_death)
	{
		dmd_alloc_low_clean ();
		sprintf ("YOU LASTED LONG");
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		sound_send (SND_HAHA_POWERFIELD_EXIT);
		task_sleep_sec (2);
	}
	
	if (powerball_death == TRUE)
	{
		dmd_alloc_low_clean ();
		sprintf ("POWERBALL");
		font_render_string_center (&font_steel, 64, 8, sprintf_buffer);
		sprintf ("DEATH");
		font_render_string_center (&font_steel, 64, 18, sprintf_buffer);
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		sound_send (SND_NEVER_UNDERESTIMATE_POWER);
		task_sleep_sec (3);
	}	
}

/* Function to calculate bonus score */
static void bonus_add_up_score (U8 award_value, U8 amount)
{
	score_t bonus_score;
	/* Zero the temporary score */
	score_zero (bonus_score);
	/* Add the award amount to the temp score */
	score_add (bonus_score, score_table[award_value]);
	/* Multiply it by award_count */
	score_mul (bonus_score, amount);
	/* Store in sprintf_buffer */
	score_add (total_bonus, bonus_score);
	sprintf_score (bonus_score);	
}

static void bonus_add_up_jets (void)
{
	score_t bonus_score;
	score_zero (bonus_score);
	score_add (bonus_score, score_table[SC_100K]);
	score_mul (bonus_score, jets_scored); 
	score_add (total_bonus, bonus_score);
		
	score_zero (bonus_score);
	score_add (bonus_score, score_table[SC_1M]);
	score_mul (bonus_score, jets_bonus_level);
	score_add (total_bonus, bonus_score);
	sprintf_score (bonus_score);
}

static void calc_and_draw_bonus (U8 award_value, U8 amount)
{
	dmd_alloc_low_clean ();
	bonus_add_up_score (award_value, amount);
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	sprintf ("%d X %10b", amount, score_table[award_value]);
	font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
}

void trans_and_show (void)
{
	bonus_sched_transition ();
	dmd_show_low ();
	sound_send (SND_GREED_MODE_BOOM);
	bonus_pause ();
}

void one_ball_score_task (void)
{
	/* 
	 *
	 * Total points this ball 
	 * 
	 */
	
	/* Calculate */
	score_zero (points_this_ball);
	score_copy (points_this_ball, current_score);
	score_sub (points_this_ball, start_ball_score);
	
	/* Don't show if on first ball, you can just look at the scoreboard */
	if (ball_up != 1 && feature_config.adv_bonus_info == YES)
	{	
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed6, 64, 6, "POINTS THIS BALL");
		sprintf_score (points_this_ball);
		font_render_string_center (&font_fixed10, 64, 24, sprintf_buffer);
		dmd_show_low ();
		if (check_for_puny_score ())
			sound_send (SND_BUYIN_CANCELLED);
		else if (check_for_big_score ())
			task_sleep_sec (2);
		else
			sound_send (SND_GREED_MODE_BOOM);
		
		bonus_sched_transition ();
		task_sleep_sec (2);
	}
	
	/* Store and sort the current one ball hi scores during multiplayer */
	if (num_players > 1 && score_compare (points_this_ball, current_one_ball_hi_score) == 1)
	{
		score_zero (current_one_ball_hi_score);
		score_copy (current_one_ball_hi_score, points_this_ball);
		current_one_ball_hi_player = player_up;
		current_one_ball_hi_ball_number = ball_up;
	}
		
	/* If it's the last player of a multi player game ... */
	if (num_players > 1 && player_up == num_players && ball_up != 1 && extra_balls == 0 && feature_config.adv_bonus_info == YES)
	{
	 	/* show highest 1 ball score so far*/
		dmd_alloc_low_clean ();
		font_render_string_center (&font_mono5, 64, 4, "HIGHEST 1 BALL SCORE");
		sprintf_score (current_one_ball_hi_score);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		if (current_one_ball_hi_ball_number == ball_up)
			sprintf ("P%d ON THEIR LAST BALL", current_one_ball_hi_player);
		else if (current_one_ball_hi_ball_number == ball_up && current_one_ball_hi_player == player_up)
			sprintf ("P%d ON THE LAST BALL", current_one_ball_hi_player);
		else
			sprintf ("PLAYER %d ON BALL %d", current_one_ball_hi_player, current_one_ball_hi_ball_number);
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		bonus_sched_transition ();
		dmd_show_low ();
		task_sleep_sec (2);
	}
}

void bonus_deff (void)
{
	draw_taunts ();
	/* Clear the bonus score */
	score_zero (total_bonus);

	/* Show Initial bonus screen */
	sample_start (MUS_FADE_BONUS, SL_500MS);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_times10, 64, 16, "BONUS");
	dmd_sched_transition (&trans_bitfade_fast);
	dmd_show_low ();
	task_sleep_sec (1);

	/* Start a task to monitor the buttons */
	task_recreate_gid (GID_BONUS_BUTTON_MONITOR, bonus_button_monitor);
	
	if (door_panels_started)
	{
		calc_and_draw_bonus (SC_1M, door_panels_started);
		font_render_string_center (&font_mono5, 64, 4, "DOOR PANELS");
		trans_and_show ();
	}
	
	if (loops)
	{
		calc_and_draw_bonus (SC_1M, loops);
		font_render_string_center (&font_mono5, 64, 4, "LOOPS");
		trans_and_show ();
		if (loops > loop_master_hi)
		{
			loop_master_hi = loops;
			loop_master_initial_enter = player_up;
			task_sleep_sec (1);
			dmd_alloc_low_clean ();
			font_render_string_center (&font_fixed10, 64, 16, "LOOP MASTER");
			dmd_sched_transition (&trans_sequential_boxfade);
			dmd_show_low ();
			sound_send (SND_GLASS_BREAKS);
			task_sleep_sec (2);
			dmd_alloc_low_clean ();
			sprintf ("%d LOOPS", loop_master_hi);
			font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
			dmd_show_low ();
			task_sleep_sec (2);
		}
	}

	if (jets_scored > 0)
	{
		dmd_alloc_low_clean ();
		bonus_add_up_jets ();
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("TOWNSQUARE JETS");
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		trans_and_show ();
	}
	
	if (left_ramps)
	{
		calc_and_draw_bonus (SC_100K, left_ramps);
		font_render_string_center (&font_mono5, 64, 4, "LEFT RAMPS");
		trans_and_show ();
	}
	
	if (gumball_collected_count)
	{
		calc_and_draw_bonus (SC_1M, gumball_collected_count);
		font_render_string_center (&font_mono5, 64, 4, "GUMBALLS");
		trans_and_show ();
	}

	if (spiralawards_collected)
	{
		calc_and_draw_bonus (SC_1M, spiralawards_collected);
		font_render_string_center (&font_mono5, 64, 4, "SPIRALAWARDS");
		trans_and_show ();
	}	
	
	if (dead_end_count)
	{
		calc_and_draw_bonus (SC_1M, dead_end_count);
		font_render_string_center (&font_mono5, 64, 4, "DEAD ENDS");
		trans_and_show ();

	}

	if (hitch_count > 0)
	{
		calc_and_draw_bonus (SC_1M, hitch_count);
		font_render_string_center (&font_mono5, 64, 4, "HITCHHIKERS");
		trans_and_show ();

	}
	
	if (rollover_count)
	{
		calc_and_draw_bonus (SC_1M, rollover_count);
		font_render_string_center (&font_mono5, 64, 4, "ROLLOVERS");
		trans_and_show ();
		if (rollover_count > 9)
		{
			task_sleep_sec (2);
			dmd_alloc_low_clean ();
			font_render_string_center (&font_mono5, 64, 10, "KEEPING THOSE");
			font_render_string_center (&font_mono5, 64, 20, "BUTTONS BUSY");
			dmd_sched_transition (&trans_sequential_boxfade);
			dmd_show_low ();
			sound_send (SND_GLASS_BREAKS);
			task_sleep_sec (3);
		}
	}
	
	if (two_way_combos + three_way_combos)
	{
		calc_and_draw_bonus (SC_5M, two_way_combos + three_way_combos);
		font_render_string_center (&font_mono5, 64, 4, "COMBOS");
		trans_and_show ();
		if (two_way_combos + three_way_combos > combo_master_hi)
		{
			combo_master_hi = two_way_combos + three_way_combos;
			combo_master_initial_enter = player_up;
			task_sleep_sec (1);
			dmd_alloc_low_clean ();
			font_render_string_center (&font_fixed10, 64, 16, "COMBO MASTER");
			dmd_sched_transition (&trans_sequential_boxfade);
			dmd_show_low ();
			sound_send (SND_GLASS_BREAKS);
			task_sleep_sec (2);
			dmd_alloc_low_clean ();
			psprintf ("%d COMBO", "%d COMBOS", combo_master_hi);
			font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
			dmd_show_low ();
			task_sleep_sec (2);
		}

	}

	if (lucky_bounces)
	{
		calc_and_draw_bonus (SC_5M, lucky_bounces);
		font_render_string_center (&font_mono5, 64, 4, "LUCKY BOUNCES");
		trans_and_show ();
		
		if (lucky_bounces > 4 && lucky_bounces < spawny_get_hi)
		{
			dmd_alloc_low_clean ();
			font_render_string_center (&font_fixed10, 64, 16, "SPAWNY GET");
			dmd_sched_transition (&trans_sequential_boxfade);
			dmd_show_low ();
			sound_send (SND_LUCKY);
			task_sleep_sec (2);
		}
		else if (lucky_bounces > spawny_get_hi)
		{
			spawny_get_hi = lucky_bounces;
			dmd_alloc_low_clean ();
			font_render_string_center (&font_fixed10, 64, 16, "SPAWNIEST GET");
			dmd_sched_transition (&trans_sequential_boxfade);
			dmd_show_low ();
			sound_send (SND_GLASS_BREAKS);
			task_sleep_sec (3);
		}
		else
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
		dmd_sched_transition (&trans_bitfade_fast);
		dmd_show_low ();
		sound_send (SND_SURVIVAL_IS_EVERYTHING);
		task_sleep_sec (3);
	}
	
	if (backdoor_award_collected == TRUE
		&& door_panels_started >= 10
		&& loops
		&& jets_bonus_level
		&& left_ramps
		&& gumball_collected_count
		&& spiralawards_collected
		&& dead_end_count
		&& hitch_count
		&& rollover_count
		&& lucky_bounces
		&& two_way_combos + three_way_combos)
	{
		sound_send (SND_PLAYER_PIANO_UNUSED);
		dmd_alloc_low_clean ();
		score_add (total_bonus, score_table[SC_100M]);
		sprintf ("TOURIST AWARD");
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf ("100 MILLION");
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		task_sleep_sec (6);
	}

	/* Add total bonus to player score */
	score_long (total_bonus);
	/* Show total Bonus */	
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 6, "TOTAL BONUS");
	sprintf_score (total_bonus);
	font_render_string_center (&font_fixed10, 64, 24, sprintf_buffer);
	bonus_sched_transition ();
	dmd_show_low ();
	sound_send (SND_GREED_MODE_BOOM);
	bonus_pause ();
	task_sleep_sec (1);
		
	
	/* Calculate and show 1 ball hiscores */
//	one_ball_score_task ();
	/* Calculate lead into temp_score */
	if (num_players > 1)
	{
		score_zero (temp_score);
		score_copy (temp_score, scores[find_player_ranked(1)]);
		score_sub (temp_score, scores[find_player_ranked(2)]);
		sprintf_score (temp_score);
	}

	if (check_if_last_ball_of_multiplayer_game ())
	{
		task_sleep_sec (2);
	}
	else if (num_players > 1 && player_up == num_players && ball_up != 1)
	{
		sound_send (SND_RABBLE_RABBLE);
		dmd_alloc_low_clean ();
		sprintf("PLAYER %d LEADS BY", find_player_ranked(1) + 1);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf_score(temp_score);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		
		if (num_players == 3)
		{
			sprintf("2ND P%d 3RD P%d", find_player_ranked(2) + 1, find_player_ranked(3) + 1);
			font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		}
		else if (num_players == 4)
		{
			sprintf("2ND P%d 3RD P%d 4TH P%d", find_player_ranked(2) + 1, find_player_ranked(3) + 1,
				find_player_ranked(4) + 1);
				font_render_string_center (&font_var5, 64, 26, sprintf_buffer);
		}
		dmd_show_low ();
		task_sleep_sec (3);
		
	
	}


	if (check_if_last_ball_of_multiplayer_game ()
		&& feature_config.adv_bonus_info == YES)

	{
		task_create_gid (GID_BONUS_TALKING, bonus_talking_task);
		sound_send (SND_PLAYER_PIANO_UNUSED);
		dmd_alloc_low_clean ();
			
		sprintf("PLAYER %d WINS BY", find_player_ranked(1) + 1);
		font_render_string_center (&font_mono5, 64, 3, sprintf_buffer);
		sound_send (SND_GREED_MODE_BOOM);
		sprintf_score (temp_score);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		if (score_compare (score_table[SC_10M], temp_score) == 1)
		{
			font_render_string_center (&font_mono5, 64, 29, "THAT WAS CLOSE");
		}
		else if (score_compare (temp_score, score_table[SC_100M]) == 1)
		{
			font_render_string_center (&font_mono5, 64, 29, "A LITTLE LOPSIDED");
		}
		else
		{
			font_render_string_center (&font_mono5, 64, 29, "CONGRATULATIONS");
		}	
		dmd_show_low ();
		task_sleep_sec (5);
	}
	
	/* Show final score */
	dmd_alloc_low_clean ();
	scores_draw ();
	dmd_sched_transition (&trans_scroll_up);
	dmd_show_low ();
	sample_start (MUS_FADE_EXIT, SL_2S);
	task_sleep (TIME_16MS);
	sample_start (SND_GREED_MODE_BOOM, SL_1S);

	task_sleep_sec (2);
	task_sleep (TIME_500MS);
	task_kill_gid (GID_BONUS_BUTTON_MONITOR);
	task_kill_gid (GID_BONUS_TALKING);
	deff_exit ();
}

CALLSET_ENTRY (bonus, serve_ball)
{
	if (check_if_last_ball_of_multiplayer_game ()
		&& feature_config.adv_bonus_info == YES)
		deff_start_sync (DEFF_SCORE_TO_BEAT);
}

void score_to_beat_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 19, "POINTS NEEDED");
	if (find_player_ranked(1) + 1 == player_up)
	{
		font_render_string_center (&font_fixed10, 64, 8, "NO");
		if (check_if_last_ball_of_multiplayer_game ())
		{
			font_render_string_center (&font_mono5, 64, 26, "SHOWBOAT A LITTLE");
		}
		else
		{
			font_render_string_center (&font_mono5, 64, 26, "INCREASE YOUR LEAD");
		}
	}
	else if (score_compare (scores[find_player_ranked(1)], current_score) == 1)
	{
		score_zero (temp_score);
		score_copy (temp_score, scores[find_player_ranked(1)]);
		score_sub (temp_score, current_score);
	
		sprintf_score (temp_score);
		font_render_string_center (&font_fixed10, 64, 8, sprintf_buffer);
		if (player_up > current_hi_player)
			sprintf ("TO BEAT PLAYER %d", current_hi_player);
		else
			sprintf ("TO CATCH UP TO P%d", current_hi_player);
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
	}
	dmd_show_low ();
	task_sleep_sec (4);
	deff_exit ();
}

CALLSET_ENTRY (bonus, start_ball)
{
	/* Store the start ball store */
	score_copy (start_ball_score, current_score);
	quickdeath_timer_already_run = FALSE;
	quickdeath_timer_running = FALSE;
}

CALLSET_ENTRY (bonus, start_game)
{
	/* Initiliase hi score storage variables */
	score_zero (start_ball_score);
	score_zero (current_one_ball_hi_score);
	current_one_ball_hi_player = 0;
	current_one_ball_hi_ball_number = 0;
}


/* Done like this rather than a timer_ as 
 * this can go for 255 seconds */
void quickdeath_timer_task (void)
{
	quickdeath_timer_running = TRUE;
	/* Amount of seconds for the timer to run */
	U8 i = 25;
	do {
		task_sleep_sec (1);
	} while (--i != 0);
	quickdeath_timer_running = FALSE;
	quickdeath_timer_already_run = TRUE;
	task_exit ();
}

CALLSET_ENTRY (bonus, valid_playfield)
{
	/* Start a timer if this is the first time
	 * a ball has entered the playfield */
	if (quickdeath_timer_already_run == FALSE && quickdeath_timer_running == FALSE)
		task_create_gid (GID_QUICKDEATH, quickdeath_timer_task);
}

CALLSET_ENTRY (bonus, rank_change)
{
	/* Don't do anything if the player up isn't now in first place */
	if (!in_live_game && (score_ranks[player_up - 1] != 1))
		return;
	 
	/* Check if last ball of game */
	/* Don't count extra balls because we don't need to */
	if (ball_up == system_config.balls_per_game 
		&& player_up == num_players 
		&& num_players > 1)
	{
		/* Notify the player that they have won */
		deff_start (DEFF_HOME_AND_DRY);
	}
	else if (check_if_last_ball_for_multiplayer ())
	{
		/* Notify the player that they have taken the lead */
		deff_start (DEFF_IN_THE_LEAD);
	}
}

CALLSET_ENTRY (bonus, status_report)
{
	status_page_init ();
	if (num_players > 1)
	{
		font_render_string_center (&font_mono5, 64, 19, "POINTS NEEDED");
		/* find_player_ranked returns from 0 */
		if (find_player_ranked(1) + 1 == player_up)
		{
			font_render_string_center (&font_fixed10, 64, 8, "NO");
			if (check_if_last_ball_of_multiplayer_game ())
			{
				font_render_string_center (&font_mono5, 64, 26, "SHOWBOAT A LITTLE");
			}
			else
			{
				font_render_string_center (&font_mono5, 64, 26, "INCREASE YOUR LEAD");
			}
		}
		else if (score_compare (scores[find_player_ranked(1)], current_score) == 1)
		{
			score_zero (temp_score);
			score_copy (temp_score, scores[find_player_ranked(1)]);
			score_sub (temp_score, current_score);
		
			sprintf_score (temp_score);
			font_render_string_center (&font_fixed10, 64, 8, sprintf_buffer);
			if (player_up > current_hi_player)
				sprintf ("TO BEAT PLAYER %d", current_hi_player);
			else
				sprintf ("TO CATCH UP TO P%d", current_hi_player);
			font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		}
	}
	/* Single ball play */
	else
	{
		score_zero (points_this_ball);
		score_copy (points_this_ball, current_score);
		score_sub (points_this_ball, start_ball_score);
		font_render_string_center (&font_fixed6, 64, 6, "POINTS THIS BALL");
		sprintf_score (points_this_ball);
		font_render_string_center (&font_fixed10, 64, 24, sprintf_buffer);
	}
	status_page_complete ();
}

CALLSET_ENTRY (bonus, end_ball)
{
	task_kill_gid (GID_QUICKDEATH);
}

