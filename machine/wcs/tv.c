
#include <freewpc.h>

__local__ U8 tv_count;

struct tv_award
{
	U8 static_prob;
	void (*dynamic_prob) (void);
	void (*award) (void);
};

void tv_award_points (void)
{
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
}

void tv_award_light_extra_ball (void)
{
}

void tv_award_add_time (void)
{
}


void tv_award_deff (void)
{
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
	bounded_increment (tv_count, 250);
	score (SC_250K);
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

