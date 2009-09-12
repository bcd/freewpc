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

/*
 * Trivial shot rules
 */

#define CLK_5_MIN  1
#define CLK_10_MIN 2
#define CLK_15_MIN 3
#define CLK_20_MIN 4
#define CLK_25_MIN 5
#define CLK_30_MIN 6

__machine__ void fh_clock_advance ();
__machine__ void rudy_look_left (void);
__machine__ void rudy_look_straight (void);
__machine__ void rudy_look_right (void);
__machine__ void rudy_blink (void);
__common__ void collect_extra_ball (void);

__local__ U8 rudy_hits;

void bonus_deff (void)
{
	music_off ();
	seg_alloc ();
	seg_erase ();
	task_sleep (TIME_100MS);
	seg_write_row_center (0, "BONUS");
	seg_show ();
	sample_start (SND_BONUS_BLIP1, SL_1S);
	task_sleep_sec (2);
	deff_exit ();
}

void million_lamp_update (void)
{
	if (flag_test (FLAG_MILLION_LIT) || flag_test (FLAG_MULTIBALL_LIT))
	{
		lamp_tristate_flash (LM_MILLION);
	}
	else
	{
		lamp_tristate_off (LM_MILLION);
	}
}


CALLSET_ENTRY (trivial, lamp_update)
{
	million_lamp_update ();
}

CALLSET_ENTRY (trivial, sw_ramp_exit)
{
	sample_start (SND_RAMP_MADE, SL_2S);
	score (SC_100K);
}

CALLSET_ENTRY (trivial, sw_left_slingshot, sw_right_slingshot)
{
	score (SC_110);
	rudy_look_straight ();
}

CALLSET_ENTRY (trivial, any_jet)
{
	score (SC_1K);
	fh_clock_advance (CLK_5_MIN);
	rudy_look_right ();
}

CALLSET_ENTRY (trivial, sw_upper_left_jet)
{
	sample_start (SND_JET (0), SL_500MS);
	callset_invoke (any_jet);
}

CALLSET_ENTRY (trivial, sw_upper_right_jet)
{
	sample_start (SND_JET (1), SL_500MS);
	callset_invoke (any_jet);
}

CALLSET_ENTRY (trivial, sw_lower_jet)
{
	sample_start (SND_JET (2), SL_500MS);
	callset_invoke (any_jet);
}

CALLSET_ENTRY (trivial, sw_left_inlane, sw_inner_right_inlane, sw_outer_right_inlane)
{
	score (SC_10K);
	rudy_look_straight ();
}

CALLSET_ENTRY (trivial, sw_left_outlane, sw_right_outlane)
{
	score (SC_50K);
	callset_invoke (any_outlane);
	rudy_look_straight ();
}

CALLSET_ENTRY (trivial, sw_wind_tunnel_hole)
{
	score (SC_50K);
}

CALLSET_ENTRY (trivial, sw_dummy_jaw)
{
	score (SC_50K);
}

CALLSET_ENTRY (trivial, sw_step_s, sw_step_t, sw_step_e, sw_step_p)
{
	score (SC_10K);
	fh_clock_advance (CLK_5_MIN);
}

CALLSET_ENTRY (trivial, sw_superdog_low, sw_superdog_center, sw_superdog_high)
{
	score (SC_25K);
	fh_clock_advance (CLK_15_MIN);
	rudy_look_straight ();
}

CALLSET_ENTRY (trivial, sw_upper_loop)
{
	score (SC_100K);
}

CALLSET_ENTRY (trivial, dev_hideout_enter)
{
	score (SC_50K);
	rudy_look_left ();
}

CALLSET_ENTRY (trivial, dev_lock_enter)
{
	score (SC_25K);
	rudy_look_left ();
	collect_extra_ball ();
}

CALLSET_ENTRY (trivial, rudy_shot)
{
	score (SC_50K);
	fh_clock_advance (CLK_15_MIN);
	rudy_blink ();
}

CALLSET_ENTRY (trivial, sw_jet_lane)
{
	score (SC_10K);
	rudy_look_right ();
}

CALLSET_ENTRY (trivial, rudy_jaw)
{
	fh_clock_advance (CLK_15_MIN);
	speech_start (SPCH_OWW, SL_2S);
}

CALLSET_ENTRY (trivial, dev_rudy_enter)
{
	fh_clock_advance (CLK_15_MIN);
}

CALLSET_ENTRY (trivial, tunnel_kickout_shot)
{
	score (SC_25K);
	fh_clock_advance (CLK_15_MIN);
	rudy_look_right ();
}

CALLSET_ENTRY (trivial, sw_left_gangway)
{
	fh_clock_advance (CLK_10_MIN);
	rudy_look_left ();
}

CALLSET_ENTRY (trivial, sw_right_gangway)
{
	fh_clock_advance (CLK_10_MIN);
	rudy_look_right ();
}

CALLSET_ENTRY (trivial, end_ball, start_ball)
{
	rudy_look_straight ();
}

CALLSET_ENTRY (trivial, start_player)
{
	rudy_hits = 0;
}

CALLSET_ENTRY (trivial, start_ball)
{
	flag_off (FLAG_MILLION_LIT);
}

CALLSET_ENTRY (trivial, bonus)
{
	deff_start (DEFF_BONUS);
	task_sleep (TIME_1S);
	while (deff_get_active () == DEFF_BONUS)
		task_sleep (TIME_166MS);
}


