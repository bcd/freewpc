/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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

void jackpot_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 10, "JACKPOT");
	dmd_show_low ();
	task_sleep (TIME_2S+TIME_166MS);
	deff_exit ();
}

void pf_mult_advance_deff (void) { deff_exit (); }

void pf_mult_shot_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 10, "ADVANCE SHOT");
	dmd_show_low ();
	task_sleep (TIME_2S+TIME_166MS);
	deff_exit ();
}

void martian_advance_deff (void) { deff_exit (); }
void martian_spelled_deff (void) { deff_exit (); }

void lower_lane_finish_deff (void) { deff_exit (); }
void luck_award_deff (void) { deff_exit (); }

void mb_running_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_mono5, 64, 5, "MULTIBALL");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		dmd_show_low ();
		do {
			task_sleep (TIME_166MS);
		} while (!score_update_required ());
	}
}

void mb_super_award_deff (void) { deff_exit (); }

void mb_increase_jackpot_deff (void)
{
	extern score_t mb_super_value;
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 7, "SUPER JACKPOT AT");
	sprintf_score (mb_super_value);
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_2S);
	deff_exit ();
}

void attack_lit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 10, "MARTIAN ATTACK");
	font_render_string_center (&font_fixed6, 64, 22, "IS LIT");
	dmd_show_low ();
	task_sleep (TIME_2S+TIME_166MS);
	deff_exit ();
}

void attack_running_deff (void)
{
	for (;;)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_mono5, 64, 5, "MARTIAN ATTACK");
		sprintf_current_score ();
		font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		dmd_show_low ();
		do {
			task_sleep (TIME_166MS);
		} while (!score_update_required ());
	}
}

void attack_score_deff (void) { deff_exit (); }

void jet_deff (void) { deff_exit (); }
void jet_level_up_deff (void) { deff_exit (); }
