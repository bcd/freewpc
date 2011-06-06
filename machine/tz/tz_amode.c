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

/* We have to inline these so the return skips the page properly */
static inline void show_silverball (const char *line1, const char *line2)
{
	amode_page_start ();
	U16 fno;
	for (fno = IMG_SILVERBALL_START; fno <= IMG_SILVERBALL_END; fno += 2)
	{
		dmd_alloc_pair ();
		frame_draw (fno);
		dmd_show2 ();
		task_sleep (TIME_66MS);
		if (amode_page_changed)
			break;
	}
	amode_sleep_sec (1);
	for (fno = 0; fno < 7; fno++)
	{
		dmd_alloc_pair_clean ();
		dmd_map_overlay ();
		dmd_clean_page_low ();
		font_render_string_center (&font_steel, 64, 12, line1);
		if (fno > 2)
			font_render_string_center (&font_var5, 64, 24, line2);
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (IMG_SILVERBALL_END);
		dmd_overlay_outline ();
		dmd_show2 ();
		amode_sleep_sec (1);
	}
	dmd_sched_transition (&trans_bitfade_slow);
}

static inline void show_bcd (void)
{
	amode_page_start ();
	dmd_sched_transition (&trans_random_boxfade);
	dmd_alloc_pair_clean ();
	/* Draw a 'white' background for the transition */
	dmd_invert_page (dmd_low_buffer);
	dmd_invert_page (dmd_high_buffer);
	dmd_show2 ();
	/* bcd should emerge from the right */
	dmd_sched_transition (&trans_scroll_left);
	dmd_alloc_pair ();
	frame_draw (IMG_BCD);
	/* Start the transition */
	dmd_show2 ();
	/* Paint the text on */
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_var5, 38, 10, "HEY ITS ONLY");
	font_render_string_center (&font_steel, 38, 20, "PINBALL");
	dmd_text_outline ();
	dmd_alloc_pair ();
	frame_draw (IMG_BCD);
	dmd_overlay_outline ();
	dmd_show2 ();
	sound_send (SND_HEY_ITS_ONLY_PINBALL);
	amode_sleep_sec (3);
	/* Now just draw bcd again */
	dmd_alloc_pair ();
	frame_draw (IMG_BCD);
	dmd_show2 ();
	/* Exit stage right */
	dmd_sched_transition (&trans_scroll_right);
	/* Draw a 'white' background for the transition again */
	dmd_alloc_pair_clean ();
	dmd_invert_page (dmd_low_buffer);
	dmd_invert_page (dmd_high_buffer);
	dmd_show2 ();
}

static void amode_talking_task (void)
{
	sound_send (SND_NOT_AN_ORDINARY_DAY);
	task_sleep_sec (2);
	sound_send (SND_OR_AN_ORDINARY_PLAYER);
	task_exit ();
}

static inline void show_sonny_jim (void)
{
	amode_page_start ();
	dmd_alloc_pair_clean ();
	/* Draw a 'white' background for the transition */
	dmd_invert_page (dmd_low_buffer);
	dmd_invert_page (dmd_high_buffer);
	dmd_show2 ();
	/* sonny_jim should emerge from the bottom */
	dmd_sched_transition (&trans_scroll_up);
	dmd_alloc_pair ();
	frame_draw (IMG_SONNY_JIM);
	/* Start the transition */
	dmd_show2 ();
	/* Paint the text on */
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_right (&font_var5, 126, 7, "NOT AN ORDINARY DAY OR");
	font_render_string_right (&font_var5, 122, 18, "AN ORDINARY PLAYER");
	dmd_text_outline ();
	dmd_alloc_pair ();
	frame_draw (IMG_SONNY_JIM);
	dmd_overlay_outline ();
	dmd_show2 ();
	task_create_anon (amode_talking_task);
	amode_sleep_sec (4);
	/* Now just draw sonny_jim again */
	dmd_alloc_pair ();
	frame_draw (IMG_SONNY_JIM);
	dmd_show2 ();
	/* Exit stage right */
	dmd_sched_transition (&trans_scroll_down);
	/* Draw a 'white' background for the transition again */
	dmd_alloc_pair_clean ();
	dmd_invert_page (dmd_low_buffer);
	dmd_invert_page (dmd_high_buffer);
	dmd_show2 ();
	
	dmd_sched_transition (&trans_bitfade_slow);
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
	dmd_alloc_pair_clean ();
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
#if 0	
static inline void draw_bttzmelt (void)
{
	amode_page_start ();
	U16 fno;
	dmd_alloc_pair_clean ();
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
#endif
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

static void show_popups (void)
{
	/* We can't get our names on the playfield, but at least we can get our
	 * ugly mugs on the DMD :-)
	 */
	U8 p;
	/* 33% chance of happening */
	if (random_scaled (3) != 1)
		return;
	/* Scan through scores and do popups if qualified */
	for (p = 0; p < num_players; p++)
	{
		if (score_compare (score_table[SC_20M], scores[p]) == 1
			&& score_compare (scores[p], score_table[SC_10]) == 1)
		{
			show_bcd ();
			return;
		}
	}
	
	for (p = 0; p < num_players; p++)
	{
		if (score_compare (scores[p], score_table[SC_500M]) == 1)
		{
			show_sonny_jim ();
			return;
		}
	}

	//show_hydra();
}

CALLSET_ENTRY (tz_amode, amode_page)
{
	if (amode_show_scores_long)
		return;
	show_popups ();

	if (random_scaled (4) == 1)
		show_silverball ("PINBALL", "HAVE YOU GOT THE BALLS?");
	else
		show_silverball ("FREEWPC", "MAKING OLD NEW AGAIN");
	
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

	//draw_bttzmelt ();
	//if (amode_show_scores_long)
	//	return;
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
