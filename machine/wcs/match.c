
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



/* FINAL_MATCH_START
MATCH_LOST
FINAL_MATCH_WON
MATCH_STARTED
MATCH_RESTART
MATCH_RESTART_SPIN */

void match_started_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("MATCH %d", match_number);
	font_render_string_center (&font_mono5, 64, 4, sprintf_buffer);
	sprintf ("%d YARDS TO LIGHT GOAL", yards_to_go);
	font_render_string_center (&font_var5, 64, 16, sprintf_buffer);
	sprintf ("%d GOALS NEEDED TO WIN", match_params[match_number].goals_needed);
	font_render_string_center (&font_var5, 64, 26, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (5);
	deff_exit ();
}

void yards_awarded_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d MORE YARDS", yards_to_go);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, 20, "TO LIGHT GOAL");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void goal_lit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "GOAL IS LIT");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void goal_scored_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d GOALS", match_goals_scored);
	font_render_string_center (&font_fixed10, 64, 10, sprintf_buffer);
	sprintf ("WIN MATCH AT %d", match_params[match_number].goals_needed);
	font_render_string_center (&font_mono5, 64, 22, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void match_won_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d GOALS", match_goals_scored);
	font_render_string_center (&font_fixed10, 64, 10, sprintf_buffer);
	font_render_string_center (&font_mono5, 64, 22, "YOU WIN THE MATCH");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void match_lost_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 10, "SHOOT SPINNER");
	font_render_string_center (&font_mono5, 64, 22, "TO RESTART MATCH");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void goal_unlit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 10, "SHOOT RAMPS TO");
	font_render_string_center (&font_mono5, 64, 22, "RELIGHT GOAL");
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
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
		sound_send (MUS_TEA_PARTY_WON);
		deff_start (DEFF_MATCH_WON);
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
	deff_start (DEFF_MATCH_STARTED);
}

void wcs_restart_match (void)
{
	spinners_needed = 0;
	wcs_restart_timer ();
	/* deff_start (DEFF_MATCH_RESTART); */
}


static inline bool match_in_progress_p (void)
{
	return (yards_to_go == 0) && (spinners_needed == 0);
}

void award_match_goal (void)
{
	if (match_in_progress_p ())
	{
		++match_goals_scored;
		sound_send (SPCH_GOALLL);
		deff_start (DEFF_GOAL_SCORED);

		if (match_goals_scored ==
			match_params[match_number].goals_needed)
		{
			wcs_finish_match ();
			wcs_start_match ();
		}
	}
	else if (yards_to_go == 0)
	{
		deff_start (DEFF_GOAL_UNLIT);
	}
}

void light_match_goal (void)
{
	yards_to_go = 0;
	sound_send (MUS_GOAL_IS_LIT);
	deff_start (DEFF_GOAL_LIT);
}


void advance_yards (U8 yards)
{
	sound_send (SND_CHALKBOARD);
	if (yards_to_go == 0)
	{
	}
	else if (yards >= yards_to_go)
	{
		light_match_goal ();
	}
	else
	{
		deff_start (DEFF_YARDS_AWARDED);
		yards_to_go -= yards;
	}
}

static inline void lamp_update_goals (U8 count, U8 lamp)
{
	if (match_goals_scored >= count)
		lamp_tristate_on (lamp);
	else if (match_params[match_number].goals_needed >= count)
		lamp_tristate_flash (lamp);
	else
		lamp_tristate_off (lamp);
}

CALLSET_ENTRY (wcs_match, lamp_update)
{
	lamp_update_goals (1, LM_1_GOAL);
	lamp_update_goals (2, LM_2_GOALS);
	lamp_update_goals (3, LM_3_GOALS);
	lamp_update_goals (4, LM_4_GOALS);
	if (match_timer)
	{
		lamp_tristate_off (LM_L_LOOP_BUILD);
		if (yards_to_go)
		{
			lamp_tristate_flash (LM_R_RAMP_BUILD);
			lamp_tristate_flash (LM_L_RAMP_BUILD);
			lamp_tristate_flash (LM_STRIKER_BUILD);
			lamp_tristate_off (LM_GOAL);
		}
		else
		{
			lamp_tristate_off (LM_R_RAMP_BUILD);
			lamp_tristate_off (LM_L_RAMP_BUILD);
			lamp_tristate_off (LM_STRIKER_BUILD);
			lamp_tristate_flash (LM_GOAL);
		}
	}
	else
	{
		lamp_tristate_flash (LM_L_LOOP_BUILD);
		lamp_tristate_off (LM_R_RAMP_BUILD);
		lamp_tristate_off (LM_L_RAMP_BUILD);
		lamp_tristate_off (LM_STRIKER_BUILD);
		lamp_tristate_off (LM_GOAL);
	}
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

CALLSET_ENTRY (wcs_match, sw_goalie_target)
{
	if (match_in_progress_p ())
	{
		// speech call here
	}
	// hit goalie sound here
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
	sound_send (SND_SPINNER);
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

