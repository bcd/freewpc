
#include <freewpc.h>

void toplane_complete (void)
{
	VOIDCALL (city_value_increase);
	task_sleep (TIME_200MS);
	sound_start (ST_SAMPLE, MUS_TICKET_BOUGHT, SL_1S, PRI_GAME_QUICK7);
	lamplist_apply (LAMPLIST_TOP_LANES, lamp_off);
	task_exit ();
}

void toplane_award (U8 lamp)
{
	if (lamp_test (lamp))
	{
		sound_start (ST_SAMPLE, SND_TOP_LANE_LIT, SL_500MS, PRI_GAME_QUICK3);
		score (SC_5K);
	}
	else
	{
		sound_start (ST_SAMPLE, SND_TOP_LANE_UNLIT, SL_500MS, PRI_GAME_QUICK5);
		score (SC_25K);
		lamp_on (lamp);

		if (lamp_test (LM_TOP_LANE_LEFT)
			&& lamp_test (LM_TOP_LANE_RIGHT))
		{
			task_create_anon (toplane_complete);
		}
	}
}

CALLSET_ENTRY (toplane, sw_upper_left_lane)
{
	toplane_award (LM_TOP_LANE_LEFT);
}

CALLSET_ENTRY (toplane, sw_upper_right_lane)
{
	toplane_award (LM_TOP_LANE_RIGHT);
}

CALLSET_ENTRY (toplane, sw_left_button, sw_right_button)
{
	lamplist_rotate_next (LAMPLIST_TOP_LANES, lamp_matrix);
}

