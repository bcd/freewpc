
#include <freewpc.h>


DECLARE_SWITCH_DRIVER (sw_left_ramp_enter)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_LEFT_RAMP_ENTER,
};


DECLARE_SWITCH_DRIVER (sw_left_ramp_exit)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_LEFT_RAMP_MADE,
};


