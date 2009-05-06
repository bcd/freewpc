
#include <freewpc.h>

#define BUILD_LOOP 0x1
#define BUILD_LRAMP 0x2
#define BUILD_POPPER 0x4
#define BUILD_RRAMP 0x8

#define MAX_GOALS 250

__local__ U8 lit_build_shots;

__local__ U8 goal_count;


void goal_lit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "GOAL IS LIT");
	dmd_show_low ();
	task_sleep (TIME_2S);
	deff_exit ();
}

void goal_scored_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 16, "GOAL SCORED");
	dmd_show_low ();
	task_sleep (TIME_2S);

	dmd_alloc_low_clean ();
	dmd_draw_border (dmd_low_buffer);
	sprintf ("GOAL %d", goal_count);
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	sprintf_score (score_deff_get ());
	font_render_string_center (&font_fixed6, 64, 21, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_2S);
	deff_exit ();
}


void goal_round_build_shot (U8 shot)
{
	if (flag_test (FLAG_MULTIBALL_RUNNING))
	{
	}
	else if ((lit_build_shots & shot) && !flag_test (FLAG_GOAL_LIT))
	{
		/* When Multiball is lit, don't expire the build lamps. */
		if (!flag_test (FLAG_MULTIBALL_LIT))
			lit_build_shots &= ~shot;
		flag_on (FLAG_GOAL_LIT);
		deff_start (DEFF_GOAL_LIT);
		sound_start (ST_MUSIC, MUS_GOAL_IS_LIT, SL_2S, PRI_GAME_QUICK4);
		score (SC_100K);
	}
	else
	{
		score (SC_25K);
	}
}

void goal_round_build_relight (void)
{
	lit_build_shots = 0xF;
}


void goal_count_lamp_update (void)
{
	lamplist_apply (LAMPLIST_GOAL_COUNTS, lamp_off);
	switch (goal_count % 5)
	{
		case 0:
			if (flag_test (FLAG_MULTIBALL_LIT))
				lamplist_apply (LAMPLIST_GOAL_COUNTS, lamp_on);
			break;
		case 1:
			lamp_on (LM_1_GOAL);
			break;
		case 2:
			lamp_on (LM_2_GOALS);
			break;
		case 3:
			lamp_on (LM_3_GOALS);
			break;
		case 4:
			lamp_on (LM_4_GOALS);
			break;
	}
}


CALLSET_ENTRY (goalround, left_ramp_shot)
{
	goal_round_build_shot (BUILD_LRAMP);
}

CALLSET_ENTRY (goalround, right_ramp_shot)
{
	goal_round_build_shot (BUILD_RRAMP);
}

CALLSET_ENTRY (goalround, striker_shot)
{
	goal_round_build_shot (BUILD_POPPER);
}

CALLSET_ENTRY (goalround, left_loop_shot)
{
	goal_round_build_shot (BUILD_LOOP);
}

CALLSET_ENTRY (goalround, goal_shot)
{
	if (flag_test (FLAG_GOAL_LIT))
	{
		flag_off (FLAG_GOAL_LIT);
		score (SC_1M);
		deff_start (DEFF_GOAL_SCORED);
		speech_start (SPCH_GOALLL, SL_2S);
		bounded_increment (goal_count, MAX_GOALS);
		VOIDCALL (ultra_add_shot);
		goal_count_lamp_update ();

		if (lit_build_shots == 0)
		{
			VOIDCALL (mb_light);

			/* Note that once 5 goals have been scored and
			Multiball is lit, all 4 Build Shots are relit so that
			Ultra Modes can still be enabled. */
			goal_round_build_relight ();
		}
	}
	else
	{
		sample_start (SND_UNLIT_GOAL, SL_2S);
	}
}



CALLSET_ENTRY (goalround, lamp_update)
{
	if (flag_test (FLAG_GOAL_LIT))
	{
		lamp_tristate_flash (LM_GOAL);
		lamplist_apply (LAMPLIST_BUILD_SHOTS, lamp_flash_off);
		/* strobe the rollovers */
	}
	else
	{
		lamp_tristate_off (LM_GOAL);
		lamp_flash_if (LM_L_LOOP_BUILD, lit_build_shots & BUILD_LOOP);
		lamp_flash_if (LM_L_RAMP_BUILD, lit_build_shots & BUILD_LRAMP);
		lamp_flash_if (LM_STRIKER_BUILD, lit_build_shots & BUILD_POPPER);
		lamp_flash_if (LM_R_RAMP_BUILD, lit_build_shots & BUILD_RRAMP);
	}

}


CALLSET_ENTRY (goalround, start_player)
{
	goal_round_build_relight ();
	flag_on (FLAG_GOAL_LIT);
}

CALLSET_ENTRY (goalround, start_ball)
{
}

