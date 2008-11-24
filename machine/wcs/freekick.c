
#include <freewpc.h>

__local__ U8 free_kicks;

U8 free_kick_multiplier;


void freekick_lamp_update (void)
{
	if (free_kick_multiplier > 1)
	{
		lamp_tristate_flash (LM_FREE_KICK);
	}
	else if (free_kick_multiplier == 1)
	{
		lamp_tristate_on (LM_FREE_KICK);
	}
	else
	{
		lamp_tristate_off (LM_FREE_KICK);
	}
}

void free_kick_reset_multiplier (void)
{
	free_kick_multiplier = 1;
	freekick_lamp_update ();
}

void free_kick_timeout_multiplier (void)
{
	task_sleep_sec (5);
	free_kick_reset_multiplier ();
	task_exit ();
}


void free_kick_award (void)
{
	free_kicks += free_kick_multiplier;
	if (free_kick_multiplier < 5)
	{
		free_kick_multiplier++;
		freekick_lamp_update ();
	}
	task_recreate_gid (GID_FREEKICK_TIMEOUT, free_kick_timeout_multiplier);
}


CALLSET_ENTRY (freekick, sw_free_kick_target)
{
	if (free_kick_multiplier)
		free_kick_award ();
}

CALLSET_ENTRY (freekick, start_player)
{
	free_kicks = 0;
	free_kick_reset_multiplier ();
}
