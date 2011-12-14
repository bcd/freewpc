
#include <freewpc.h>

__local__ U8 lock_count;

__local__ U8 mb_spinner_count;

score_t mb_jackpot_value;

#define SPINNERS_FOR_SUPER 20


void ball_locked_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_draw_thin_border (dmd_low_buffer);
	sprintf ("BALL %d LOCKED", lock_count);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void multiball_lit_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	font_render_string_center (&font_term6, 64, 9, "SHOOT FINAL DRAW");
	font_render_string_center (&font_term6, 64, 21, "FOR MULTIBALL");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void multiball_start_deff (void)
{
	sprintf ("MULTIBALL");
	flash_and_exit_deff (30, TIME_100MS);
}


void multiball_running_deff (void)
{
	for (;;)
	{
		score_update_start ();
		dmd_alloc_pair ();
		dmd_clean_page_low ();
		font_render_string_center (&font_mono5, 64, 5, "MULTIBALL");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		dmd_copy_low_to_high ();

		font_render_string_center (&font_var5, 64, 27, "ALL SHOTS = 2X");

		dmd_show_low ();
		do {
			task_sleep (TIME_100MS);
			dmd_show_other ();
			task_sleep (TIME_100MS);
			dmd_show_other ();
		} while (!score_update_required ());
	}
}


void multiball_jackpot_deff (void)
{
	sprintf ("JACKPOT");
	flash_and_exit_deff (20, TIME_100MS);
}

void multiball_super_jackpot_deff (void)
{
	sprintf ("SUPER JACKPOT");
	flash_and_exit_deff (30, TIME_100MS);
}

void multiball_spinner_deff (void)
{
	deff_exit ();
}


void mb_reset (void)
{
	flag_off (FLAG_LEFT_EJECT_LOCK);
	flag_off (FLAG_RIGHT_EJECT_LOCK);
	flag_off (FLAG_MULTIBALL_LIT);
	flag_off (FLAG_MULTIBALL_RUNNING);
	flag_off (FLAG_JACKPOT_LIT);
	flag_off (FLAG_SUPER_JACKPOT_LIT);
}

void mb_light (void)
{
	if (!flag_test (FLAG_MULTIBALL_LIT))
	{
		flag_on (FLAG_MULTIBALL_LIT);
		deff_start (DEFF_MULTIBALL_LIT);
	}
}

void mb_light_jackpot (void)
{
	flag_on (FLAG_JACKPOT_LIT);
}

void mb_light_super_jackpot (void)
{
	flag_on (FLAG_SUPER_JACKPOT_LIT);
}

void mb_start (void)
{
	flag_off (FLAG_MULTIBALL_LIT);
	flag_on (FLAG_MULTIBALL_RUNNING);
	/* TODO - init base jackpot value */
	/* TODO - add one more ball to play */
	mb_light_jackpot ();
	flag_off (FLAG_SUPER_JACKPOT_LIT);
	mb_spinner_count = 0;
	VOIDCALL (goal_count_lamp_update);
	deff_start (DEFF_MULTIBALL_START);
	if (flag_test (FLAG_LEFT_EJECT_LOCK))
		device_unlock_ball (device_entry (DEVNO_LEFT_EJECT));
	if (flag_test (FLAG_RIGHT_EJECT_LOCK))
		device_unlock_ball (device_entry (DEVNO_RIGHT_EJECT));
}

void mb_start_surprise (void)
{
	mb_start ();
}

static void mb_advance_spinner (void)
{
	if (!flag_test (FLAG_SUPER_JACKPOT_LIT) &&
		(++mb_spinner_count == SPINNERS_FOR_SUPER))
	{
		mb_light_super_jackpot ();
	}
}

static void mb_award_jackpot (void)
{
	if (!flag_test (FLAG_JACKPOT_LIT))
	{
		flag_off (FLAG_JACKPOT_LIT);
		/* score jackpot value */
		/* increase jackpot value */
	}
}

static void mb_award_super_jackpot (void)
{
	if (!flag_test (FLAG_SUPER_JACKPOT_LIT))
	{
		flag_off (FLAG_SUPER_JACKPOT_LIT);
	}
}

static void mb_award_lock (U8 devno)
{
	device_lock_ball (device_entry (devno));
	sound_start (ST_SAMPLE, SND_LOCK_MAGNET, SL_4S, PRI_GAME_QUICK5);
	deff_start (DEFF_BALL_LOCKED);
	lock_count++;
}


CALLSET_ENTRY (mb, dev_left_eject_enter)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING) && !flag_test_and_set (FLAG_LEFT_EJECT_LOCK))
		mb_award_lock (DEVNO_LEFT_EJECT);
}

CALLSET_ENTRY (mb, dev_right_eject_enter)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING) && !flag_test_and_set (FLAG_RIGHT_EJECT_LOCK))
		mb_award_lock (DEVNO_RIGHT_EJECT);
}

CALLSET_ENTRY (mb, dev_left_eject_surprise_release)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING) && flag_test (FLAG_LEFT_EJECT_LOCK))
	{
		flag_off (FLAG_LEFT_EJECT_LOCK);
		device_disable_lock (device_entry (DEVNO_LEFT_EJECT));
		mb_start_surprise ();
	}
}

CALLSET_ENTRY (mb, dev_right_eject_surprise_release)
{
	if (!flag_test (FLAG_MULTIBALL_RUNNING) && flag_test (FLAG_RIGHT_EJECT_LOCK))
	{
		flag_off (FLAG_RIGHT_EJECT_LOCK);
		device_disable_lock (device_entry (DEVNO_RIGHT_EJECT));
		mb_start_surprise ();
	}
}

CALLSET_ENTRY (mb, dev_tv_popper_enter)
{
	if (flag_test (FLAG_MULTIBALL_LIT))
	{
		mb_start ();
	}
	else if (flag_test (FLAG_MULTIBALL_RUNNING)
		&& flag_test (FLAG_SUPER_JACKPOT_LIT))
	{
		mb_award_super_jackpot ();
	}
}

CALLSET_ENTRY (mb, left_loop_shot)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING)
		&& !flag_test (FLAG_JACKPOT_LIT))
		mb_light_jackpot ();
}

CALLSET_ENTRY (mb, goal_shot)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING)
		&& flag_test (FLAG_JACKPOT_LIT))
	{
		mb_award_jackpot ();
	}
}

CALLSET_ENTRY (mb, sw_spinner_slow)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
		mb_advance_spinner ();
}

CALLSET_ENTRY (mb, lamp_update)
{
	lamp_flash_if (LM_FINAL_DRAW,
		(flag_test (FLAG_MULTIBALL_LIT) || flag_test (FLAG_SUPER_JACKPOT_LIT)));
}

CALLSET_ENTRY (mb, music_refresh)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		music_request (MUS_MULTIBALL, PRI_GAME_MODE3);
	}
}

CALLSET_ENTRY (mb, display_update)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
		deff_start_bg (DEFF_MULTIBALL_RUNNING, PRI_GAME_MODE3);
}

CALLSET_ENTRY (mb, start_player, single_ball_play, end_ball)
{
	mb_reset ();
}

