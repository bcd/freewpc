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

__local__ U8 ball_save_count;


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
	sprintf ("%d,000,000", n);
}

void printf_thousands (U8 n)
{
	sprintf ("%d,000", n);
}

void replay_deff (void)
{
	sprintf ("REPLAY");
	flash_and_exit_deff (20, TIME_100MS);
}

void extra_ball_deff (void)
{
	sprintf ("EXTRA BALL");
	flash_and_exit_deff (20, TIME_100MS);
}

void special_deff (void)
{
	sprintf ("SPECIAL");
	flash_and_exit_deff (20, TIME_100MS);
}

void jackpot_deff (void)
{
	U8 i;
	for (i=1; i < 8; i++)
	{
		dmd_alloc_low_clean ();
		sprintf ("JACKPOT");
		if (i < 7)
			sprintf_buffer[i] = '\0';
		font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
		dmd_show_low ();
		sound_send (SND_CLOCK_CHAOS_END_BOOM);
		task_sleep (TIME_300MS);
	}

	for (i=0; i < 8; i++)
	{
		dmd_sched_transition (&trans_scroll_up);
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed10, 64, 16, "JACKPOT");
		dmd_show_low ();
	}

	for (i=0; i < 8; i++)
	{
		dmd_sched_transition (&trans_scroll_up);
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed10, 64, 8, "JACKPOT");
		font_render_string_center (&font_fixed10, 64, 24, "JACKPOT");
		dmd_show_low ();
	}

	dmd_alloc_low_high ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 16, "JACKPOT");
	dmd_copy_low_to_high ();
	dmd_show_low ();
	dmd_invert_page (dmd_low_buffer);
	deff_swap_low_high (25, TIME_100MS);
	task_sleep (TIME_500MS);
	deff_exit ();
}

void ball_save_deff (void)
{
	dmd_alloc_low_high ();
	dmd_clean_page_low ();
	sprintf ("PLAYER %d", player_up);
	font_render_string_center (&font_fixed6, 64, 8, sprintf_buffer);
	dmd_copy_low_to_high ();
	font_render_string_center (&font_fixed6, 64, 22, "BALL SAVED");
	dmd_show_low ();

	switch (ball_save_count)
	{
		case 0:
			sound_send (SND_POWER_HUH_3);
			break;
		case 1:
			sound_send (SND_POWER_HUH_4);
			break;
		default:
			break;
	}
	ball_save_count++;

	deff_swap_low_high (24, TIME_100MS);
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
		dmd_alloc_low_high ();

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
	dmd_alloc_low_high ();
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

	dmd_alloc_low_high ();
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


CALLSET_ENTRY (tz_deff, start_player)
{
	ball_save_count = 0;
}

