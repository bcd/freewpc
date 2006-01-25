
#include <freewpc.h>


void sw_hitch_handler (void)
{
	score_add_current_const (0x50000);
}


DECLARE_SWITCH_DRIVER (sw_hitch)
{
	.fn = sw_hitch_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
	.sound = SND_HITCHHIKER_DRIVE_BY
};

