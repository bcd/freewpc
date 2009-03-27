/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/**
 * Throw events for various shots that are determined
 * by more than just a single switch.
 */

/*
 * Throw goal_shot and striker_shot.
 *
 * The goal trough switch throws goal and disables
 * striker_shot until the ball is released.
 */

CALLSET_ENTRY (shot, sw_goal_trough)
{
	callset_invoke (goal_shot);
	free_timer_restart (TIM_GOAL_TROUGH_MADE, TIME_4S);
}

CALLSET_ENTRY (shot, dev_goal_popper_enter)
{
	if (!free_timer_test (TIM_GOAL_TROUGH_MADE))
	{
		callset_invoke (striker_shot);
	}
	else
	{
		free_timer_stop (TIM_GOAL_TROUGH_MADE);
	}
}

CALLSET_ENTRY (shot, dev_goal_popper_kick_attempt)
{
	free_timer_restart (TIM_GOAL_TROUGH_MADE, TIME_4S);
}

/*
 * Throw ramp events.
 *
 * A diverted left ramp should ignore the right ramp exit.
 */

void ramp_enter (void)
{
	sound_start (ST_SAMPLE, SND_ASTHMA_ATTACK, SL_2S, PRI_GAME_QUICK1);
}

CALLSET_ENTRY (shot, sw_l_ramp_enter)
{
	ramp_enter ();
	/* TODO - start a timer for awarding the
	shot on a jet bumper hit */
}

CALLSET_ENTRY (shot, any_jet)
{
}

CALLSET_ENTRY (shot, sw_r_ramp_enter)
{
	ramp_enter ();
}

CALLSET_ENTRY (shot, sw_l_ramp_diverted)
{
	/* TODO - be careful, cannot ask for more than 4s of this API */
	free_timer_restart (TIM_IGNORE_R_RAMP_EXIT, TIME_4S);
	sound_start (ST_SAMPLE, MUS_TICKET_BOUGHT, SL_1S, PRI_GAME_QUICK2);
	callset_invoke (left_ramp_shot);
}

CALLSET_ENTRY (shot, sw_l_ramp_exit)
{
	sound_start (ST_SAMPLE, MUS_TICKET_BOUGHT, SL_1S, PRI_GAME_QUICK2);
	callset_invoke (left_ramp_shot);
}

CALLSET_ENTRY (shot, sw_r_ramp_exit)
{
	if (!free_timer_test (TIM_IGNORE_R_RAMP_EXIT))
	{
		sound_start (ST_SAMPLE, MUS_TICKET_BOUGHT, SL_1S, PRI_GAME_QUICK2);
		callset_invoke (right_ramp_shot);
	}
	else
	{
		free_timer_stop (TIM_IGNORE_R_RAMP_EXIT);
	}
}

/*
 * Throw kickback events.  Kickback upper throws
 * two different sub-events depending on direction:
 * 'enter' by default; 'exit' right after the lower
 * kickback.
 *
 * TODO - this does not work completely in multiball.
 */

CALLSET_ENTRY (shot, sw_kickback_upper)
{
	if (free_timer_test (TIM_KICKBACK_MADE))
	{
		free_timer_stop (TIM_KICKBACK_MADE);
		callset_invoke (sw_kickback_upper_exit);
	}
	else
	{
		callset_invoke (sw_kickback_upper_enter);
	}
}

CALLSET_ENTRY (shot, sw_kickback)
{
	free_timer_restart (TIM_KICKBACK_MADE, TIME_2S);
}


/*
 * Start sound effects for the simple switches
 */

CALLSET_ENTRY (shot, sw_left_slingshot, sw_right_slingshot)
{
	sound_start (ST_SAMPLE, SND_SLINGSHOT, SL_500MS, PRI_GAME_QUICK2);
}

CALLSET_ENTRY (shot, sw_left_flipper_lane, sw_right_flipper_lane)
{
	sound_start (ST_SAMPLE, SND_INLANE, SL_500MS, PRI_GAME_QUICK2);
}

/*
 * Throw the left_loop event.  It is not counted
 * if certain switch closures occurred just prior to
 * the travel lane switch: bounceback off loop gate
 * back onto switch, etc.
 */

CALLSET_ENTRY (shot, sw_travel_lane)
{
	callset_invoke (left_loop_shot);
}

