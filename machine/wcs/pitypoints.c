
#include <freewpc.h>

void pity_rollover (void)
{
	score (SC_2500);
}

void pity_skill_lane (void)
{
	score (SC_2500);
	sound_send (SND_WAIT_BELL);
}

CALLSET_ENTRY (pity, sw_rollover_1)
{
	pity_rollover ();
	sound_send (SND_DING1);
}

CALLSET_ENTRY (pity, sw_rollover_2)
{
	pity_rollover ();
	sound_send (SND_DING2);
}

CALLSET_ENTRY (pity, sw_rollover_3)
{
	pity_rollover ();
	sound_send (SND_DING3);
}

CALLSET_ENTRY (pity, sw_rollover_4)
{
	pity_rollover ();
	sound_send (SND_DING4);
}

CALLSET_ENTRY (pity, sw_skill_shot_front)
{
	pity_skill_lane ();
}

CALLSET_ENTRY (pity, sw_skill_shot_center)
{
	pity_skill_lane ();
}

CALLSET_ENTRY (pity, sw_skill_shot_rear)
{
	pity_skill_lane ();
}
