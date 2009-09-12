
#include <freewpc.h>
#include <eb.h>

__local__ U8 mirror_award;

__local__ U8 mirror_award_count;

U8 mirror_being_awarded;

/* TODO - move to include/lamptimer.h */
__common__ void lamp_timer_stop (lampnum_t lamp);
__common__ task_pid_t lamp_timer_find (lampnum_t lamp);
__common__ void lamp_timer_start (lampnum_t lamp, U8 secs);


void mirror_exec_eb (void)
{
	light_easy_extra_ball ();
}

void mirror_exec_million (void)
{
	flag_on (FLAG_MILLION_LIT);
}

void mirror_exec_bumpers (void)
{
}

void mirror_exec_superdog (void)
{
}

void mirror_exec_gate (void)
{
}

void mirror_exec_quickmb (void)
{
}

void (*mirror_exec_table[]) (void) = {
	mirror_exec_eb,
	mirror_exec_million,
	mirror_exec_bumpers,
	mirror_exec_superdog,
	mirror_exec_gate,
	mirror_exec_quickmb,
};

struct {
	const char *line1;
	const char *line2;
} mirror_text_table[] = {
	{ "EXTRA BALL", "IS LIT" },
	{ "MILLION", "IS LIT" },
	{ "JET BUMPERS", "AT MAX" },
	{ "SUPER DOG", "IS LIT" },
	{ "STEPS GATE", "IS OPEN" },
	{ "QUICK", "MULTIBALL" },
};

void mirror_collect_deff (void)
{
	generic_deff (mirror_text_table[mirror_being_awarded].line1,
		mirror_text_table[mirror_being_awarded].line2);
}

void mirror_update (void)
{
	lamp_tristate_flash (lamplist_index (LAMPLIST_MIRROR_AWARDS, mirror_award));
}

void mirror_move (void)
{
	if (lamplist_test_all (LAMPLIST_MIRROR_AWARDS, lamp_test))
		return;
	lamp_flash_off (lamplist_index (LAMPLIST_MIRROR_AWARDS, mirror_award));
	do {
		mirror_award++;
		if (mirror_award >= 6)
			mirror_award = 0;
		if (!lamp_test (lamplist_index (LAMPLIST_MIRROR_AWARDS, mirror_award)))
			break;
	} while (0);
	mirror_update ();
}

static inline bool mirror_qualified_p (void)
{
	return flag_test (FLAG_MIRROR_LIT);
}

static inline bool mirror_masked_p (void)
{
	return flag_test (FLAG_MULTIBALL_RUNNING);
}

static bool mirror_lit_p (void)
{
	return mirror_qualified_p () && !mirror_masked_p ();
}


void mirror_collect (void)
{
	mirror_being_awarded = mirror_award;
	lamp_tristate_on (lamplist_index (LAMPLIST_MIRROR_AWARDS, mirror_award));
	(*mirror_exec_table[mirror_award]) ();
	mirror_move ();
	deff_start (DEFF_MIRROR_COLLECT);
	sample_start (SND_WIND_TUNNEL, SL_3S);
	score (SC_100K);
}


CALLSET_ENTRY (mirror, sw_wind_tunnel_hole)
{
	if (mirror_lit_p ())
	{
		flag_off (FLAG_MIRROR_LIT);
		mirror_collect ();
	}
	else
	{
		leff_start (LEFF_CIRCLE_OUT);
	}
}

CALLSET_ENTRY (mirror, rudy_shot)
{
	flag_on (FLAG_MIRROR_LIT);

	if (flag_test (FLAG_MILLION_LIT))
	{
		score (SC_1M);
		flag_off (FLAG_MILLION_LIT);
	}
}

CALLSET_ENTRY (mirror, sw_outer_right_inlane)
{
	if (!lamp_test (LM_MIRROR_VALUE))
	{
		//lamp_timer_start (LM_MIRROR_VALUE, 5);
	}
}

CALLSET_ENTRY (mirror, any_jet)
{
	mirror_move ();
}

CALLSET_ENTRY (mirror, lamp_update)
{
	/* if (!lamp_timer_find (LM_MIRROR_VALUE)) */
	lamp_on_if (LM_MIRROR_VALUE, mirror_lit_p ());
}


CALLSET_ENTRY (mirror, start_player)
{
	lamplist_apply (LAMPLIST_MIRROR_AWARDS, lamp_off);
	mirror_award = 2;
	flag_on (FLAG_MIRROR_LIT);
	mirror_update ();
}

CALLSET_ENTRY (mirror, start_ball)
{
	flag_on (FLAG_MIRROR_LIT);
}

