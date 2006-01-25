
#include <freewpc.h>


void sw_clock_target_handler (void)
{
	score_add_current_const (0x500);
}


DECLARE_SWITCH_DRIVER (sw_clock_target)
{
	.fn = sw_clock_target_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.sound = SND_CUCKOO,
};

