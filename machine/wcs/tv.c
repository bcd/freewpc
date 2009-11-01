
#include <freewpc.h>
#include <eb.h>

__local__ U8 tv_count;

U8 tv_award_selected;

struct tv_award
{
	const char *name;
	void (*award) (void);
	U8 static_prob;
	U8 (*dynamic_prob) (struct tv_award *);
};

void tv_award_points (void)
{
	score (SC_250K);
}

void tv_award_start_mode (void)
{
	VOIDCALL (mode_start);
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
	light_easy_extra_ball ();
}

void tv_award_add_time (void)
{
	timed_game_extend (30);
}

#define TV_AWARD_COUNT (sizeof (tv_award_table) / sizeof (struct tv_award))

struct tv_award tv_award_table[] = {
	{ "QUICK MULTIBALL", tv_award_start_quickmb, 5, NULL },
	{ "HURRY UP", tv_award_hurryup, 40, NULL },
	/* next one should not apply unless in timed game */
	{ "ADD 30 SECONDS", tv_award_add_time, 20, NULL },
#if 0
	{ "20 SEC. BALLSAVE", tv_award_ball_save, 20, NULL },
	{ "LIGHT MULTIBALL", tv_award_light_mb, 20, NULL },
	{ "MULTIBALL", tv_award_start_mb, 20, NULL },
	{ "ADVANCE CITY", tv_award_advance_city, 20, NULL },
#endif
	{ "START MODE", tv_award_start_mode, 60, NULL },
	{ "LITE ULTRA SHOT", tv_award_add_ultra, 60, NULL },
	/* next one should not apply in timed game */
	{ "LIGHT EXTRA BALL", tv_award_light_extra_ball, 5, NULL },
	{ "250,000", tv_award_points, 100, NULL },
};


void tv_award_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("TV AWARD %d", tv_count);
	font_render_string_center (&font_fixed6, 64, 9, sprintf_buffer);
	sprintf ("%s", tv_award_table[tv_award_selected].name);
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);
	dmd_draw_border (dmd_low_buffer);
	dmd_show_low ();
	sample_start (SND_TV_STATIC, SL_3S);
	task_sleep_sec (3);
	deff_exit ();
}

void tv_lit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "TV IS LIT");
	dmd_show_low ();
	task_sleep (TIME_1500MS);
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
#if 1
	/* Until the logic below is implemented, just go through them in order. */
	tv_award_selected = tv_count;
	while (tv_award_selected > TV_AWARD_COUNT)
		tv_award_selected -= TV_AWARD_COUNT;
#else
	/* The TV Awards are pseudorandom, like AFM's Stroke of Luck.
	The algorithm works as follows:
		Scan the table of TV awards in order from top to bottom.
	The awards should be ordered so that the higher priority awards
	are at the top.
		If the current award can be given out, then stop and do so.
	Else move to the next award, and so on.
		To decide whether or not the current award should be selected,
	two things happen.  First, there is a static priority, in the range of
	0 to 100.  This determines the likelihood of giving out that award,
	when it is truly random.  Before doing this, though, if a
	dynamic_prob function is defined, then call that.  It returns a new
	probability.  It can use any criteria for increasing/decreasing the
	probability.  0 and 100 are allowable results, meaning that the
	award is definitely/never given out.  If not needed, this function
	should be NULL.
		The last entry in the table must always succeed, as a last resort. */
#endif
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
		deff_start (DEFF_TV_LIT);
	}
}

CALLSET_ENTRY (tv, lamp_update)
{
	lamp_on_if (LM_TV_AWARD, tv_can_be_collected ());
}

CALLSET_ENTRY (tv, start_player)
{
	tv_count = 0;
	tv_light ();
}

