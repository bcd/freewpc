
#include <freewpc.h>


void sw_hitch_handler (void)
{
}


DECLARE_SWITCH_DRIVER (sw_hitch)
{
	.fn = sw_hitch_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

