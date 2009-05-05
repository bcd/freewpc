
#include <freewpc.h>

__local__ U8 tv_count;

U8 tv_award_selected;

struct tv_award
{
	const char *name;
	void (*award) (void);
	U8 static_prob;
	void (*dynamic_prob) (void);
};

void tv_award_points (void)
{
	score (SC_250K);
}

void tv_award_start_mode (void)
{
}

void tv_award_start_quickmb (void)
{
}

void tv_award_hurryup (void)
{
}

void tv_award_add_ultra (void)
{
	VOIDCALL (ultra_add_shot);
}

void tv_award_light_extra_ball (void)
{
}

void tv_award_add_time (void)
{
}

struct tv_award tv_award_table[] = {
	{ "250,000", tv_award_points },
	{ "START MODE", tv_award_start_mode },
	{ "QUICK MULTIBALL", tv_award_start_quickmb },
	{ "HURRY-UP", tv_award_hurryup },
	{ "LIGHT ULTRA MODE", tv_award_add_ultra },
	{ "LIGHT EXTRA BALL", tv_award_light_extra_ball },
	{ "ADD 30 SECONDS", tv_award_add_time },
};


void tv_award_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("TV AWARD %d", tv_count);
	font_render_string_center (&font_fixed10, 64, 9, sprintf_buffer);
	sprintf ("%s", tv_award_table[tv_award_selected].name);
	font_render_string_center (&font_fixed10, 64, 22, sprintf_buffer);
	dmd_draw_border (dmd_low_buffer);
	dmd_show_low ();
	sample_start (SND_TV_STATIC, SL_3S);
	task_sleep_sec (3);
	deff_exit ();
}


static bool tv_can_be_collected (void)
{
	return flag_test (FLAG_TV_LIT) &&
		!flag_test (FLAG_MULTIBALL_LIT) &&
		!flag_test (FLAG_MULTIBALL_RUNNING);
}

void tv_light (void)
{
	flag_on (FLAG_TV_LIT);
}

void tv_collect (void)
{
	tv_award_selected = tv_count;
	bounded_increment (tv_count, 250);
	score (SC_100K);
	tv_award_table[tv_award_selected].award ();
	deff_start (DEFF_TV_AWARD);
}


CALLSET_ENTRY (tv, dev_tv_popper_enter)
{
	if (flag_test (FLAG_TV_LIT))
	{
		flag_off (FLAG_TV_LIT);
		tv_collect ();
	}
}

CALLSET_ENTRY (tv, sw_free_kick_target)
{
	if (!flag_test (FLAG_TV_LIT))
	{
		tv_light ();
	}
}

CALLSET_ENTRY (tv, lamp_update)
{
	lamp_on_if (LM_TV_AWARD, tv_can_be_collected ());
}

CALLSET_ENTRY (tv, start_player)
{
	tv_count = 0;
}

CALLSET_ENTRY (tv, start_ball)
{
	tv_light ();
}

