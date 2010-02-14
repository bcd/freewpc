
#include <freewpc.h>


extern inline void score_deff_begin (const font_t *font, U8 x, U8 y, const char *text)
{
	score_update_start ();
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (font, x, y, text);
	sprintf_current_score ();
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_copy_low_to_high ();
}


extern inline void score_deff_end (task_ticks_t flash_rate)
{
	dmd_show_low ();
	while (!score_update_required ())
	{
		task_sleep (flash_rate);
		dmd_show_other ();
	}
}


extern inline bool multiball_mode_start (U8 flag, U8 deff, U8 leff, U8 music)
{
	if (!flag_test (flag))
	{
		flag_on (flag);
		return TRUE;
	}
	else
		return FALSE;
}


extern inline bool multiball_mode_stop (U8 flag, U8 deff, U8 leff, U8 music)
{
	if (flag_test (flag))
	{
		flag_off (flag);
		return TRUE;
	}
	else
		return FALSE;
}



__local__ U8 chaosmb_level;

__local__ U8 chaosmb_hits_to_relight;

struct {
	const char *shot_name;
	U8 jackpot_value;
	U8 lamp_num;
} chaosmb_shots[] = {
	{ "LEFT RAMP", 15, },
	{ "RIGHT RAMP", 20, },
	{ "PIANO", 25,  },
	{ "HITCHHIKER", 30 },
	{ "POWER PAYOFF", 40 },
	{ "DEAD END", 50 },
};


void chaos_jackpot_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 9, "CHAOS");
	font_render_string_center (&font_fixed10, 64, 23, "JACKPOT");
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (15, TIME_100MS);
	deff_exit ();
}


void chaosmb_running_deff (void)
{
	for (;;)
	{
		score_deff_begin (&font_fixed6, 64, 4, "CHAOS MULTIBALL");
		if (chaosmb_hits_to_relight == 0)
		{
			sprintf ("SHOOT %s", chaosmb_shots[chaosmb_level].shot_name);
			font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
		}
		else if (chaosmb_hits_to_relight == 1)
		{
			font_render_string_center (&font_var5, 64, 27,
				"HIT CLOCK TO LIGHT JACKPOT");
		}
		else
		{
			sprintf ("HIT CLOCK %d MORE TIMES", chaosmb_hits_to_relight);
			font_render_string_center (&font_var5, 64, 27, sprintf_buffer);
		}
		score_deff_end (TIME_100MS);
	}
}


void chaosmb_score_jackpot (void)
{
	chaosmb_level++;
	chaosmb_hits_to_relight = chaosmb_level * 2;
	deff_start (DEFF_CHAOS_JACKPOT);
	sound_send (SND_EXPLOSION_1);
}

void chaosmb_start (void)
{
	if (multiball_mode_start (FLAG_CHAOSMB_RUNNING, DEFF_CHAOSMB_RUNNING, 0, MUS_SPIRAL_ROUND))
	{
		chaosmb_level = 0;
		chaosmb_hits_to_relight = 0;
		device_multiball_set (3);
		ballsave_add_time (10);
	}
}

void chaosmb_stop (void)
{
	if (multiball_mode_stop (FLAG_CHAOSMB_RUNNING, DEFF_CHAOSMB_RUNNING, 0, MUS_SPIRAL_ROUND))
	{
	}
}


void chaosmb_check_level (U8 level)
{
	if (flag_test (FLAG_CHAOSMB_RUNNING)
		&& (chaosmb_level == level)
		&& (chaosmb_hits_to_relight == 0))
	{
		chaosmb_score_jackpot ();
	}
}


CALLSET_ENTRY (chaosmb, display_update)
{
	if (flag_test (FLAG_CHAOSMB_RUNNING))
		deff_start_bg (DEFF_CHAOSMB_RUNNING, PRI_GAME_MODE6);
}

CALLSET_ENTRY (chaosmb, music_refresh)
{
	if (flag_test (FLAG_CHAOSMB_RUNNING))
		music_request (MUS_SPIRAL_ROUND, PRI_GAME_MODE6);
}


CALLSET_ENTRY (chaosmb, door_start_clock_chaos)
{
	chaosmb_start ();
}


CALLSET_ENTRY (chaosmb, sw_left_ramp_exit)
{
	chaosmb_check_level (0);
}

CALLSET_ENTRY (chaosmb, sw_right_ramp)
{
	chaosmb_check_level (1);
}

CALLSET_ENTRY (chaosmb, sw_piano)
{
	chaosmb_check_level (2);
}

CALLSET_ENTRY (chaosmb, sw_hitchhiker)
{
	chaosmb_check_level (3);
}

CALLSET_ENTRY (chaosmb, sw_power_payoff)
{
	chaosmb_check_level (4);
}

CALLSET_ENTRY (chaosmb, sw_dead_end)
{
	chaosmb_check_level (5);
}

CALLSET_ENTRY (chaosmb, sw_clock_target)
{
	if (flag_test (FLAG_CHAOSMB_RUNNING)
		&& (chaosmb_hits_to_relight > 0))
	{
		chaosmb_hits_to_relight--;
	}
}

CALLSET_ENTRY (chaosmb, single_ball_play)
{
	chaosmb_stop ();
}

CALLSET_ENTRY (chaosmb, start_player)
{
	chaosmb_level = 0;
	chaosmb_hits_to_relight = 0;
}

