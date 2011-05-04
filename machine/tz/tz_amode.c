/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
 *
 * Additional code by Ewan Meadows <sonny_jim@hotmail.com>
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
#include <amode.h>

extern U8 amode_page_changed;
extern bool amode_show_scores_long;

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

static inline void show_driver_animation (void)
{
	amode_page_start ();
	/* Show driver animation */	
	U16 fno;
	U8 i;
	for (i = 0; i < 5; i++)
	{
		for (fno = IMG_DRIVER_START; fno <= IMG_DRIVER_END; fno += 2)
		{
			/* We are drawing a full frame, so a clean isn't needed */
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_show2 ();
			task_sleep (TIME_66MS);
			if (amode_page_changed)
				break;
		}
	}
}

void show_text_on_stars (void)
{
	amode_page_start ();
	U8 n;
	for (n = 0; n < 40; n++)
	{
		dmd_dup_mapped ();
		dmd_overlay_onto_color ();
		star_draw ();
		dmd_show2 ();
		if (amode_page_changed)
			break;
		task_sleep (TIME_100MS);
		dmd_map_overlay ();
	}
}

static void map_and_clean (void)
{
	dmd_map_overlay ();
	dmd_clean_page_high ();
	dmd_clean_page_low ();
}

static inline void draw_bttzwave (void)
{
	amode_page_start ();
	U16 fno;
	U8 i;
	for (i = 0; i < 5; i++)
	{
		for (fno = IMG_BTTZWAVE_START; fno <= IMG_BTTZWAVE_END; fno += 2)
		{
			dmd_alloc_pair ();
			frame_draw (fno);
			dmd_show2 ();
			task_sleep (TIME_100MS);
			if (amode_page_changed)
				break;
		}
	}
}
	
static inline void draw_bttzmelt (void)
{
	amode_page_start ();
	U16 fno;
	for (fno = IMG_BTTZMELT_START; fno <= IMG_BTTZMELT_MIDDLE; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		if (amode_page_changed)
			return;
		task_sleep (TIME_100MS);
	}
	amode_sleep_sec (2);
	for (fno = IMG_BTTZMELT_MIDDLE; fno <= IMG_BTTZMELT_END; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		if (amode_page_changed)
			break;
		task_sleep (TIME_100MS);
	}
}

static inline void scroll_text (void)
{
	amode_page_start ();
	dmd_sched_transition (&trans_bitfade_slow);
	/* Clean the low screen for the transition scroll*/
	dmd_alloc_low_clean ();
	if (amode_show_scores_long || amode_page_changed)
		return;
	dmd_show_low ();

	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 5, "FREEWPC WAS DESIGNED");
	font_render_string_center (&font_var5, 64, 12, "BY BRIAN DOMINY AND IS");
	font_render_string_center (&font_var5, 64, 19, "RELEASED UNDER THE GNU");
	font_render_string_center (&font_var5, 64, 26, "GENERAL PUBLIC LICENSE.");
	
	dmd_sched_transition (&trans_scroll_up_slow);
	if (amode_show_scores_long || amode_page_changed)
		return;
	dmd_show_low ();

	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 5, "VISIT WWW.ODDCHANGE.COM");
	font_render_string_center (&font_var5, 64, 12, "FOR MORE INFORMATION.");
	
	dmd_sched_transition (&trans_scroll_up_slow);
	if (amode_show_scores_long || amode_page_changed)
		return;
	dmd_show_low ();

	dmd_alloc_low_clean ();
	
	dmd_sched_transition (&trans_scroll_up_slow);
	if (amode_show_scores_long || amode_page_changed)
		return;
	dmd_show_low ();
}

CALLSET_ENTRY (tz_amode, amode_page)
{
	if (amode_show_scores_long)
		return;
	map_and_clean ();
	font_render_string_center (&font_fixed10, 64, 22, "THE ZONE");
	dmd_text_blur ();
	font_render_string_center (&font_fixed6, 64, 7, "BACK TO");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;

	map_and_clean ();
	font_render_string_center (&font_steel, 64, 7, "SOFTWARE BY");
	font_render_string_center (&font_steel, 64, 21, "BCD");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_steel, 64, 7, "AND");
	font_render_string_center (&font_steel, 64, 20, "SONNY JIM");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_var5, 64, 7, "ARTWORK AND");
	font_render_string_center (&font_var5, 64, 20, "ANIMATIONS BY");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_steel, 64, 16, "HIGHRISE");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_steel, 64, 16, "OPEN GAME ART");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_steel, 64, 7, "AND");
	font_render_string_center (&font_steel, 64,20, "POW STUDIOS");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_var5, 64, 16, "THANKS GO TO");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_steel, 64, 10, "HYDRA");
	font_render_string_center (&font_var5, 64, 23, "FREEWPC CORVETTE");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_steel, 64, 10, "METALLIK");
	font_render_string_center (&font_var5, 64, 23, "HARDWARE TESTING");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_steel, 64, 10, "LITZ");
	font_render_string_center (&font_var5, 64, 23, "RULES AND IDEAS");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_var5, 64, 7, "AND EVERYBODY IN");
	font_render_string_center (&font_var5, 64, 20, "EFNET #PINBALL");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_var5, 64, 7, "PRESS BUYIN BUTTON");
	font_render_string_center (&font_var5, 64, 20, "TO DISPLAY RULES");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;

	map_and_clean ();
	font_render_string_center (&font_var5, 64, 7, "HOLD LEFT FLIPPER TO");
	font_render_string_center (&font_var5, 64, 20, "START TOURNAMENT");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	map_and_clean ();
	font_render_string_center (&font_var5, 64, 7, "HOLD RIGHT FLIPPER TO");
	font_render_string_center (&font_var5, 64, 20, "SHOW LAST SCORES");
	show_text_on_stars ();
	if (amode_show_scores_long)
		return;
	
	show_random_factoid ();
	if (amode_show_scores_long)
		return;
	
	draw_bttzwave ();
	if (amode_show_scores_long)
		return;
	
	dmd_sched_transition (&trans_scroll_left);
	show_driver_animation ();
	if (amode_show_scores_long)
		return;
	
	scroll_text ();
	if (amode_show_scores_long)
		return;

	draw_bttzmelt ();
	if (amode_show_scores_long)
		return;
}

static void lock_and_outhole_monitor (void)
{
	/* Wait for balls to settle/amode to start before emptying
	 * locks/outhole */
	task_sleep_sec (3);
	while (!in_live_game)
	{
		if (switch_poll (SW_LOCK_LOWER))
		{
			device_request_kick (device_entry (DEVNO_LOCK));
		}

		if (switch_poll (SW_OUTHOLE))
		{
			sol_request (SOL_OUTHOLE);
		}
		if (!switch_poll (SW_AUTOFIRE2))
		{
			callset_invoke (clear_autofire);	
		}
		/* Wait for the balls to be cleared before starting again */
		task_sleep_sec (3);
	}
	task_exit ();
}

CALLSET_ENTRY (tz_amode, amode_start)
{
	task_create_gid (GID_LOCK_AND_OUTHOLE_MONITOR, lock_and_outhole_monitor);
}
