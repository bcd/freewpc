
#include <freewpc.h>

__local__ U8 jet_count;

__local__ U8 jet_goal;

__local__ U8 ultra_jet_count;


void jet_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 96, 5, "JET BUMPER");
	sprintf ("%d", jet_count);
	font_render_string_center (&font_fixed10, 96, 16, sprintf_buffer);

	if (jet_count == jet_goal)
		sprintf ("BONUS AWARDED");
	else
		sprintf ("BONUS AT %d", jet_goal);

	font_render_string_center (&font_var5, 64, 26, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void jet_goal_reset (void)
{
	jet_count = 0;
	jet_goal = 25;
}


void jet_goal_award (void)
{
	sound_start (ST_SAMPLE, MUS_TICKET_BOUGHT, SL_1S, PRI_GAME_QUICK5);
	jet_count = 0;
	if (jet_goal < 50)
		jet_goal += 5;
}


CALLSET_ENTRY (jets, sw_left_jet, sw_upper_jet, sw_lower_jet)
{
	++jet_count;
	if (lamp_flash_test (LM_ULTRA_JETS))
	{
		score (SC_25K);
		sound_start (ST_SAMPLE, SND_FIREWORK_EXPLODE, SL_500MS, PRI_GAME_QUICK5);
		if (++ultra_jet_count == 80)
		{
			lamp_tristate_on (LM_ULTRA_JETS);
		}
	}
	else
	{
		score (SC_5K);
		deff_start (DEFF_JET);
		task_sleep (TIME_16MS);
	}


	if (jet_count == jet_goal)
	{
		jet_goal_award ();
	}
	else
	{
		sound_start (ST_SAMPLE, SND_JET_BUMPER, SL_500MS, PRI_GAME_QUICK3);
	}
}

CALLSET_ENTRY (jets, start_player)
{
	jet_goal_reset ();
}

CALLSET_ENTRY (jets, start_ball)
{
	ultra_jet_count = 0;
}
