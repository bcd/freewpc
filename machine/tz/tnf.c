
/* CALLSET_SECTION (__machine3__, tnf) */

#include <freewpc.h>

extern U8 mpf_buttons_pressed;
/* used to randomise the position of the DOINK text */
U8 tnf_x;
U8 tnf_y;

/* Total amount scored from doink mode */
score_t tnf_score;

void tnf_deff (void)
{
	while (task_find_gid (GID_TNF_RUNNING))
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_mono5, 64, 4, "HIT FLIPPER BUTTONS");
		psprintf ("%d DOINK", "%d DOINKS", mpf_buttons_pressed);
		font_render_string_center (&font_term6, 64 + tnf_x, 16 + tnf_y, sprintf_buffer);
		dmd_show_low ();
		task_sleep (TIME_33MS);
	}
	deff_exit ();
}

void tnf_exit_deff (void)
{
	sound_send (SND_OOH_GIMME_SHELTER);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 4, "POINTS EARNED FROM DOINKS");
	sprintf_score (tnf_score);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

CALLSET_ENTRY (tnf, tnf_button_pushed)
{
	bounded_increment (mpf_buttons_pressed, 255);
	sound_send (SND_HITCHHIKER_COUNT);
	score_add (tnf_score, score_table[SC_250K]);
	tnf_x = random_scaled(10);
	tnf_y = random_scaled(8);
}

CALLSET_ENTRY (tnf, tnf_start)
{
	flipper_disable ();
	music_disable ();
	mpf_buttons_pressed = 1;
	score_zero (tnf_score);
	tnf_x = 0;
	tnf_y = 0;
	leff_start (LEFF_NO_GI);
	timer_restart_free (GID_TNF_RUNNING, TIME_3S);
	deff_start_sync (DEFF_TNF);
	callset_invoke (tnf_end);
}

CALLSET_ENTRY (tnf, tnf_end)
{
	score_add (current_score, tnf_score);
	flipper_enable ();
	music_enable ();
	music_refresh ();
	magnet_enable_catch_and_throw (MAG_LEFT);
	deff_start_sync (DEFF_TNF_EXIT);
}
