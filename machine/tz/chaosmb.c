
#include <freewpc.h>



extern inline void score_deff_begin (const font_t *font, U8 x, U8 y, const char *text)
{
	score_update_start ();
	dmd_alloc_low_high ();
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


extern inline bool multiball_mode_start (U8 flag, U8 deff, U8 leff)
{
	if (!flag_test (flag))
	{
		flag_on (flag);
		if (deff != DEFF_NULL)
			deff_start (deff);
		if (leff != LEFF_NULL)
			leff_start (leff);
		return TRUE;
	}
	else
		return FALSE;
}


extern inline bool multiball_mode_stop (U8 flag, U8 deff, U8 leff)
{
	if (flag_test (flag))
	{
		flag_off (flag);
		if (deff != DEFF_NULL)
			deff_stop (deff);
		if (leff != LEFF_NULL)
			leff_stop (leff);
		return TRUE;
	}
	else
		return FALSE;
}



U8 chaosmb_level;

U8 chaosmb_hits_to_relight;

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

void chaosmb_lamp_update (void)
{
	if (flag_test (FLAG_CHAOSMB_RUNNING))
	{
	}
}


void chaosmb_running_deff (void)
{
	for (;;)
	{
		score_deff_begin (&font_fixed6, 64, 4, "CHAOS MULTIBALL");
		font_render_string_center (&font_var5, 64, 27, "SHOOT SOMETHING");
		score_deff_end (TIME_100MS);
	}
}


void chaosmb_score_jackpot (void)
{
	chaosmb_level++;
	chaosmb_hits_to_relight = chaosmb_level * 2;
}



void chaosmb_start (void)
{
	if (multiball_mode_start (FLAG_CHAOSMB_RUNNING, DEFF_CHAOSMB_RUNNING, 0))
	{
		chaosmb_level = 0;
		chaosmb_hits_to_relight = 0;
	}
}

void chaosmb_stop (void)
{
	if (multiball_mode_stop (FLAG_CHAOSMB_RUNNING, DEFF_CHAOSMB_RUNNING, 0))
	{
	}
}


CALLSET_ENTRY (chaosmb, sw_left_ramp_exit)
{
	if (flag_test (FLAG_CHAOSMB_RUNNING)
		&& (chaosmb_level == 0))
	{
		chaosmb_score_jackpot ();
	}
}

CALLSET_ENTRY (chaosmb, sw_right_ramp)
{
}

CALLSET_ENTRY (chaosmb, sw_piano)
{
}

CALLSET_ENTRY (chaosmb, sw_hitchhiker)
{
}

CALLSET_ENTRY (chaosmb, sw_power_payoff)
{
}

CALLSET_ENTRY (chaosmb, sw_dead_end)
{
}

CALLSET_ENTRY (chaosmb, sw_clock_target)
{
	if (flag_test (FLAG_CHAOSMB_RUNNING)
		&& (chaosmb_hits_to_relight > 0))
	{
		chaosmb_hits_to_relight--;
		if (chaosmb_hits_to_relight == 0)
		{
			chaosmb_level++;
		}
	}
}

