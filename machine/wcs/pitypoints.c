
#include <freewpc.h>


CALLSET_ENTRY (pity, bonus)
{
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_500MS);
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_300MS);
	sound_send (MUS_BONUS_BONG);
	task_sleep (TIME_300MS);
}

CALLSET_ENTRY (pity, any_pf_switch)
{
	score (SC_2500);
}

CALLSET_ENTRY (pity, sw_spinner_slow)
{
	score (SC_10K);
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

CALLSET_ENTRY (pity, sw_shooter)
{
	sound_send (SND_PLUNGE);
}
