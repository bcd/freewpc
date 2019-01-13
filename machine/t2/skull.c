
#include <freewpc.h>
#include "skull_drop.h"

CALLSET_ENTRY (drop, skull_drop_down)
{
	sound_send (SND_SPLAT);
	//skull_drop_reset ();
}


CALLSET_ENTRY (drop, start_ball)
{
	//skull_drop_reset ();
}

