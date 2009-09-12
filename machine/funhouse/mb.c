
#include <freewpc.h>

__local__ U8 mb_level;

__local__ U8 mb_locks_lit;

__local__ U8 mb_balls_locked;

__local__ U8 mb_jackpot_millions;


static __machine__ void _mb_lamp_update (void);
__machine__ void fh_clock_reset (void);

void mb_start (void)
{
	if (flag_test (FLAG_MULTIBALL_LIT))
	{
		flag_off (FLAG_MULTIBALL_LIT);
		flag_on (FLAG_MULTIBALL_RUNNING);
		flag_on (FLAG_JACKPOT_LIT);
	}
}


void mb_stop (void)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		flag_off (FLAG_MULTIBALL_RUNNING);
		fh_clock_reset ();
		mb_locks_lit = mb_balls_locked = 0;
	}
}


void mb_light (void)
{
	flag_on (FLAG_MULTIBALL_LIT);
}

void collect_jackpot (void)
{
	if (flag_test (FLAG_JACKPOT_LIT))
	{
		flag_on (FLAG_JACKPOT_THIS_BALL);
		flag_off (FLAG_JACKPOT_LIT);
		score_multiple (SC_1M, mb_jackpot_millions);
		bounded_increment (mb_jackpot_millions, 10);
		music_effect_start (SND_JACKPOT, SL_3S);
	}
}

void collect_lock (void)
{
	if (mb_balls_locked < mb_locks_lit)
	{
		mb_balls_locked++;
		if (mb_balls_locked == 2)
			mb_light ();
		_mb_lamp_update ();
	}
}


void add_lock (U8 count)
{
	if (mb_locks_lit < 3)
	{
		mb_locks_lit += count;
		flag_off (FLAG_JACKPOT_THIS_BALL);
		_mb_lamp_update ();
	}
}

static void _mb_lamp_update (void)
{
	if (mb_locks_lit == 0)
	{
		lamp_tristate_off (LM_LOCK);
	}
	else if (mb_locks_lit == mb_balls_locked || mb_balls_locked == 2)
	{
		lamp_tristate_on (LM_LOCK);
	}
	else
	{
		lamp_tristate_flash (LM_LOCK);
	}

	lamp_flash_if (LM_MILLION_PLUS, flag_test (FLAG_JACKPOT_LIT));
	lamp_on_if (LM_RAMP_250K, flag_test (FLAG_MULTIBALL_RUNNING));
}


bool lock_lit_p (void)
{
	return mb_locks_lit && (mb_balls_locked < mb_locks_lit)
		&& (mb_balls_locked < 2);
}

bool multiball_lit_p (void)
{
	return (mb_locks_lit == 3 && mb_balls_locked == 2);
}


CALLSET_ENTRY (mb, single_ball_play)
{
	mb_stop ();
}

CALLSET_ENTRY (mb, lamp_update)
{
	_mb_lamp_update ();
}

CALLSET_ENTRY (mb, music_refresh)
{
	if (flag_test (FLAG_JACKPOT_THIS_BALL))
	{
		music_request (MUS_JACKPOT, PRI_MULTIBALL);
	}
	else if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		music_request (MUS_MULTIBALL, PRI_MULTIBALL);
	}
	else if (flag_test (FLAG_MULTIBALL_LIT))
	{
		music_request (MUS_MIDNIGHT, PRI_MULTIBALL);
	}
	else if (mb_balls_locked == 1)
	{
		music_request (MUS_1130, PRI_MULTIBALL);
	}
	else if (mb_balls_locked == 2)
	{
		music_request (MUS_1145, PRI_MULTIBALL);
	}
}


CALLSET_ENTRY (mb, dev_lock_enter)
{
	if (lock_lit_p ())
	{
		collect_lock ();
	}
}

CALLSET_ENTRY (mb, rudy_shot)
{
	if (multiball_lit_p ())
	{
		mb_start ();
	}
	else if (flag_test (FLAG_JACKPOT_LIT))
	{
		score (SC_1M);
		flag_off (FLAG_JACKPOT_LIT);
	}
}


CALLSET_ENTRY (mb, sw_ramp_exit)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		score (SC_250K);
		if (!flag_test (FLAG_JACKPOT_LIT))
		{
			flag_on (FLAG_JACKPOT_LIT);
		}
	}
	else
	{
		add_lock (1);
	}
}


CALLSET_ENTRY (mb, upper_loop_or_trap_door_shot)
{
	collect_jackpot ();
}


CALLSET_ENTRY (mb, start_player)
{
	mb_level = 0;
	mb_locks_lit = 0;
	mb_balls_locked = 0;
	mb_jackpot_millions = 2;
	flag_off (FLAG_MULTIBALL_LIT);
}

CALLSET_ENTRY (mb, start_ball)
{
	flag_off (FLAG_JACKPOT_THIS_BALL);
	mb_stop ();
}

