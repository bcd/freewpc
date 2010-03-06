/* CALLSET_SECTION (buyin, __machine2__) */
#include <freewpc.h>

CALLSET_ENTRY (buyin, sw_buyin_button)
{
	if (in_live_game)
	{
		callset_invoke (door_start_sslot);
	}
}
