
#include <freewpc.h>

struct match_params
{
	U8 goal_starts_lit;
	U8 goals_needed;
	U8 yards_per_goal;
	U8 total_time;
	U8 goalie_difficulty;
};

const struct match_params match_params[] = {
	/* Round 0 - not used */ {},
	{ TRUE,  2, 100, 45, 0 },
	{ TRUE,  3, 100, 45, 0 },
	{ FALSE, 3, 100, 45, 0 },

	{ FALSE, 3, 100, 40, 0 },
	{ FALSE, 3, 100, 35, 0 },
	{ FALSE, 4, 100, 35, 0 },

	{ FALSE, 4, 100, 30, 1 },
	{ FALSE, 5, 100, 30, 1 },
	{ FALSE, 5, 100, 30, 2 },
};

__local__ U8 match_number;

__local__ U8 match_goals_scored;

__local__ U8 yards_to_go;

U8 match_timer;

__local__ U8 spinners_needed;

__local__ U8 spinners_needed_next;


void goal_flash_multiple (void)
{
	for (;;)
	{
		sol_request_async (FLASH_GOAL);
		task_sleep (TIME_200MS);
	}
}


void goal_scored_leff (void)
{
	leff_create_peer (goal_flash_multiple);
	sol_request_async (FLASH_GOAL_CAGE_TOP);
	task_sleep (TIME_500MS);
	sol_request_async (FLASH_GOAL_CAGE_TOP);
	task_sleep_sec (1);
	task_kill_peers ();
	leff_exit ();
}


void flash_many (void)
{
	for (;;)
	{
		task_sleep (TIME_200MS);
	}
}


void match_won_leff (void)
{
	leff_create_peer (flash_many);
	task_sleep_sec (1);
	task_kill_peers ();
	leff_exit ();
}


void match_running_deff (void)
{
	for (;;)
	{
		score_update_start ();

		dmd_alloc_low_clean ();

		font_render_string_left (&font_var5, 1, 9, "YARDS");
		sprintf ("%d", yards_to_go);
		font_render_string_left (&font_fixed10, 1, 16, sprintf_buffer);

		font_render_string_right (&font_var5, 1, 9, "GOALS");
		sprintf ("%d", match_goals_scored);
		font_render_string_right (&font_fixed10, 127, 16, sprintf_buffer);

		sprintf_current_score ();
		font_render_string_center (&font_mono5, 64, 3, sprintf_buffer);

		dmd_show_low ();

		while (!score_update_required ())
			task_sleep (TIME_133MS);
	}
}


void wcs_start_final_match (void)
{
	/* deff_start (DEFF_FINAL_MATCH_START); */
}


void wcs_restart_timer (void)
{
	match_timer = match_params[match_number].total_time;
}

void wcs_match_lost (void)
{
	spinners_needed = spinners_needed_next;
	if (spinners_needed_next < 50)
	{
		spinners_needed_next += 5;
	}
	deff_stop (DEFF_MATCH_RUNNING);
	/* deff_start (DEFF_MATCH_LOST); */
}


void wcs_finish_match (void)
{
	if (match_number >= 9)
	{
		/* deff_start (DEFF_FINAL_MATCH_WON); */
		match_number = 0;
	}
	else
	{
		/* deff_start (DEFF_MATCH_WON); */
	}
}


void wcs_start_match (void)
{
	match_number++;
	if (match_number >= 9)
	{
		wcs_start_final_match ();
		return;
	}

	match_goals_scored = 0;
	yards_to_go = match_params[match_number].yards_per_goal;
	wcs_restart_timer ();
	spinners_needed = 0;
	spinners_needed_next = 20;
	deff_restart (DEFF_MATCH_RUNNING);
	/* deff_start (DEFF_MATCH_STARTED); */
}

void wcs_restart_match (void)
{
	spinners_needed = 0;
	wcs_restart_timer ();
	deff_restart (DEFF_MATCH_RUNNING);
	/* deff_start (DEFF_MATCH_RESTART); */
}

void award_match_goal (void)
{
	if ((yards_to_go == 0) && (spinners_needed == 0))
	{
		++match_goals_scored;
		/* deff_start (DEFF_GOAL_SCORED); */

		if (match_goals_scored ==
			match_params[match_number].goals_needed)
		{
			wcs_finish_match ();
			wcs_start_match ();
		}
	}
}

void light_match_goal (void)
{
	yards_to_go = 0;
	/* deff_start (DEFF_GOAL_LIT); */
}


void advance_yards (U8 yards)
{
	if (yards_to_go == 0)
	{
	}
	else if (yards >= yards_to_go)
	{
		light_match_goal ();
	}
	else
	{
		/* deff_start (DEFF_YARDS_AWARDED); */
		yards_to_go -= yards;
	}
}

CALLSET_ENTRY (wcs_match, lamp_update)
{
}

CALLSET_ENTRY (wcs_match, start_player)
{
	match_number = 0;
	wcs_start_match ();
}

CALLSET_ENTRY (wcs_match, goal_shot)
{
	award_match_goal ();
}

CALLSET_ENTRY (wcs_match, striker_shot)
{
	advance_yards (10);
}

CALLSET_ENTRY (wcs_match, left_ramp_shot)
{
	advance_yards (20);
}

CALLSET_ENTRY (wcs_match, right_ramp_shot)
{
	advance_yards (30);
}

CALLSET_ENTRY (wcs_match, any_rollover)
{
	advance_yards (5);
}

CALLSET_ENTRY (wcs_match, sw_spinner_slow)
{
	if (spinners_needed)
	{
		if (--spinners_needed == 0)
		{
			wcs_restart_match ();
		}
		else
		{
			/* deff_start (DEFF_MATCH_RESTART_SPIN); */
		}
	}
}

