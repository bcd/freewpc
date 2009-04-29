
#include <freewpc.h>

U8 generic_combo_timer;

U8 combo_count;

U8 right_ramp_count;

static U8 combo_score_ladder[] = {
	SC_100K, SC_200K, SC_300K, SC_400K, SC_500K
};

void combo_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d WAY COMBO", combo_count);
	font_render_string_center (&font_fixed6, 64, 5, sprintf_buffer);
	sprintf_score (score_deff_get ());
	font_render_string_center (&font_fixed6, 64, 15, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void combo_reset (void)
{
	combo_count = 0;
	right_ramp_count = 0;
}

void combo_award (void)
{	
	U8 value;

	if (combo_count > 5)
		value = SC_500K;
	else
		value = combo_score_ladder[combo_count - 1];
	score_multiple (value, right_ramp_count+1);
	deff_start (DEFF_COMBO);
	sound_start (ST_SAMPLE, SND_FIREWORK_LAUNCH2, SL_1S, PRI_GAME_QUICK3);
}

void combo_running (void)
{
	generic_combo_timer = TIME_4S / TIME_500MS;
	while (generic_combo_timer > 0)
	{
		task_sleep (TIME_500MS);
		generic_combo_timer--;
	}
	combo_reset ();
	task_exit ();
}

void combo_shot (void)
{
	bounded_increment (combo_count, 99);
	if (task_find_gid (GID_COMBO))
		combo_award ();
	task_recreate_gid (GID_COMBO, combo_running);
}

CALLSET_ENTRY (combo, left_ramp_shot, striker_shot)
{
	right_ramp_count = 0;
	combo_shot ();
}

CALLSET_ENTRY (combo, right_ramp_shot)
{
	bounded_increment (right_ramp_count, 5);
	combo_shot ();
}

CALLSET_ENTRY (combo, start_ball)
{
	combo_reset ();
}

