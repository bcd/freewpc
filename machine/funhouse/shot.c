
#include <freewpc.h>

CALLSET_ENTRY (tunnel, sw_wind_tunnel_hole)
{
	free_timer_restart (TIM_TUNNEL_ENTERED, TIME_4S);
}

CALLSET_ENTRY (tunnel, sw_trap_door)
{
	free_timer_restart (TIM_TUNNEL_ENTERED, TIME_4S);
}

CALLSET_ENTRY (tunnel, sw_lower_right_hole)
{
	free_timer_restart (TIM_TUNNEL_ENTERED, TIME_3S);
}

CALLSET_ENTRY (tunnel, sw_tunnel_kickout)
{
	if (!free_timer_test (TIM_TUNNEL_ENTERED))
	{
		callset_invoke (tunnel_kickout_shot);
	}
}

