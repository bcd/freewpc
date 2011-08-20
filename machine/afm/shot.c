
#include <freewpc.h>

/*
 * Loop detection
 */

CALLSET_ENTRY (shot, sw_left_loop_low)
{
	callset_invoke (left_loop_shot);
}

CALLSET_ENTRY (shot, sw_left_loop_high)
{
}

CALLSET_ENTRY (shot, sw_right_loop_low)
{
	if (!global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER))
	{
		set_valid_playfield ();
		callset_invoke (right_loop_shot);
	}
}

CALLSET_ENTRY (shot, sw_right_loop_high)
{
	if (!global_flag_test (GLOBAL_FLAG_BALL_AT_PLUNGER))
	{
		set_valid_playfield ();
	}
}

/*
 * Ramp detection
 */

CALLSET_ENTRY (shot, sw_left_ramp_exit)
{
	callset_invoke (left_ramp_shot);
}

CALLSET_ENTRY (shot, sw_center_ramp_enter)
{
	free_timer_start (TIM_CENTER_RAMP_ENTERED, TIME_3S);
}

CALLSET_ENTRY (shot, sw_right_ramp_enter)
{
	free_timer_start (TIM_RIGHT_RAMP_ENTERED, TIME_2S);
}

CALLSET_ENTRY (shot, sw_right_ramp_exit)
{
	if (free_timer_test (TIM_CENTER_RAMP_ENTERED))
	{
		callset_invoke (center_ramp_shot);
		free_timer_stop (TIM_CENTER_RAMP_ENTERED);
	}
	if (free_timer_test (TIM_RIGHT_RAMP_ENTERED))
	{
		callset_invoke (right_ramp_shot);
		free_timer_stop (TIM_CENTER_RAMP_ENTERED);
	}
}

/*
 * Center hole / ball lock
 */

CALLSET_ENTRY (shot, sw_center_trough)
{
	free_timer_restart (TIM_CENTER_TROUGH_ENTERED, TIME_3S);
}

CALLSET_ENTRY (shot, dev_left_hole_enter)
{
	if (free_timer_test (TIM_CENTER_TROUGH_ENTERED))
	{
		callset_invoke (center_hole_shot);
	}
	else
	{
		callset_invoke (lock_lane_shot);
	}
}

/*
 * Stroke of Luck front/back
 */

CALLSET_ENTRY (shot, sw_left_jet, sw_bottom_jet, sw_right_jet)
{
	callset_invoke (any_jet);
	free_timer_restart (TIM_BALL_IN_JETS, TIME_4S);
}

CALLSET_ENTRY (shot, sw_left_top_lane, sw_right_top_lane)
{
	free_timer_restart (TIM_BALL_IN_JETS, TIME_4S);
}

CALLSET_ENTRY (shot, dev_right_hole_enter)
{
	if (free_timer_test (TIM_BALL_IN_JETS))
	{
		callset_invoke (right_hole_back_shot);
	}
	else
	{
		callset_invoke (right_hole_front_shot);
	}
}

/*
 * Standup targets
 */

CALLSET_ENTRY (shot, sw_motor_bank_1, sw_motor_bank_2, sw_motor_bank_3)
{
	callset_invoke (any_motor_bank);
}


CALLSET_ENTRY (shot, sw_martian_1, sw_martian_2, sw_martian_3, sw_martian_4)
{
	callset_invoke (any_martian);
}

CALLSET_ENTRY (shot, sw_martian_5, sw_martian_6, sw_martian_7)
{
	callset_invoke (any_martian);
}

