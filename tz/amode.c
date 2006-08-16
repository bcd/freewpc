/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
#include <tz/clock.h>

U8 egg_code_values[3];
U8 egg_index;

extern void starfield_start (void);
extern void starfield_stop (void);


bool amode_page_delay (U8 secs)
{
	U8 amode_flippers;
	U8 amode_flippers_start;

	/* Convert secs to 66ms ticks */
	secs <<= 4;

	amode_flippers_start = switch_poll_logical (SW_LL_FLIP_SW);
	while (secs != 0)
	{
		task_sleep (TIME_66MS);
		amode_flippers = switch_poll_logical (SW_LL_FLIP_SW);

		if ((amode_flippers != amode_flippers_start) &&
			 (amode_flippers != 0))
		{
			return TRUE;
		}
		amode_flippers_start = amode_flippers;
		secs--;
	}
	return FALSE;
}

void amode_flipper_sound_debounce_timer (void)
{
	task_sleep_sec (10);
	task_sleep_sec (10);
	task_sleep_sec (10);
	task_exit ();
}

void amode_flipper_sound (void)
{
	if (!task_find_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE))
	{
		task_create_gid (GID_AMODE_FLIPPER_SOUND_DEBOUNCE,
			amode_flipper_sound_debounce_timer);
		sound_send (SND_THUD);
	}
}

void amode_scroll (void)
{
}


void egg_left_flipper (void)
{
	if (!task_find_gid (GID_EGG_TIMER))
	{
		timer_start1_free (GID_EGG_TIMER, TIME_7S);
		egg_index = 0;
		egg_code_values[0] = 0;
		egg_code_values[1] = 0;
		egg_code_values[2] = 0;
	}
	egg_code_values[egg_index]++;
}

void egg_brian_image_deff (void)
{
	dmd_alloc_low_high ();
	dmd_draw_image2 (brianhead_bits);
	font_render_string_center (&font_term6, 76, 11, "ARE YOU READY");
	dmd_flip_low_high ();
	font_render_string_center (&font_term6, 76, 22, "TO BATTLE...");
	dmd_flip_low_high ();
	dmd_show2 ();
	task_sleep_sec (8);
	deff_exit ();
}

void egg_right_flipper (void)
{
	if (!task_find_gid (GID_EGG_TIMER)) 
		return;
	egg_index++;
	if (egg_index == 3)
	{
		dbprintf ("\nEgg code %d %d %d entered\n", egg_code_values[0], egg_code_values[1], egg_code_values[2]);
		if ((egg_code_values[0] == 2) &&
			 (egg_code_values[1] == 3) &&
			 (egg_code_values[2] == 4))
		{
			deff_start (DEFF_BRIAN_IMAGE);
		}
	}
}

void amode_left_flipper (void)
{
	amode_flipper_sound ();
	amode_scroll ();
	egg_left_flipper ();
}


void amode_right_flipper (void)
{
	amode_flipper_sound ();
	amode_scroll ();
	egg_right_flipper ();
}

void amode_lamp_toggle_task (void) __taskentry__
{
	lampset_apply_leff_toggle (LAMPSET_AMODE_ALL);
	task_exit ();
}


void amode_leff (void) __taskentry__
{
	U8 i;

	triac_enable (TRIAC_GI_MASK);
	lampset_set_apply_delay (0);
	for (;;)
	{
		lampset_apply_leff_off (LAMPSET_AMODE_ALL);

		lampset_set_apply_delay (TIME_16MS);
		for (i=0; i < 6; i++)
		{
			lampset_apply_leff_toggle (LAMPSET_AMODE_ALL);
			task_sleep (TIME_100MS * 3);
		}

		lampset_set_apply_delay (0);
		lampset_apply_leff_alternating (LAMPSET_AMODE_ALL, 0);
		for (i=0; i < 20; i++)
		{
			lampset_apply_leff_toggle (LAMPSET_AMODE_ALL);
			task_sleep (TIME_100MS * 2);
		}

		lampset_apply_leff_alternating (LAMPSET_AMODE_RAND, 0);
		for (i=0; i < 60; i++)
		{
			lampset_apply_leff_toggle (LAMPSET_AMODE_RAND);
			task_sleep (TIME_100MS);
		}
	}
}


void amode_show_design_credits (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 3, "FREEWPC");
	font_render_string_center (&font_mono5, 64, 12, "BY BRIAN DOMINY");
	font_render_string_center (&font_mono5, 64, 19, "RELEASED UNDER THE");
	font_render_string_center (&font_mono5, 64, 25, "GNU PUBLIC LICENSE");
	dmd_sched_transition (&trans_scroll_up_slow);
	dmd_show_low ();

	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 5, "VISIT ODDCHANGE.COM");
	font_render_string_center (&font_mono5, 64, 12, "FOR MORE INFO");
	dmd_sched_transition (&trans_scroll_up_slow);
	dmd_show_low ();

	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_up_slow);
	dmd_show_low ();
}


void amode_deff (void) __taskentry__
{
	int design_credit_counter = 3;

	tz_clock_reset ();
#if 1
	dmd_alloc_low_high ();
	dmd_draw_image2 (tileback0_bits);
	dmd_show2 ();
	task_sleep_sec (10);
#endif
	for (;;)
	{
		/** Display last set of player scores **/
		dmd_alloc_low_clean ();
		scores_draw ();
		dmd_show_low ();
		if (amode_page_delay (5) && system_config.tournament_mode)
			continue;

		/** Display FreeWPC logo **/
		dmd_alloc_low_high ();
		dmd_draw_image2 (freewpc_logo_bits);
		dmd_sched_transition (&trans_random_boxfade);
		dmd_show2 ();
		if (amode_page_delay (5) && system_config.tournament_mode)
			continue;

		/** Display credits message **/
		credits_draw ();

		/** Display game title message **/
		dmd_alloc_low_high ();
		dmd_clean_page_low ();
		font_render_string_center (&font_mono5, 64, 10, "BACK TO THE ZONE");
		starfield_start ();
		dmd_copy_low_to_high ();
		font_render_string_center (&font_mono5, 64, 20, "TZ 2006");
		deff_swap_low_high (19, TIME_100MS * 2);
		starfield_stop ();

		/** Display high scores **/
		high_score_amode_show ();

		/** Display PLAY PINBALL message **/
		dmd_sched_transition (&trans_scroll_left);
		dmd_alloc_low_clean ();
		font_render_string_center (&font_fixed10, 64, 16, 
			"PLAY PINBALL");
		dmd_show_low ();
		if (amode_page_delay (3) && system_config.tournament_mode)
			continue;

		/** Display 'custom message'? **/

		/* Display date/time */
		rtc_show_date_time ();
		if (amode_page_delay (5) && system_config.tournament_mode)
			continue;

		/* Kill music if it is running */
		music_set (MUS_OFF);

		if (--design_credit_counter == 0)
		{
			design_credit_counter = 3;
			amode_show_design_credits ();
		}
	}
}

