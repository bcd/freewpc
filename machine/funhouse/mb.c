/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>

__local__ U8 mb_level;

__local__ U8 mb_locks_lit;

__local__ U8 mb_balls_locked;

__local__ U8 mb_jackpot_millions;


void mb_1130_deff (void)
{
	U8 n;
	for (n=0; n < 3; n++)
	{
		seg_alloc_clean ();
		seg_show ();
		task_sleep (TIME_200MS);
		seg_alloc_clean ();
		seg_write_row_center (0, "11:30");
		seg_write_row_center (1, "SHOOT LOCK");
		seg_show ();
		sample_start (SND_GONG, SL_1S);
		task_sleep (TIME_800MS);
	}
	task_sleep (TIME_500MS);
	deff_exit ();
}

void mb_1145_deff (void)
{
	deff_exit ();
}

void mb_midnight_deff (void)
{
	deff_exit ();
}

void mb_start_deff (void)
{
	deff_exit ();
}

void mb_jackpot_deff (void)
{
	deff_exit ();
}


static __machine__ void _mb_lamp_update (void);
__machine__ void fh_clock_reset (void);
__machine__ void fh_clock_advance_to_1145 (void);
__machine__ void fh_clock_advance_to_1200 (void);


bool multiball_mode_running_p (void)
{
	return flag_test (FLAG_MULTIBALL_RUNNING) ||
		flag_test (FLAG_QUICK_MB_RUNNING);
}

void trap_door_update (void)
{
	if (in_live_game)
	{
		if (flag_test (FLAG_JACKPOT_LIT) ||
			(flag_test (FLAG_FRENZY_LIT) && !multiball_mode_running_p ()))
		{
			sol_request (SOL_TRAP_DOOR_OPEN);
		}
		else
		{
			sol_request (SOL_TRAP_DOOR_CLOSE);
		}
	}
}

/* update trapdoor after ball search */

CALLSET_ENTRY (trapdoor, sw_trap_door, sw_upper_loop)
{
	trap_door_update ();
}


void light_jackpot (void)
{
	if (!flag_test (FLAG_JACKPOT_LIT))
	{
		flag_on (FLAG_JACKPOT_LIT);
		trap_door_update ();
	}
}


void mb_start (void)
{
	if (flag_test (FLAG_MULTIBALL_LIT))
	{
		flag_off (FLAG_MULTIBALL_LIT);
		flag_on (FLAG_MULTIBALL_RUNNING);
		light_jackpot ();
		device_request_empty (device_entry (DEVNO_LOCK));
		music_timed_disable (TIME_1S);
	}
}


void mb_stop (void)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
		fh_clock_reset ();
		mb_locks_lit = mb_balls_locked = 0;
	}
	flag_off (FLAG_MULTIBALL_RUNNING);
	flag_off (FLAG_JACKPOT_LIT);
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
		trap_door_update ();
	}
}

void collect_lock (void)
{
	if (mb_balls_locked < mb_locks_lit)
	{
		mb_balls_locked++;
		device_lock_ball (device_entry (DEVNO_LOCK));
		if (mb_balls_locked == 1)
		{
			fh_clock_advance_to_1145 ();
		}
		else if (mb_balls_locked == 2)
		{
			fh_clock_advance_to_1200 ();
			mb_light ();
		}
		_mb_lamp_update ();
	}
}


void light_lock (void)
{
	mb_locks_lit = 2;
	flag_off (FLAG_JACKPOT_THIS_BALL);
	_mb_lamp_update ();
	deff_start (DEFF_MB_1130);
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
	return flag_test (FLAG_MULTIBALL_LIT);
}


CALLSET_ENTRY (mb, end_ball, single_ball_play)
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
	else if (mb_locks_lit)
	{
		if (!valid_playfield)
			music_request (MUS_1130_PLUNGER, PRI_MULTIBALL);
		else if (mb_balls_locked == 0)
			music_request (MUS_1130, PRI_MULTIBALL);
		else
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

CALLSET_ENTRY (mb, dev_lock_kick_attempt)
{
	if (flag_test (FLAG_JACKPOT_LIT))
	{
		sample_start (SND_LOCK_MB_KICK, SL_1S);
	}
	else
	{
		sample_start (SND_BOOM2, SL_2S);
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
	if (multiball_mode_running_p ())
	{
		score (SC_250K);
		light_jackpot ();
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
	/* TODO - update mb_balls_locked according to physical state */
}

