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
 * Trap door and loop shots.
 * In some cases, we treat these the same.
 */

CALLSET_ENTRY (trap_door_or_loop, sw_trap_door, sw_upper_loop)
{
	callset_invoke (upper_loop_or_trap_door_shot);
}


CALLSET_ENTRY (hideout, dev_hideout_kick_attempt)
{
	sample_start (SND_KICKOUT, SL_1S);
}

/*
 * Tunnel system shots.
 * The tunnel can be entered from four different shots.
 */

static void tunnel_entered (void)
{
	/* From any entrance except the kickout itself, disable scoring on the
	 * kickout switch temporarily. */
	free_timer_restart (TIM_TUNNEL_ENTERED, TIME_4S);

	/* TODO : Start a monitor task to see that the ball eventually reaches the
	 * kickout.  Fire the kicker in case the switch is bad... */
}

CALLSET_ENTRY (tunnel, sw_wind_tunnel_hole)
{
	tunnel_entered ();
	sample_start (SND_WIND, SL_2S);
}

CALLSET_ENTRY (tunnel, sw_trap_door, sw_lower_right_hole)
{
	tunnel_entered ();
}


CALLSET_ENTRY (tunnel, sw_tunnel_kickout)
{
	/* Stop the kickout monitor */
	if (!free_timer_test (TIM_TUNNEL_ENTERED))
	{
		callset_invoke (tunnel_kickout_shot);
	}
}

CALLSET_ENTRY (tunnel, dev_tunnel_kick_attempt)
{
	leff_start (LEFF_CLOCK_VIBRATE);
	sample_start (SND_KICKOUT_COMING, SL_1S);
	task_sleep_sec (1);
	sample_start (SND_KICKOUT, SL_1S);
}


/*
 * Rudy hit shots.
 * When the mouth is closed, a hit to the jaw should be debounced longer
 * to avoid spurious opto closures.
 * When the mouth is open and the ball is spit out again, the jaw opto
 * should be ignored altogether.
 */

CALLSET_ENTRY (rudy_hit, sw_dummy_jaw)
{
	if (free_timer_test (TIM_IGNORE_JAW))
		return;
	callset_invoke (rudy_shot);
	callset_invoke (rudy_jaw);
	free_timer_restart (TIM_IGNORE_JAW, TIME_1S);
}

CALLSET_ENTRY (rudy_hit, dev_rudy_kick_attempt)
{
	free_timer_restart (TIM_IGNORE_JAW, TIME_2S);
	speech_start (SPCH_BLEH, SL_2S);
}


/*
 * Ramp gate/made
 */
CALLSET_ENTRY (ramp, sw_ramp_enter)
{
	if (free_timer_test (TIM_RAMP_JUST_ENTERED))
	{
		sample_start (SND_RAMP_ENTER2, SL_1S);
		free_timer_stop (TIM_RAMP_JUST_ENTERED);
	}
	else
	{
		sample_start (SND_RAMP_ENTER, SL_1S);
		free_timer_start (TIM_RAMP_JUST_ENTERED, TIME_3S);
		callset_invoke (ramp_entered);
	}
}

CALLSET_ENTRY (ramp, sw_ramp_exit)
{
	free_timer_stop (TIM_RAMP_JUST_ENTERED);
}



