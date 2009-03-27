
#include <freewpc.h>

__local__ U8 dog_targets;

__local__ U8 modes_started;

void mode_draw_target_status (U8 targets)
{
	dmd_alloc_low_high ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 48, 12, "D");
	font_render_string_center (&font_fixed10, 64, 12, "O");
	font_render_string_center (&font_fixed10, 80, 12, "G");
	dmd_flip_low_high ();

	dmd_clean_page_low ();
	if (targets & 1)
		font_render_string_center (&font_fixed10, 48, 12, "D");
	if (targets & 2)
		font_render_string_center (&font_fixed10, 64, 12, "O");
	if (targets & 4)
		font_render_string_center (&font_fixed10, 80, 12, "G");

	if (targets < 0x7)
	{
		font_render_string_center (&font_var5, 64, 27, "COMPLETE TO LIGHT MODE");
	}
	else
	{
		font_render_string_center (&font_var5, 64, 27, "MODE START IS LIT");
	}

	dmd_flip_low_high ();
	dmd_show2 ();

}

void mode_target_status_deff (void)
{
	mode_draw_target_status (dog_targets);
	task_sleep_sec (2);
	deff_exit ();
}

void mode_lit_deff (void)
{
	mode_draw_target_status (0x7);
	task_sleep_sec (3);
	deff_exit ();
}

void mode_started_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "MODE START");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void mode_finished_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "MODE FINISHED");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void mode_reset_dog_targets (void)
{
	dog_targets = 0;
	flag_off (FLAG_MODE_LIT);
}

void mode_collect_dog_target (U8 target)
{
	sound_start (ST_SAMPLE, SND_DRIBBLE, SL_500MS, PRI_GAME_QUICK1);
	if (!flag_test (FLAG_MODE_LIT))
	{
		dog_targets |= target;
		if (dog_targets == 0x7)
		{
			flag_on (FLAG_MODE_LIT);
			deff_start (DEFF_MODE_LIT);
			score (SC_50K);
		}
		else
		{
			deff_start (DEFF_MODE_TARGET_STATUS);
			score (SC_25K);
		}
	}
	else
	{
		score (SC_5K);
	}
}

bool mode_can_be_started (void)
{
	return TRUE;
}

void mode_start (void)
{
	if (flag_test (FLAG_MODE_LIT))
	{
		mode_reset_dog_targets ();
		score (SC_100K);
		/* start mode */
		deff_start (DEFF_MODE_STARTED);
		modes_started++;
	}
}

CALLSET_ENTRY (mode, sw_striker_1)
{
	mode_collect_dog_target (0x1);
}

CALLSET_ENTRY (mode, sw_striker_2)
{
	mode_collect_dog_target (0x2);
}

CALLSET_ENTRY (mode, sw_striker_3)
{
	mode_collect_dog_target (0x4);
}

CALLSET_ENTRY (mode, striker_shot)
{
	mode_start ();
}

CALLSET_ENTRY (mode, start_player)
{
	modes_started = 0;
	mode_reset_dog_targets ();
}

CALLSET_ENTRY (mode, lamp_update)
{
	lamp_flash_if (LM_STRIKER,
		mode_can_be_started () && flag_test (FLAG_MODE_LIT));
}

CALLSET_ENTRY (mode, start_ball)
{
}

