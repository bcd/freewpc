
#include <freewpc.h>

/*
 * Trap door and loop shots.
 * In some cases, we treat these the same.
 */

CALLSET_ENTRY (trap_door_or_loop, sw_trap_door, sw_upper_loop)
{
	callset_invoke (upper_loop_or_trap_door_shot);
}


/*
 * Tunnel system shots.
 * The tunnel can be entered from four different shots.
 */

CALLSET_ENTRY (tunnel, sw_wind_tunnel_hole, sw_trap_door, sw_lower_right_hole)
{
	/* From any entrance except the kickout itself, disable scoring on the
	 * kickout switch temporarily. */
	free_timer_restart (TIM_TUNNEL_ENTERED, TIME_4S);

	/* Start a monitor task to see that the ball eventually reaches the
	 * kickout. */
}


CALLSET_ENTRY (tunnel, sw_tunnel_kickout)
{
	/* Stop the kickout monitor */
	if (!free_timer_test (TIM_TUNNEL_ENTERED))
	{
		callset_invoke (tunnel_kickout_shot);
	}
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
	callset_invoke (jaw_shot);
	free_timer_restart (TIM_IGNORE_JAW, TIME_1S);
}

CALLSET_ENTRY (rudy_hit, dev_rudy_kick_attempt)
{
	free_timer_restart (TIM_IGNORE_JAW, TIME_2S);
}

