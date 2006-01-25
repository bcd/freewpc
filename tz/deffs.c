
#include <freewpc.h>

void flash_and_exit_deff (U8 flash_count, task_ticks_t flash_delay)
{
	dmd_alloc_low_high ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (flash_count, flash_delay);
	deff_exit ();
}

void printf_millions (U8 n)
{
	printf ("%d,000,000", n);
}

void printf_thousands (U8 n)
{
	printf ("%d,000", n);
}

void replay_deff (void)
{
	printf ("REPLAY");
	flash_and_exit_deff (20, TIME_100MS);
}

void extra_ball_deff (void)
{
	printf ("EXTRA BALL");
	flash_and_exit_deff (20, TIME_100MS);
}

void special_deff (void)
{
	printf ("SPECIAL");
	flash_and_exit_deff (20, TIME_100MS);
}

void jackpot_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("JACKPOT");
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	printf_millions (50);
	font_render_string_center (&font_fixed6, 64, 21, sprintf_buffer);
	dmd_sched_transition (&trans_scroll_up);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void ballsave_deff (void)
{
	dmd_alloc_low_high ();
	dmd_clean_page_low ();
	printf ("PLAYER %d", player_up);
	font_render_string_center (&font_fixed6, 64, 10, sprintf_buffer);
	dmd_copy_low_to_high ();
	font_render_string_center (&font_fixed6, 64, 21, "BALL SAVED");
	dmd_show_low ();
	deff_swap_low_high (32, TIME_100MS);
	deff_exit ();
}



