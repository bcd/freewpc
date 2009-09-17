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
#include <eb.h>
#include <steps_gate.h>

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
	SECTION_VOIDCALL (__machine__, jets_max);
}

void mirror_exec_superdog (void)
{
	SECTION_VOIDCALL (__machine__, superdog_start);
}

void mirror_exec_gate (void)
{
	steps_gate_start ();
}

void mirror_exec_quickmb (void)
{
	SECTION_VOIDCALL (__machine__, quickmb_start);
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
	if (lamplist_test_all (LAMPLIST_MIRROR_AWARDS, lamp_flash_test))
		return;
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
	return flag_test (FLAG_MIRROR_LIT) && !flag_test (FLAG_MIRROR_COMPLETE);
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
	if (flag_test (FLAG_SUPER_FRENZY_LIT))
	{
		flag_off (FLAG_SUPER_FRENZY_LIT);
		flag_on (FLAG_MIRROR_COMPLETE);
		lamplist_apply (LAMPLIST_MIRROR_AWARDS, lamp_flash_off);
	}
	else
	{
		mirror_being_awarded = mirror_award;
		lamp_tristate_on (lamplist_index (LAMPLIST_MIRROR_AWARDS, mirror_award));
		(*mirror_exec_table[mirror_award]) ();
		mirror_move ();
		deff_start (DEFF_MIRROR_COLLECT);
		score (SC_100K);

		if (mirror_being_awarded == 5)
			sample_start (SND_QUICK_MB_AWARD, SL_4S);
		else
			sample_start (SND_WIND_TUNNEL, SL_3S);

		if (lamplist_test_all (LAMPLIST_MIRROR_AWARDS, lamp_test))
		{
			lamplist_apply (LAMPLIST_MIRROR_AWARDS, lamp_flash_on);
			lamplist_apply (LAMPLIST_MIRROR_AWARDS, lamp_off);
			flag_on (FLAG_SUPER_FRENZY_LIT);
		}
	}
}


CALLSET_ENTRY (mirror, sw_wind_tunnel_hole)
{
	if (mirror_lit_p ())
	{
		flag_off (FLAG_MIRROR_LIT);
		mirror_collect ();
	}
	else if (single_ball_play ())
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
	lamp_tristate_off (lamplist_index (LAMPLIST_MIRROR_AWARDS, mirror_award));
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
	flag_off (FLAG_SUPER_FRENZY_LIT);
	mirror_update ();
}

CALLSET_ENTRY (mirror, start_ball)
{
	flag_on (FLAG_MIRROR_LIT);
	flag_off (FLAG_MIRROR_COMPLETE);
}

