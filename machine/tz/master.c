/*
 * Copyright 2006-2010 by Ewan Meadows <sonny_jim@hotmail.com>
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

/* CALLSET_SECTION (master, __machine3__) */

__permanent__ U8 loop_master_hi;
__permanent__ U8 combo_master_hi;
__permanent__ U8 spawny_get_hi;
__permanent__ char loop_master_initials[3];
__permanent__ char combo_master_initials[3];

U8 loop_master_initial_enter;
U8 combo_master_initial_enter;

extern U8 loops;
extern U8 combos;
extern U8 lucky_bounces;
extern char initials_data[3];
extern bool amode_show_scores_long;

void loop_master_entry_deff (void)
{
	dmd_alloc_pair_clean ();
	font_render_string_center (&font_mono5, 64, 8, "CONGRATULATIONS");
	font_render_string_center (&font_mono5, 64, 16, "NEW LOOP MASTER");
	font_render_string_center (&font_var5, 64, 22, "ENTER INITIALS");
	dmd_sched_transition (&trans_scroll_up);
	sound_send (SND_GLASS_BREAKS);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void loop_master_exit_deff (void)
{
	dmd_alloc_pair_clean ();
	sprintf ("%d LOOPS", loop_master_hi);
	font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
	sound_send (SND_CLOCK_GONG);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void combo_master_entry_deff (void)
{
	dmd_alloc_pair_clean ();
	font_render_string_center (&font_mono5, 64, 8, "CONGRATULATIONS");
	font_render_string_center (&font_mono5, 64, 16, "NEW COMBO MASTER");
	font_render_string_center (&font_var5, 64, 22, "ENTER INITIALS");
	dmd_sched_transition (&trans_scroll_up);
	sound_send (SND_GLASS_BREAKS);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

void combo_master_exit_deff (void)
{
	dmd_alloc_pair_clean ();
	sprintf ("%d COMBOS", combo_master_hi);
	font_render_string_center (&font_mono5, 64, 16, sprintf_buffer);
	sound_send (SND_CLOCK_GONG);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}



void loop_master_check (void)
{
	if (loop_master_initial_enter != 0 
		&& loop_master_initial_enter <= num_players)
	{
		loop_master_initial_enter = 0;
		deff_start_sync (DEFF_LOOP_MASTER_ENTRY);
		SECTION_VOIDCALL (__common__, initials_enter);
	//	loop_master_initials = initials_data;	
		deff_start_sync (DEFF_LOOP_MASTER_EXIT);
	}
}

void combo_master_check (void)
{
	if (combo_master_initial_enter != 0 
		&& combo_master_initial_enter <= num_players)
	{
		combo_master_initial_enter = 0;
		deff_start_sync (DEFF_COMBO_MASTER_ENTRY);
		SECTION_VOIDCALL (__common__, initials_enter);
	//	combo_master_initials = initials_data;	
		deff_start_sync (DEFF_COMBO_MASTER_EXIT);
	}
}

CALLSET_ENTRY (master, start_game)
{
	loop_master_initial_enter = 0;
	combo_master_initial_enter = 0;
}

CALLSET_ENTRY (master, factory_reset)
{
	loop_master_hi = 5;
	combo_master_hi = 2;
	//loop_master_initials = "FEK";
	//combo_master_initials = "FEK";
	spawny_get_hi = 1;
}

CALLSET_ENTRY (master, amode_page)
{
	if (amode_show_scores_long)
		return;
	dmd_sched_transition (&trans_vstripe_left2right);
	dmd_map_overlay ();
	dmd_clean_page_high ();
	dmd_clean_page_low ();
	//sprintf ("LOOP MASTER: %s %d LOOPS", loop_master_initials, loop_master_hi);
	psprintf ("LOOP MASTER: %d LOOP", "LOOP MASTER: %d LOOPS", loop_master_hi);
	font_render_string_center (&font_var5, 64, 10, sprintf_buffer);
	//sprintf ("COMBO MASTER: %s %d COMBOS", combo_master_initials, combo_master_hi);
	psprintf ("COMBO MASTER: %d COMBO", "COMBO MASTER: %d COMBOS", combo_master_hi);
	font_render_string_center (&font_var5, 64, 20, sprintf_buffer);
	show_text_on_stars ();
}
