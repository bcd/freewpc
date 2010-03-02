/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (tz_amode, __machine2__) */
#include <freewpc.h>
//#include <coin.h>
//#include <highscore.h>

U8 egg_code_values[3];
U8 egg_index;


#if 0
/* Attract mode display delay function.
 * This function waits for the specified amount of time, but
 * returns immediately if either flipper is pressed.
 * Returns whether or not the timeout was aborted. */
bool amode_page_delay (U8 secs)
{
	U8 amode_flippers;
	U8 amode_flippers_start;

	/* Convert secs to 66ms ticks */
	secs <<= 4;

	amode_flippers_start = switch_poll_logical (SW_L_L_FLIPPER_BUTTON);
	while (secs != 0)
	{
		task_sleep (TIME_66MS);
		amode_flippers = switch_poll_logical (SW_L_L_FLIPPER_BUTTON);

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
#endif


/* TODO : use flipcode module to implement this */
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

void brian_image_deff (void)
{
	//dmd_alloc_low ();
	dmd_alloc_pair ();
	//dmd_draw_fif (fif_cow);
	font_render_string_center (&font_var5, 40, 11, "THE POWER");
	font_render_string_center (&font_var5, 40, 22, "SAYS ...");
	dmd_show_low ();
	task_sleep_sec (2);
	sound_send (SND_POWER_GRUNT_1);
	task_sleep_sec (4);
	deff_exit ();
}

void egg_right_flipper (void)
{
	if (!task_find_gid (GID_EGG_TIMER)) 
		return;
	egg_index++;
	if (egg_index == 3)
	{
		dbprintf ("\nEgg code %d %d %d entered\n", 
			egg_code_values[0], egg_code_values[1], egg_code_values[2]);
		if ((egg_code_values[0] == 2) &&
			 (egg_code_values[1] == 3) &&
			 (egg_code_values[2] == 4))
		{
			//deff_start (DEFF_BRIAN_IMAGE);
		}
	}
}


void amode_lamp_toggle_task (void)
{
	lamplist_apply (LAMPLIST_AMODE_ALL, leff_toggle);
	task_exit ();
}

U8 amode_leff_subset;

void amode_leff_subset_task (void)
{
	register U8 lamplist = amode_leff_subset;
	lamplist_set_apply_delay (TIME_100MS);
	for (;;)
		lamplist_apply (lamplist, leff_toggle);
}

void amode_leff (void)
{
	U8 i;
	triac_leff_enable (TRIAC_GI_MASK);

	for (;;)
	{
		amode_leff_subset = LAMPLIST_DOOR_PANELS_AND_HANDLE;
		leff_create_peer (amode_leff_subset_task);
		task_sleep (TIME_33MS);
	
		for (amode_leff_subset = LAMPLIST_DOOR_LOCKS_AND_GUMBALL;
			amode_leff_subset <= LAMPLIST_SPIRAL_AWARDS;
			amode_leff_subset++)
		{
			leff_create_peer (amode_leff_subset_task);
			task_sleep (TIME_33MS);
		}

		task_sleep_sec (15);
		task_kill_peers ();

		lamplist_apply_nomacro (LAMPLIST_SORT1, leff_off);
		lamplist_set_apply_delay (TIME_16MS);
		for (i=0 ; i < 10; i++)
			lamplist_apply (LAMPLIST_SORT1, leff_toggle);
	}
}

void show_driver_animation (void)
{
	/* Show driver animation */	
	U16 fno;
	U8 i;
	for (i = 0; i < 4; i++)
	{
		for (fno = IMG_DRIVER_START; fno <= IMG_DRIVER_END; fno += 2)
		{
			/* We are drawing a full frame, so a clean isn't needed */
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_show2 ();
			task_sleep (TIME_66MS);
		}
	}
	/* Clean both pages */
	dmd_alloc_pair_clean ();
}

void show_text_on_stars (void)
{
	U8 n;
	for (n = 0; n < 40; n++)
	{
		dmd_dup_mapped ();
		dmd_overlay_onto_color ();
		star_draw ();
		dmd_show2 ();
		task_sleep (TIME_100MS);
		dmd_map_overlay ();
	}
	dmd_alloc_pair_clean ();
}

CALLSET_ENTRY (tz_amode, amode_page)
{
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 22, "THE ZONE");
	dmd_text_blur ();
	font_render_string_center (&font_fixed6, 64, 7, "BACK TO");
	show_text_on_stars ();
	
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 64, 22, "BCD");
	dmd_text_blur ();
	font_render_string_center (&font_fixed10, 64, 7, "SOFTWARE BY");
	show_text_on_stars ();
	
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 64, 22, "HIGHRISE");
	dmd_text_blur ();
	font_render_string_center (&font_fixed10, 64, 7, "ANIMATIONS BY");
	show_text_on_stars ();
	
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_term6, 64, 22, "SONNY JIM");
	dmd_text_blur ();
	font_render_string_center (&font_fixed6, 64, 7, "ASSISTED BY");
	show_text_on_stars ();
	show_driver_animation ();
}

CALLSET_ENTRY (tz_amode, sw_buyin_button)
{
	//if (in_amode ())
//	{
		/* Start award descriptions */
		/* Spiralaward */
//		leff_stop (LEFF_AMODE);

	}
#if 0
void amode_show_design_credits (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();

	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 5, "FREEWPC WAS DESIGNED");
	font_render_string_center (&font_var5, 64, 12, "BY BRIAN DOMINY AND IS");
	task_sleep (TIME_16MS); /* drawing all of this text is slow; be nice */
	font_render_string_center (&font_var5, 64, 19, "RELEASED UNDER THE GNU");
	font_render_string_center (&font_var5, 64, 26, "GENERAL PUBLIC LICENSE.");
	dmd_sched_transition (&trans_scroll_up_slow);
	dmd_show_low ();

	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 5, "VISIT WWW.ODDCHANGE.COM");
	font_render_string_center (&font_var5, 64, 12, "FOR MORE INFORMATION.");
	dmd_sched_transition (&trans_scroll_up_slow);
	dmd_show_low ();

	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_up_slow);
	dmd_show_low ();
}
#endif
