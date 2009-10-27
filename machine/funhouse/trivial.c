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

/*
 * Trivial shot rules
 */

#define CLK_5_MIN  1
#define CLK_10_MIN 2
#define CLK_15_MIN 3
#define CLK_20_MIN 4
#define CLK_25_MIN 5
#define CLK_30_MIN 6

__local__ U8 rudy_hits;

static inline U8 decimal_to_bcd_byte (U8 decimal)
{
#ifdef __m6809__
	U8 quot, rem;
	DIV10 (decimal, quot, rem);
	return (quot << 4) + rem;
#else
	return ((decimal / 10) << 4) + (decimal % 10);
#endif
}


void replay_code_to_score (score_t s, U8 val)
{
	/* Replay ranges from 1 = 5M to 11 = 15M */
	s[1] = decimal_to_bcd_byte (val + 4);
}


void bonus_deff (void)
{
	U8 hits;

	music_disable ();
	seg_alloc_clean ();
	task_sleep (TIME_100MS);
	seg_write_row_center (0, "BONUS");
	if (rudy_hits != 1)
		sprintf ("%d RUDY HITS", rudy_hits);
	else
		sprintf ("%d RUDY HIT", rudy_hits);
	seg_write_row_center (1, sprintf_buffer);
	seg_sched_transition (&seg_trans_ltr);
	sample_start (SND_BONUS_BLIP1, SL_1S);
	seg_show ();
	task_sleep_sec (1);

	hits = 0;
	while (hits < rudy_hits)
	{
		score (SC_50K);
		sample_start (SND_BONUS_BLIP3, SL_500MS);
		seg_alloc_clean ();
		scores_draw ();
		seg_show ();
		if (hits <= 9)
			task_sleep (TIME_500MS - hits * TIME_33MS);
		else
			task_sleep (TIME_200MS);
		hits++;
	}

	task_sleep_sec (1);
	music_enable ();
	deff_exit ();
}

void million_lamp_update (void)
{
	if (flag_test (FLAG_MILLION_LIT) ||
		flag_test (FLAG_MULTIBALL_LIT) ||
		flag_test (FLAG_QUICK_MB_RUNNING))
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


CALLSET_ENTRY (trivial, valid_playfield)
{
	extern U8 mb_level;
	if (mb_level == 0)
		fh_clock_advance (CLK_30_MIN);
}

CALLSET_ENTRY (trivial, sw_ramp_enter)
{
	score (SC_2570);
}

CALLSET_ENTRY (trivial, sw_ramp_exit)
{
	if (!multiball_mode_running_p ())
	{
		sample_start (SND_RAMP_MADE, SL_2S);
		score (SC_100K);
		fh_clock_advance (CLK_15_MIN);
	}
}

CALLSET_ENTRY (trivial, sw_left_slingshot, sw_right_slingshot)
{
	rudy_look_straight ();
	score (SC_110);
	sample_start (SND_SLING, SL_100MS * 3);
	if (flag_test (FLAG_OUTLANES_LIT))
		lamp_toggle (LM_SPECIALS);
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

CALLSET_ENTRY (trivial, sw_left_inlane, sw_inner_right_inlane)
{
	fh_clock_advance (CLK_5_MIN);
	score (SC_10K);
	rudy_look_straight ();
	sample_start (SND_INLANE, SL_500MS);
}

CALLSET_ENTRY (trivial, sw_outer_right_inlane)
{
	fh_clock_advance (CLK_5_MIN);
	score (SC_10K);
	rudy_look_straight ();
	sample_start (SND_OUTER_RIGHT_INLANE, SL_500MS);
}

void common_outlane (void)
{
	score (SC_50K);
	rudy_look_straight ();
	if (!flag_test (FLAG_STEPS_OPEN))
		sample_start (SND_OUTLANE, SL_500MS);
	if (lamp_test (LM_SPECIALS))
	{
		lamp_off (LM_SPECIALS);
		flag_off (FLAG_OUTLANES_LIT);
		special_award ();
	}
}

CALLSET_ENTRY (trivial, sw_left_outlane)
{
	if (!flag_test (FLAG_BALL_AT_STEPS))
		common_outlane ();
}

CALLSET_ENTRY (trivial, sw_right_outlane)
{
	common_outlane ();
}

CALLSET_ENTRY (trivial, sw_wind_tunnel_hole)
{
	fh_clock_advance (CLK_15_MIN);
	score (SC_50K);
}

CALLSET_ENTRY (trivial, sw_lower_right_hole)
{
	fh_clock_advance (CLK_30_MIN);
	score (SC_50K);
}

CALLSET_ENTRY (trivial, sw_dummy_jaw)
{
	score (SC_50K);
}

CALLSET_ENTRY (trivial, sw_step_s, sw_step_t, sw_step_e, sw_step_p)
{
	score (SC_5130);
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
	score (SC_75K);
	/* score skill shot */
	rudy_look_left ();
}

CALLSET_ENTRY (trivial, dev_trough_kick_success)
{
	rudy_look_right ();
}

CALLSET_ENTRY (trivial, dev_lock_enter)
{
	score (SC_25K);
	fh_clock_advance (CLK_30_MIN);
	rudy_look_left ();
	collect_extra_ball ();
}

CALLSET_ENTRY (trivial, rudy_shot)
{
	score (SC_50K);
	fh_clock_advance (CLK_15_MIN);
	if (rudy_hits < 99)
		rudy_hits++;
}

CALLSET_ENTRY (trivial, sw_jet_lane)
{
	score (SC_10K);
	rudy_look_right ();
}

CALLSET_ENTRY (trivial, rudy_jaw)
{
	audit_increment (&feature_audits.rudy_hits);
	speech_start (SPCH_OWW, SL_2S); /* happening on gulp too */
}

CALLSET_ENTRY (trivial, dev_rudy_enter)
{
	score (SC_250K);
}

CALLSET_ENTRY (trivial, tunnel_kickout_shot)
{
	score (SC_25K);
	fh_clock_advance (CLK_15_MIN);
	rudy_look_right ();
}

CALLSET_ENTRY (trivial, left_loop_shot)
{
	fh_clock_advance (CLK_10_MIN);
	rudy_look_left ();
	sample_start (SND_LOOP, SL_2S);
}

CALLSET_ENTRY (trivial, right_loop_shot)
{
	fh_clock_advance (CLK_10_MIN);
	rudy_look_right ();
	sample_start (SND_LOOP, SL_2S);
}

CALLSET_ENTRY (trivial, sw_right_plunger)
{
	if (!switch_poll_logical (SW_RIGHT_PLUNGER))
	{
		sample_start (SND_PLUNGE, SL_2S);
		leff_start (LEFF_SHOOTER);
	}
	callset_invoke (sw_shooter);
}

CALLSET_ENTRY (trivial, end_ball, start_ball)
{
	rudy_look_straight ();
}

CALLSET_ENTRY (trivial, start_player)
{
	rudy_hits = 1;
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

