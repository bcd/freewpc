
#include <freewpc.h>

/* Super Skill Shot multiball rules */

U8 sssmb_initial_ramps_to_divert;
U8 sssmb_ramps_to_divert;
U8 sssmb_jackpot_value;

void sssmb_running_deff (void)
{
	for (;;)
	{
		score_update_start ();
		dmd_alloc_low_high ();
		dmd_clean_page_low ();

		font_render_string_center (&font_term6, 64, 4, "SKILL MULTIBALL");

		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);

		dmd_copy_low_to_high ();

		if (timer_find_gid (GID_SSSMB_DIVERT_DEBOUNCE))
		{
			sprintf ("SKILL SHOT SCORES JACKPOT");
		}
		else if (sssmb_ramps_to_divert == 0)
		{
			sprintf ("SHOOT LEFT RAMP NOW");
		}
		else if (sssmb_ramps_to_divert == 1)
		{
			sprintf ("1 RAMP FOR SKILL SHOT");
		}
		else
		{
			sprintf ("%d RAMPS FOR SKILL SHOT", sssmb_ramps_to_divert);
		}
		font_render_string_center (&font_var5, 64, 26, sprintf_buffer);

		dmd_show_low ();
		while (!score_update_required ())
		{
			task_sleep (TIME_66MS);
			dmd_show_other ();
		}
	}
}

void sssmb_jackpot_lit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 9, "GET THE");
	font_render_string_center (&font_fixed10, 64, 22, "JACKPOT");
	dmd_show_low ();
	sound_send (SND_SPIRAL_EB_LIT);
	task_sleep_sec (2);
	for (;;)
	{
		dmd_alloc_low_clean ();
		sprintf ("JACKPOT IS %d,000,000", sssmb_jackpot_value);
		font_render_string_center (&font_var5, 64, 24, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_100MS);
	}
}

void sssmb_jackpot_collected_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_term6, 64, 10, "SKILL JACKPOT");
	printf_millions (sssmb_jackpot_value);
	font_render_string_center (&font_fixed6, 64, 21, sprintf_buffer);
	sound_send (SND_SKILL_SHOT_CRASH_3);
	dmd_show_low ();
	task_sleep_sec (1);
	sound_send (SND_SKILL_SHOT_CRASH_3);
	task_sleep_sec (1);
	deff_exit ();
}


void sssmb_relight_all_jackpots (void)
{
	flag_on (FLAG_SSSMB_RED_JACKPOT);
	flag_on (FLAG_SSSMB_ORANGE_JACKPOT);
	flag_on (FLAG_SSSMB_YELLOW_JACKPOT);
	sssmb_initial_ramps_to_divert++;
}


void sssmb_award_jackpot (void)
{
	score_1M (sssmb_jackpot_value);
	deff_start (DEFF_SSSMB_JACKPOT_COLLECTED);
	sound_send (SND_EXPLOSION_1);

	/* score it */

	if (sssmb_jackpot_value < 100)
		sssmb_jackpot_value += 10;
	sssmb_ramps_to_divert = sssmb_initial_ramps_to_divert;

	if (!flag_test (FLAG_SSSMB_RED_JACKPOT)
		&& !flag_test (FLAG_SSSMB_ORANGE_JACKPOT)
		&& !flag_test (FLAG_SSSMB_YELLOW_JACKPOT))
	{
		sssmb_relight_all_jackpots ();
	}
}


void sssmb_jackpot_ready_task (void)
{
	deff_start (DEFF_SSSMB_JACKPOT_LIT);
	sound_send (SND_HEEHEE);
	task_sleep_sec (4);
	sound_send (SND_FASTER);
	sssmb_jackpot_value--;
	task_sleep_sec (2);
	sssmb_jackpot_value--;
	task_sleep_sec (2);
	sssmb_jackpot_value--;
	sound_send (SND_PUT_IT_BACK_2);
	task_sleep_sec (2);
	sssmb_jackpot_value--;
	task_sleep_sec (2);
	sssmb_jackpot_value--;
	sound_send (SND_OH_NO);
	task_sleep_sec (2);
	task_exit ();
}


