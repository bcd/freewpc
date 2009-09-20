
#include <freewpc.h>

CALLSET_ENTRY (gangway, start_player)
{
	lamplist_apply (LAMPLIST_GANGWAYS, lamp_off);
	lamp_tristate_flash (lamplist_index (LAMPLIST_GANGWAYS, 0));
}

