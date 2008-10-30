
#include <freewpc.h>

CALLSET_ENTRY (pity, any_pf_switch)
{
	score (SC_2500);
}

void pity_skill_lane (void)
{
	sound_send (SND_WAIT_BELL);
}

CALLSET_ENTRY (pity, sw_rollover_1)
{
	sound_send (SND_DING1);
}

CALLSET_ENTRY (pity, sw_rollover_2)
{
	sound_send (SND_DING2);
}

CALLSET_ENTRY (pity, sw_rollover_3)
{
	sound_send (SND_DING3);
}

CALLSET_ENTRY (pity, sw_rollover_4)
{
	sound_send (SND_DING4);
}

