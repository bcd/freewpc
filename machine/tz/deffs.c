/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>

void cow_deff (void)
{
	dmd_alloc_pair ();
	frame_draw (IMG_COW);
	font_render_string_center (&font_var5, 40, 11, "THE POWER");
	font_render_string_center (&font_var5, 40, 22, "SAYS ...");
	dmd_show2 ();
	task_sleep_sec (2);
	
	sound_send (SND_POWER_GRUNT_1);
	dmd_alloc_pair ();
	frame_draw (IMG_COW);
	font_render_string_center (&font_fixed6, 40, 11, "MOO");
	dmd_show2 ();
	task_sleep_sec (4);
	deff_exit ();
}

void flash_and_exit_deff (U8 flash_count, task_ticks_t flash_delay)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (flash_count, flash_delay);
	deff_exit ();
}

void flash_small_deff (U8 flash_count, task_ticks_t flash_delay)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (flash_count, flash_delay);
}

void extra_ball_deff (void)
{
	sound_send (SND_HERES_YOUR_EB);
	U16 fno;
	for (fno = IMG_EBALL_START; fno <= IMG_EBALL_END; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	task_sleep_sec (2);
	deff_exit ();
}

void driver_deff (void)
{
	U16 fno;
	U8 i;
	for (i = 0; i < 3; i++)
	{
		for (fno = IMG_DRIVER_START; fno <= IMG_DRIVER_END; fno += 2)
		{
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
	deff_exit ();
}

void printf_millions (U8 n)
{
	sprintf ("%d,000,000", n);
}

void printf_thousands (U8 n)
{
	sprintf ("%d,000", n);
}

void replay_deff (void)
{
	sprintf ("REPLAY");
	flash_and_exit_deff (20, TIME_66MS);
}


void special_deff (void)
{
	sprintf ("SPECIAL");
	flash_and_exit_deff (20, TIME_100MS);
}

void pb_jackpot_deff (void)
{
	sprintf ("PB JACKPOT");
	flash_and_exit_deff (20, TIME_100MS);
}

void two_way_combo_deff (void)
{
	sprintf ("2 WAY COMBO");
	flash_and_exit_deff (15, TIME_66MS);
}

void shoot_camera_deff (void)
{
	sprintf ("SHOOT CAMERA");
	flash_and_exit_deff (15, TIME_66MS);
}

void shoot_jackpot_deff (void)
{
	sprintf ("SHOOT PIANO");
	flash_and_exit_deff (15, TIME_66MS);
}

void home_and_dry_deff (void)
{
	sound_send (SND_BIG_RISK_BIG_REWARD);
	sprintf ("HOME AND DRY");
	flash_small_deff (15, TIME_33MS);
	sprintf ("YOU HAVE WON");
	flash_and_exit_deff (15, TIME_66MS);
}

void in_the_lead_deff (void)
{
	sound_send (SND_GO_FOR_THE_HILL);
	sprintf ("IN THE LEAD");
	flash_small_deff (15, TIME_33MS);
	sprintf ("GO FOR IT");
	flash_and_exit_deff (15, TIME_66MS);
}

void get_ready_to_doink_deff (void)
{
	sprintf ("DOINK ALERT");
	flash_small_deff (15, TIME_33MS);
	sprintf ("DOINK ALERT");
	flash_and_exit_deff (15, TIME_66MS);
}

void lucky_bounce_deff (void)
{
	sprintf ("LUCKY");
	flash_small_deff (15, TIME_33MS);
	sprintf ("BOUNCE");
	flash_small_deff (15, TIME_33MS);
	deff_exit ();
}

void ball_from_lock_deff (void)
{
	sprintf ("WATCH OUT");
	flash_small_deff (10, TIME_33MS);
	sprintf ("BALL FROM LOCK");
	flash_small_deff (10, TIME_200MS);
	deff_exit ();
}

void button_masher_deff (void)
{
	sprintf ("IS MIKE PLAYING");
	flash_small_deff (10, TIME_66MS);
	sprintf ("BUTTON MASHER");
	flash_small_deff (20, TIME_33MS);
	deff_exit ();
}

void mb_ten_million_added_deff (void)
{
	sprintf ("10 MILLION ADDED");
	flash_small_deff (20, TIME_33MS);
	sprintf ("TO JACKPOT");
	flash_small_deff (20, TIME_66MS);
	deff_exit ();
}

void three_way_combo_deff (void)
{
	sprintf ("3 WAY COMBO");
	flash_and_exit_deff (20, TIME_66MS);
}

/* Jackpot animation contributed by highrise */
void jackpot_deff (void)
{
	U16 fno;
	U8 i;
	sample_start (SND_JACKPOT_BACKGROUND, SL_1S);
	/* Loop the start if the animation 3 times */
	for (i = 3; i > 0; --i)
	{
		for (fno = IMG_JACKPOT_START; fno <= IMG_JACKPOT_END - 9; fno += 1)
		{
			//dmd_alloc_low_high ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_show2 ();
			task_sleep (i);
		}
	}
	/* Show once fully */
	for (fno = IMG_JACKPOT_START; fno <= IMG_JACKPOT_END - 2; fno += 1)
	{
			//dmd_alloc_low_high ();
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_show2 ();
			task_sleep (TIME_16MS);
	}
	/* Flash Jackpot */
	dmd_alloc_pair ();
	frame_draw (IMG_JACKPOT_END - 2);
	dmd_show2 ();
	sample_start (SND_JACKPOT, SL_100MS);
	task_sleep (TIME_200MS);
	
	dmd_alloc_pair ();
	frame_draw (IMG_JACKPOT_END);
	dmd_show2 ();
	sample_start (SND_JACKPOT, SL_100MS);
	task_sleep (TIME_500MS);

	dmd_alloc_pair ();
	frame_draw (IMG_JACKPOT_END - 2);
	dmd_show2 ();
	sample_start (SND_JACKPOT, SL_100MS);
	task_sleep (TIME_500MS);

	dmd_alloc_pair ();
	frame_draw (IMG_JACKPOT_END);
	dmd_show2 ();
	sound_send (SND_JACKPOT);
	task_sleep_sec (1);
	sound_send (SND_FIST_BOOM2);
	deff_exit ();
}

U16 tv_static_data[] = {
	0x4964UL, 0x3561UL, 0x2957UL, 0x1865UL, 
	0x8643UL, 0x8583UL, 0x18C9UL, 0x9438UL,
	0x2391UL, 0x1684UL, 0x6593UL,
};

void tv_static_deff (void)
{
	U8 loop;
	register U16 *dmd;
	U8 r;

	for (loop = 0; loop < 32; loop++)
	{
		dmd_alloc_pair ();
		dmd = (U16 *)dmd_low_buffer;
		while (dmd < (U16 *)dmd_high_buffer)
		{
			r = random_scaled (11);
			*dmd++ = tv_static_data[r++];
			*dmd++ = tv_static_data[r++];
		}

		dmd = (U16 *)dmd_high_buffer;
		while (dmd < ((U16 *)(dmd_high_buffer + DMD_PAGE_SIZE)))
		{
			r = random_scaled (11);
			*dmd++ = tv_static_data[r++];
			*dmd++ = tv_static_data[r++];
		}
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	deff_exit ();
}


void text_color_flash_deff (void)
{
	U8 count = 8;

	dmd_alloc_pair_clean ();
	font_render_string_center (&font_fixed10, 64, 9, "QUICK");
	font_render_string_center (&font_fixed10, 64, 22, "MULTIBALL");

	/* low = text, high = blank */
	while (--count > 0)
	{
		dmd_show2 ();
		task_sleep (TIME_100MS);

		dmd_flip_low_high ();
		dmd_show2 ();
		task_sleep (TIME_100MS);

		dmd_show_high ();
		task_sleep (TIME_200MS);

		dmd_show2 ();
		task_sleep (TIME_100MS);
		dmd_flip_low_high ();
	}

	deff_exit ();	
}


void spell_test_deff (void)
{
	U8 count = 4;
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	sprintf ("%*s", count, "FASTLOCK");
	font_render_string_left (&font_fixed10, 16, 9, sprintf_buffer);
	dmd_flip_low_high ();
	dmd_clean_page_low ();
	font_render_string_left (&font_fixed10, 16, 9, "FASTLOCK");
	dmd_flip_low_high ();
	dmd_show2 ();
	task_sleep_sec (3);
	deff_exit ();	
}


void two_color_flash_deff (void)
{
	U8 n;

	dmd_alloc_pair_clean ();
	font_render_string_center (&font_fixed6, 64, 21, "BRIGHT");
	dmd_copy_low_to_high ();
	font_render_string_center (&font_fixed6, 64, 9, "DARK");

	for (n = 0; n < 5; n++)
	{
		dmd_show2 ();
		task_sleep (TIME_300MS);
		dmd_show_high ();
		task_sleep (TIME_300MS);
	}
	deff_exit ();
}


void bg_flash_deff (void)
{
	const U8 flash_time = TIME_50MS;

	dmd_alloc_pair ();
	dmd_fill_page_low ();
	dmd_clean_page_high ();
	for (;;)
	{
		dmd_show_high ();
		task_sleep (flash_time); /* 0% */

		dmd_show2 ();
		task_sleep (flash_time); /* 33% */

		dmd_flip_low_high ();
		dmd_show2 ();
		dmd_flip_low_high ();
		task_sleep (flash_time * 2); /* 66% */

		dmd_show2 ();
		task_sleep (flash_time); /* 33% */
	}
}

void ball_drain_outlane_deff (void)
{
	U16 fno;
	for (fno = IMG_VOID_START; fno <= IMG_VOID_END; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	dmd_sched_transition (&trans_scroll_down_fast);
	deff_exit ();
}

void ball_explode_deff (void)
{
	U16 fno;
	extern bool powerball_death;
	if (!multi_ball_play () && !ballsave_test_active ())
		music_request (MUS_POWERFIELD, PRI_GAME_MODE1);

	dmd_alloc_pair_clean ();
	dmd_show2 ();
	task_sleep (TIME_200MS);

	dmd_sched_transition (&trans_scroll_down_fast);
	dmd_alloc_pair ();
	frame_draw (IMG_BALLEXPLODE_START);
	dmd_show2 ();
	if (powerball_death == FALSE)	
		sound_send (SND_EXPLOSION_3);
	for (fno = IMG_BALLEXPLODE_START + 1; fno <= IMG_BALLEXPLODE_END; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	/* Play in reverse if ballsave is active */
	if (ballsave_test_active ())
	{
		sound_send (SND_TWILIGHT_ZONE_SHORT_SOUND);
		for (fno = IMG_BALLEXPLODE_END; fno >= IMG_BALLEXPLODE_START; fno -= 2)
		{
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
	deff_exit ();
}

void tz_ball_save_deff (void)
{
	U16 fno;
	sound_send (SND_STATIC);
	for (fno = IMG_BALLEXPLODE_END; fno >= IMG_BALLEXPLODE_START; fno -= 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
	}
	deff_exit ();
}

void rollover_completed_deff (void)
{
	sound_send (SND_GLASS_BREAKS);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 8, "ROLLOVER");
	font_render_string_center (&font_fixed6, 64, 18, "COMPLETED");
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
}
