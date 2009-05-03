
#include <freewpc.h>

CALLSET_ENTRY (wcs_effect, timed_game_tick)
{
	if (timed_game_timer == 10)
	{
		speech_start (SPCH_FEW_SECONDS_REMAIN, SL_3S);
	}
	else if (timed_game_timer <= 5)
	{
		switch (timed_game_timer)
		{
			case 5:
			case 4:
			case 3:
			case 2:
			case 1:
				sample_start (SND_DING, SL_1S);
				break;
			case 0:
				sample_start (SND_BUZZER, SL_2S);
				break;
		}
	}
}