void sssmb_start (void)
{
	if (!flag_test (FLAG_SSSMB_RUNNING))
	{
		effect_update_request ();
		flag_on (FLAG_SSSMB_RUNNING);
		flag_on (FLAG_SSSMB_RED_JACKPOT);
		flag_on (FLAG_SSSMB_ORANGE_JACKPOT);
		flag_on (FLAG_SSSMB_YELLOW_JACKPOT);
		sssmb_initial_ramps_to_divert = 1;
		sssmb_ramps_to_divert = 0;
		sssmb_jackpot_value = 20;
		device_multiball_set (2);
		ballsave_add_time (15);
	}
}


void sssmb_stop (void)
{
	if (flag_test (FLAG_SSSMB_RUNNING))
	{
		flag_off (FLAG_SSSMB_RUNNING);
		flag_off (FLAG_SSSMB_RED_JACKPOT);
		flag_off (FLAG_SSSMB_ORANGE_JACKPOT);
		flag_off (FLAG_SSSMB_YELLOW_JACKPOT);
		timer_kill_gid (GID_SSSMB_DIVERT_DEBOUNCE);
		task_kill_gid (GID_SSSMB_JACKPOT_READY);
		deff_stop (DEFF_SSSMB_RUNNING);
		music_refresh ();
	}
}

CALLSET_ENTRY (sssmb, display_update)
{
	if (flag_test (FLAG_SSSMB_RUNNING))
		deff_start_bg (DEFF_SSSMB_RUNNING, 0);
}

CALLSET_ENTRY (sssmb, music_refresh)
{
	if (flag_test (FLAG_SSSMB_RUNNING))
		music_request (MUS_SPIRAL_ROUND, PRI_GAME_MODE1 + 9);
};


CALLSET_ENTRY (sssmb, door_start_super_skill)
{
	sssmb_start ();
}


CALLSET_ENTRY (sssmb, single_ball_play)
{
	sssmb_stop ();
}

CALLSET_ENTRY (sssmb, skill_red)
{
	if (flag_test (FLAG_SSSMB_RUNNING)
		&& flag_test (FLAG_SSSMB_RED_JACKPOT))
	{
		flag_off (FLAG_SSSMB_RED_JACKPOT);
		sssmb_award_jackpot ();
	}
}

CALLSET_ENTRY (sssmb, skill_orange)
{
	if (flag_test (FLAG_SSSMB_RUNNING)
		&& flag_test (FLAG_SSSMB_ORANGE_JACKPOT))
	{
		flag_off (FLAG_SSSMB_ORANGE_JACKPOT);
		sssmb_award_jackpot ();
	}
}

CALLSET_ENTRY (sssmb, skill_yellow)
{
	if (flag_test (FLAG_SSSMB_RUNNING)
		&& flag_test (FLAG_SSSMB_YELLOW_JACKPOT))
	{
		flag_off (FLAG_SSSMB_YELLOW_JACKPOT);
		sssmb_award_jackpot ();
	}
}

CALLSET_ENTRY (sssmb, sw_left_ramp_exit)
{
	if (flag_test (FLAG_SSSMB_RUNNING))
	{
		if (sssmb_ramps_to_divert == 0)
		{
			if (!timer_find_gid (GID_SSSMB_DIVERT_DEBOUNCE))
			{
				timer_start_free (GID_SSSMB_DIVERT_DEBOUNCE, TIME_6S);
				ramp_divert ();
			}
		}
		else
		{
			sssmb_ramps_to_divert--;
		}
		score_update_required ();
	}
}

CALLSET_ENTRY (sssmb, sw_shooter)
{
	if (flag_test (FLAG_SSSMB_RUNNING)
		&& timer_find_gid (GID_SSSMB_DIVERT_DEBOUNCE))
	{
		extern U8 skill_switch_reached;
		skill_switch_reached = 0;
		/* TODO: handle case where red jackpot switch is tripped but
		ball falls back to plunger lane */
		task_create_gid1 (GID_SSSMB_JACKPOT_READY, sssmb_jackpot_ready_task);
	}
}

CALLSET_ENTRY (sssmb, any_skill_switch)
{
	dbprintf ("Jackpot ready cancelled\n");
	task_kill_gid (GID_SSSMB_JACKPOT_READY);
	deff_stop (DEFF_SSSMB_JACKPOT_LIT);
}

CALLSET_ENTRY (sssmb, start_game)
{
	sssmb_stop ();
}


CALLSET_ENTRY (sssmb, sw_buyin_button)
{
#if 0
	if (in_live_game)
		sssmb_start ();
#endif
}

