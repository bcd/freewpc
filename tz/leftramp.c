
#include <freewpc.h>

__local__ U8 left_ramps;


void left_ramp_deff (void)
{
}


void sw_left_ramp_handler (void)
{
	leff_start (LEFF_LEFT_RAMP);
	left_ramps++;
	score_add_current_const (0x10000);
}


DECLARE_SWITCH_DRIVER (sw_left_ramp_enter)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_LEFT_RAMP_ENTER,
};


DECLARE_SWITCH_DRIVER (sw_left_ramp_exit)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_LEFT_RAMP_MADE,
	.fn = sw_left_ramp_handler,
};


