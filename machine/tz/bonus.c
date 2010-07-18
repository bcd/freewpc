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
//TODO Handle replay during/after bonus
// Count up luckys, sudden deaths etc 
/* CALLSET_SECTION (bonus, __machine2__) */
#include <freewpc.h>
#include <eb.h>

/* Total bonus score */
score_t total_bonus;
/* Temp variable used to calculate bonus per door/loop etc */
score_t bonus_scored;
/* 1 Ball Hi score values */
score_t start_ball_score;
score_t points_this_ball;
score_t temp_score;

/* Current single ball top score during this game */
score_t current_one_ball_hi_score;
#define current_hi_score scores[find_player_ranked(1)]
#define current_hi_player find_player_ranked(1) + 1
//score_t current_hi_score;
//U8 current_hi_player;
/* Which player obtained the current 1 ball hi score*/
U8 current_one_ball_hi_player;
/* On which ball was the current 1 ball hi score */
U8 current_one_ball_hi_ball_number;
U8 current_player_rankings[4];
U8 countup_pause_iterations;

bool buttons_held;
bool quickdeath_timer_running;
bool quickdeath_timer_already_run;

extern bool powerball_death;
extern U8 score_ranks[MAX_PLAYERS];
extern U8 door_panels_started;
extern U8 loops;
extern U8 jets_bonus_level;
extern U8 jets_scored;
extern U8 left_ramps;
extern U8 gumball_collected_count;
extern U8 spiralawards_collected;
extern U8 dead_end_count;
extern U8 hitch_count;
extern U8 rollover_count;
extern bool backdoor_award_collected;
extern bool multidrain_awarded;
extern bool slot_stdm_death;
extern bool unfair_death;
	
extern U8 two_way_combos;
extern U8 three_way_combos;
extern U8 lucky_bounces;


/* Function to find who holds what score position 
 * eg
 * find_player_ranked (1) returns the player ranked first
 * returns from 0 - MAX_PLAYERS
 */

static U8 find_player_ranked (U8 ranking)
{
	/* If out of range, return last place */
	if (ranking > MAX_PLAYERS)
		return MAX_PLAYERS;
	U8 i = 0;
	while (score_ranks[i] != ranking)
		i++;
	return i;
}


/* Speed up the bonus if both flipper buttons are pressed */
static void bonus_button_monitor (void)
{
	buttons_held = FALSE;
	for (;;)
	{
		if ((switch_poll_logical (SW_LEFT_BUTTON) 
			&& switch_poll_logical (SW_RIGHT_BUTTON)) 
			&& buttons_held == FALSE)
		{	
			buttons_held = TRUE;
			sound_send (SND_FIST_BOOM1);
		}
		task_sleep (TIME_100MS);
	}
}

static void bonus_pause (void)
{
	if (buttons_held)
		task_sleep (TIME_100MS);
	else
		task_sleep_sec (1);
}

void countup_pause (void)
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
static void bonus_sched_transition (void)
{
	if (buttons_held == TRUE)
		dmd_sched_transition (&trans_scroll_down_fast);
	else
		dmd_sched_transition (&trans_scroll_down);
}

/* Function to calculate bonus score */
static void bonus_add_up_score (U8 award_count, score_id_t award_amount)
{
	/* Zero the temporary score */
	score_zero (bonus_scored);
	/* Add the award amount to the temp score */
	score_add (bonus_scored, score_table[award_amount]);
	/* Multiply it by award_count */
	score_mul (bonus_scored, award_count);
	/* Add the temp score to the total bonus */
	score_add (total_bonus, bonus_scored);
	sprintf_score (bonus_scored);	
}

