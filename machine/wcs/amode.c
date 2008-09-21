
#include <freewpc.h>

static U8 lamplist;

CALLSET_ENTRY (wcs_amode, start_without_credits)
{
	sound_send (SND_WHISTLE);
}


void amode_pgj_logo (void)
{
	dmd_map_lookaside (0);
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 44, 25, "10");
	font_render_string_center (&font_var5, 64, 27, "YEARS");
	dmd_shadow_copy ();

	dmd_alloc_low_high ();
	dmd_draw_fif (fif_pgj);
	dmd_overlay_alpha (wpc_dmd_get_mapped (), 0);
	dmd_show2 ();
	task_sleep_sec (15);
	deff_exit ();
}

void amode_leff1 (void)
{
	register U8 my_lamplist = lamplist;
	lamplist_set_apply_delay (TIME_66MS);
	for (;;)
	{
		lamplist_apply (my_lamplist, leff_toggle);
		task_sleep (TIME_100MS);
	}
}


void amode_leff (void)
{
	triac_leff_enable (TRIAC_GI_MASK);
	lamp_leff_free (LM_START_BUTTON);

	for (lamplist = LAMPLIST_GOAL_COUNTS; lamplist <= LAMPLIST_RAMP_TICKETS; lamplist++)
	{
		leff_create_peer (amode_leff1);
		task_sleep (TIME_33MS);
	}

	for (;;)
	{
		leff_toggle (LM_TRAVEL);
		leff_toggle (LM_TACKLE);
		leff_toggle (LM_GOAL_JACKPOT);
		leff_toggle (LM_FREE_KICK);
		leff_toggle (LM_FINAL_DRAW);
		leff_toggle (LM_LIGHT_KICKBACK);
		leff_toggle (LM_ULTRA_RAMP_COLLECT);

		task_sleep (TIME_100MS);

		leff_toggle (LM_WORLD_CUP_FINAL);
		leff_toggle (LM_GOAL);
		leff_toggle (LM_ULTRA_RAMP_COLLECT);
		leff_toggle (LM_TV_AWARD);
		leff_toggle (LM_LIGHT_MAGNA_GOALIE);

		task_sleep (TIME_100MS);
	}
}

