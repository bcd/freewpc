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

/* Library calls */

void message_end (void)
{
	dmd_show_low ();
	task_sleep (TIME_2S+TIME_166MS);
	deff_exit ();
}

void message1 (const font_t *f, const char *line)
{
	dmd_alloc_low_clean ();
	font_render_string_center (f, 64, 16, line);
	message_end ();
}

void message2 (const font_t *f, const char *line1, const char *line2)
{
	dmd_alloc_low_clean ();
	font_render_string_center (f, 64, 10, line1);
	font_render_string_center (f, 64, 22, line2);
	message_end ();
}

/* AFM display effects */

const char *shot_names[] = {
	"LEFT LOOP", "LEFT RAMP", "CENTER RAMP",
	"RIGHT RAMP", "RIGHT LOOP"
};

const char *jackpot_levels[] = {
	"SUPER", "DOUBLE SUPER", "TRIPLE SUPER", "4X SUPER"
};

const char *luck_awards[] = {
	"BIG POINTS",
	"LIGHT MARTIAN ATTACK",
	"JACKPOT",
	"ADD-A-BALL",
	"ADD TIME",
	"START 2X SCORING",
	"ADD TO JACKPOT",
	"SPOT MARTIAN",
};

void jackpot_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 10, "JACKPOT");
	sprintf_score (last_score);
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_2S+TIME_166MS);
	deff_exit ();
}

void pf_mult_advance_deff (void)
{
	message1 (&font_fixed6, "ADV. MULTIPLIER");
}

void pf_mult_shot_deff (void)
{
	message1 (&font_fixed10, "ADVANCE SHOT");
}

void martian_spell (bool on)
{
	const char text[] = "MARTIAN";
	const char *s;
	U8 x = 16;
	U8 i;
	char c[2];

	s = text;
	i = 0;
	while (*s)
	{
		bool b = lamp_test (lamplist_index (LAMPLIST_MARTIANS, i));
		if (!on)
			b = !b;
		if (b)
		{
			c[0] = *s;
			c[1] = '\0';
			font_render_string_center (&font_fixed10, x, 11, c);
		}
		x += 16;
		s++;
		i++;
	}
}

void martian_advance_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	martian_spell (TRUE);
	font_render_string_center (&font_var5, 64, 25, "COMPLETE FOR EXTRA TIME");
	dmd_copy_low_to_high ();
	martian_spell (FALSE);
	dmd_show2 ();
	task_sleep (TIME_2S);
	deff_exit ();
}

void martian_spelled_deff (void)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	martian_spell (TRUE);
	font_render_string_center (&font_var5, 64, 25, "EXTRA TIME ADDED");
	dmd_copy_low_to_high ();
	dmd_show2 ();
	task_sleep (TIME_2S+TIME_500MS);
	deff_exit ();
}

void luck_lit_deff (void)
{
	message2 (&font_fixed6, "STROKE OF LUCK", "IS LIT");
}

void luck_award_deff (void)
{
	const char *award = luck_awards[0];
	message2 (&font_fixed6, "STROKE OF LUCK", award);
}

void mb_running_deff (void)
{
	extern U8 mb_targets_left;

target_change:
	dmd_map_overlay ();
	frame_draw (IMG_UFO);
	font_render_string_center (&font_mono5, 92, 5, "MULTIBALL");
	dmd_rough_copy (64, 2, 64, 6);
	if (mb_targets_left == 0)
		sprintf ("SUPER JACKPOT LIT");
	else if (mb_targets_left == 1)
		sprintf ("%d TARGET LEFT", mb_targets_left);
	else
		sprintf ("%d TARGETS LEFT", mb_targets_left);
	font_render_string_center (&font_var5, 88, 27, sprintf_buffer);

	U8 t = mb_targets_left;
score_change:
	for (;;)
	{
		dmd_map_overlay ();
		sprintf_current_score ();
		dmd_rough_erase (64, 11, 64, 10);
		font_render_string_center (&font_mono9, 92, 16, sprintf_buffer);
		dmd_rough_copy (64, 11, 64, 10);

		for (;;)
		{
			score_update_start ();
			dmd_map_overlay ();
			dmd_dup_mapped ();
			dmd_show2 ();
			task_sleep (TIME_200MS);
			if (score_update_required ())
				goto score_change;

			dmd_map_overlay ();
			dmd_dup_mapped ();
			dmd_rough_erase (64, 11, 64, 10);
			dmd_flip_low_high ();
			dmd_rough_erase (64, 11, 64, 10);
			dmd_flip_low_high ();
			dmd_show2 ();
			task_sleep (TIME_200MS);
			if (score_update_required ())
				goto score_change;
		}
	}
}

void mb_super_award_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 10, "SUPER JACKPOT");
	sprintf_score (last_score);
	font_render_string_center (&font_fixed6, 64, 22, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_3S);
	deff_exit ();
}

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
	message2 (&font_fixed6, "MARTIAN ATTACK", "IS LIT");
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