static void bonus_talking_task (void)
{
	sound_send (SND_NOT_AN_ORDINARY_GAME);
	task_sleep_sec (2);
	sound_send (SND_OR_AN_ORDINARY_PLAYER);
	task_sleep_sec (2);
	
	switch (current_one_ball_hi_player)
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
static bool check_for_big_score (void)
{
	if (score_compare (points_this_ball, score_table[SC_100M]) == 1)
		return TRUE;
	else
		return FALSE;
}

static bool check_for_puny_score (void)
{
	if (score_compare (score_table[SC_10M], points_this_ball) == 1)
		return TRUE;
	else
		return FALSE;

}

static void points_this_ball_sound_task (void)
{
	sound_send (SND_NOT_AN_ORDINARY_GAME);
	task_sleep_sec (2);
	sound_send (SND_OR_AN_ORDINARY_PLAYER);
	task_sleep_sec (10);
	task_exit ();
}

/* See if it's the players last ball */
static bool check_if_last_ball_for_multiplayer (void)
{
	if (ball_up == system_config.balls_per_game && num_players > 1 && extra_balls == 0)
		return TRUE;
	else
		return FALSE;
}

static bool check_if_last_ball_of_multiplayer_game (void)
{
	if (ball_up == system_config.balls_per_game && player_up == num_players && 
		num_players > 1 && extra_balls == 0)
		return TRUE;
	else
		return FALSE;
}

void bonus_deff (void)
{
	
	/* 
	 * Taunts.....
	 * */
	/* Wait a bit so music_stop doesn't kill the taunt sounds */
	task_sleep (TIME_66MS);
	if (multidrain_awarded == TRUE)
	{
		dmd_alloc_low_clean ();
		sprintf ("MULTIDRAIN");
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		multidrain_awarded = FALSE;
		sound_send (SND_HEY_ITS_ONLY_PINBALL);
		task_sleep_sec (2);
	}
	
	if (slot_stdm_death == TRUE)
	{
		sound_send (SND_HEY_ITS_ONLY_PINBALL);
		dmd_alloc_low_clean ();
		sprintf ("SLOT DEATH");
	}
	
	if (slot_stdm_death == TRUE)
	{
		sound_send (SND_HEY_ITS_ONLY_PINBALL);
		dmd_alloc_low_clean ();
		sprintf ("STDM DEATH");
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		task_sleep_sec (2);
	}
	
	if (unfair_death == TRUE)
	{
		sound_send (SND_HAHA_POWERFIELD_EXIT);
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed10, 64, 11, "BAD SHOW");
		font_render_string_center (&font_var5, 64, 26, "UNFAIR DEATH");
		dmd_sched_transition (&trans_bitfade_slow);
		dmd_show_low ();
		task_sleep_sec (2);
	}

	if (quickdeath_timer_running == TRUE)
	{
	/* TODO This should only taunt if the game time was <10 seconds
	 * atm it does so when any ball is served, rather than the first */
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
		task_sleep_sec (4);
	}	
	
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
	
	if (door_panels_started > 0)
	{
		dmd_alloc_low_clean ();
		score_multiple (SC_1M, door_panels_started);
		psprintf ("%d DOOR PANEL", "%d DOOR PANELS", door_panels_started);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		
		bonus_add_up_score (door_panels_started, SC_1M);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("%d X 1,000,000", door_panels_started);
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
		bonus_pause ();
	}
	
	if (loops > 0)
	{
		dmd_alloc_low_clean ();
		psprintf ("%d LOOP", "%d LOOPS", loops);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		
		bonus_add_up_score (loops, SC_100K);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("%d X 100,000", loops);
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
		bonus_pause ();
	}

	if (jets_scored > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_100K]);
		score_mul (bonus_scored, jets_scored); 
		score_add (total_bonus, bonus_scored);
		
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_1M]);
		score_mul (bonus_scored, jets_bonus_level);
		score_add (total_bonus, bonus_scored);

		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("TOWNSQUARE JETS");
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
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
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
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
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
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
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
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
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
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
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
		bonus_pause ();
	}
	
	if (rollover_count > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_1M]);
		score_mul (bonus_scored, rollover_count); 
		psprintf ("%d ROLLOVER", "%d ROLLOVERS", rollover_count);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("%d X 1,000,000", (rollover_count));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
		bonus_pause ();
	}

	U8 total_combos = two_way_combos + three_way_combos;
	if (total_combos > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_5M]);
		score_mul (bonus_scored, total_combos); 
		score_add (total_bonus, bonus_scored);
		psprintf ("%d COMBO", "%d COMBOS", total_combos);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("%d X 5,000,000", (total_combos));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
		bonus_pause ();
	}

	if (lucky_bounces > 0)
	{
		dmd_alloc_low_clean ();
		score_zero (bonus_scored);
		score_add (bonus_scored, score_table[SC_5M]);
		score_mul (bonus_scored, lucky_bounces); 
		score_add (total_bonus, bonus_scored);
		psprintf ("%d LUCKYS", "%d LUCKYS", lucky_bounces);
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf_score (bonus_scored);
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sprintf ("%d X 5,000,000", (lucky_bounces));
		font_render_string_center (&font_mono5, 64, 26, sprintf_buffer);
		bonus_sched_transition ();
		dmd_show_low ();
		sound_send (SND_GREED_MODE_BOOM);
		bonus_pause ();
	}


	/* Do not allow the player to skip the next bonuses */
	task_kill_gid (GID_BONUS_BUTTON_MONITOR);

	if (backdoor_award_collected == TRUE)
	{
		dmd_alloc_low_clean ();
		score_add (total_bonus, score_table[SC_20M]);
		sprintf ("BACKDOOR AWARD");
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf ("20 MILLION");
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_show_low ();
		sound_send (SND_SURVIVAL_IS_EVERYTHING);
		task_sleep_sec (3);
	}
	
	if (backdoor_award_collected == TRUE
		&& door_panels_started
		&& loops
		&& jets_bonus_level
		&& left_ramps
		&& gumball_collected_count
		&& spiralawards_collected
		&& dead_end_count
		&& hitch_count
		&& rollover_count
		&& lucky_bounces
		&& total_combos)
	{
		dmd_alloc_low_clean ();
		score_add (total_bonus, score_table[SC_100M]);
		sprintf ("TOURIST AWARD");
		font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
		sprintf ("100 MILLION");
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		sound_send (SND_PLAYER_PIANO_UNUSED);
		dmd_show_low ();
		task_sleep_sec (6);
	}

	/* Restart a task to monitor the buttons */
	buttons_held = FALSE;
	task_recreate_gid (GID_BONUS_BUTTON_MONITOR, bonus_button_monitor);
	
	score_zero (temp_score);
	
	bonus_sched_transition ();
	
	countup_pause_iterations = 0;
	/* Show total Bonus */	
	do {
		dmd_alloc_low_clean ();
		/* Shake the text */
		U8 x;
		U8 y;
		bounded_increment (countup_pause_iterations, 254);
		if (countup_pause_iterations < 10)
		{
			x = random_scaled (1);
			y = random_scaled (2);
		}
		else if (countup_pause_iterations < 20)
		{
			x = random_scaled (2);
			y = random_scaled (3);
		}
		else if (countup_pause_iterations < 30)
		{
			x = random_scaled (4);
			y = random_scaled (5);
		}
		else if (countup_pause_iterations < 40)
		{
			x = random_scaled (8);
			y = random_scaled (4);
		}
		else 
		{
			x = random_scaled (10);
			y = random_scaled (6);
		}
		
		font_render_string_center (&font_fixed6, 64, 6, "TOTAL BONUS");
		sprintf_score (temp_score);
		font_render_string_center (&font_fixed10, 60 + x, 22 + y, sprintf_buffer);
		dmd_show_low ();
		score_add (temp_score, score_table[SC_5130]);
		score_add (temp_score, score_table[SC_500K]);
		sound_send (SND_THUD);
		countup_pause ();
	} while ( score_compare (total_bonus, temp_score) == 1 );
	
	sound_send (SND_GREED_MODE_BOOM);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 6, "TOTAL BONUS");
	sprintf_score (total_bonus);
	font_render_string_center (&font_fixed10, 64, 24, sprintf_buffer);
	dmd_show_low ();	
	task_kill_gid (GID_BONUS_BUTTON_MONITOR);
	task_sleep_sec (2);
		
	/* Add to total bonus to player score */
	score_long (total_bonus);
	
	/* 
	 *
	 * Total points this ball 
	 * 
	 */
	
	/* Calculate */
	score_zero (points_this_ball);
	score_copy (points_this_ball, current_score);
	score_sub (points_this_ball, start_ball_score);
	
	score_zero (temp_score);

	/* Don't show if on first ball, you can just look at the scoreboard */
	if (ball_up != 1)
	{	
		/* Restart a task to monitor the buttons */
		task_create_gid (GID_BONUS_BUTTON_MONITOR, bonus_button_monitor);
		task_kill_gid (GID_BONUS_TALKING);
		
		countup_pause_iterations = 0;
		do {
			dmd_alloc_low_clean ();
			
			/* Shake the text */
			U8 x = random_scaled (8);
			U8 y = random_scaled (4);
			
			font_render_string_center (&font_fixed6, 64, 6, "POINTS THIS BALL");
			sprintf_score (temp_score);
			font_render_string_center (&font_fixed10, 60 + x, 22 + y, sprintf_buffer);
			dmd_show_low ();
			score_add (temp_score, score_table[SC_5130]);
			score_add (temp_score, score_table[SC_500K]);
			/* Make some noise based on points */
			if (score_compare (temp_score, score_table[SC_100M]) == 1 \
				&& !task_find_gid (GID_BONUS_TALKING))
				task_create_gid (GID_BONUS_TALKING, points_this_ball_sound_task);
			else if (!task_find_gid (GID_BONUS_TALKING))
				sound_send (SND_THUD);
			bounded_increment (countup_pause_iterations, 254);
			if (buttons_held == TRUE)
				score_copy (temp_score, points_this_ball);
			countup_pause ();
		} while ( score_compare (points_this_ball, temp_score) == 1 );
		
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
	if (num_players > 1 && player_up == num_players && ball_up != 1 && extra_balls == 0)
	{
	 	/* show highest 1 ball score so far*/
		task_kill_gid (GID_BONUS_BUTTON_MONITOR);
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
		task_sleep_sec (4);
	

		/* Calculate lead into temp_score */
		score_zero (temp_score);
		score_copy (temp_score, scores[find_player_ranked(1)]);
		score_sub (temp_score, scores[find_player_ranked(2)]);
		sprintf_score (temp_score);


		if (check_if_last_ball_of_multiplayer_game ())
		{
			task_create_gid (GID_BONUS_TALKING, bonus_talking_task);
			task_sleep_sec (4);
		}
		else
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
			task_sleep_sec (4);
			
		
		}
	}

	if (check_if_last_ball_of_multiplayer_game ())
	{
		sound_send (SND_PLAYER_PIANO_UNUSED);
		dmd_alloc_low_clean ();
			
		sprintf("PLAYER %d WINS BY", find_player_ranked(1) + 1);
		font_render_string_center (&font_mono5, 64, 3, sprintf_buffer);
		sound_send (SND_GREED_MODE_BOOM);
		sprintf_score (temp_score);
		font_render_string_center (&font_fixed10, 64, 13, sprintf_buffer);
		font_render_string_center (&font_mono5, 64, 23, "CONGRATULATIONS");
		
		dmd_show_low ();
		task_sleep_sec (6);
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

CALLSET_ENTRY (bonus, serve_ball)
{
	/* Announce score to beat if in multiplayer and last ball*/
	if (check_if_last_ball_for_multiplayer ())	
		deff_start (DEFF_SCORE_TO_BEAT);
}

CALLSET_ENTRY (bonus, start_game)
{
	/* Initiliase hi score storage variables */
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

CALLSET_ENTRY (bonus, start_ball)
{
	/* Store the start ball store */
	score_zero (start_ball_score);
	score_copy (start_ball_score, current_score);
	quickdeath_timer_already_run = FALSE;
	quickdeath_timer_running = FALSE;
}

CALLSET_ENTRY (bonus, rank_change)
{
	if (!in_live_game && score_ranks[player_up-1] == 1)
		return;

	if (check_if_last_ball_for_multiplayer ())
	{
		/* Notify the player that they have taken the lead */
		deff_start (DEFF_IN_THE_LEAD);
	}
	else if (ball_up == system_config.balls_per_game 
		&& player_up == num_players 
		&& num_players > 1)
	{
		/* Notify the player that they have won */
		deff_start (DEFF_HOME_AND_DRY);
	}
}

CALLSET_ENTRY (bonus, end_ball)
{
	task_kill_gid (GID_QUICKDEATH);
}
