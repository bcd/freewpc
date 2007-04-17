/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Implements the match function
 *
 */

U8 match_count;

bcd_t match_value;


void
match_award (void)
{
	audit_increment (&system_audits.match_credits);
	add_credit ();
	knocker_fire ();
}


void
match_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_right (&font_fixed6, 126, 2, "MATCH");
	sprintf ("%2b", match_value);
	font_render_string_right (&font_fixed6, 126, 22, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


/** Runs the match sequence if necessary */
void
match_start (void) 
{
	U8 match_flag;

	/* Nothing to do if match has been disabled */
	if (system_config.match_feature == 0)
		return;

	/* Randomly decide whether or not to award a match.
	 * TODO : This algorithm is too simple, it will not work
	 * for multi-player games. */
	match_flag = random_scaled (100);
	match_value = 0x50;
	match_count = 0;

	/* Start the match effect, then wait until it finishes. */
	deff_start (DEFF_MATCH);
	while (deff_get_active () == DEFF_MATCH)
		task_sleep (TIME_100MS);

	/* Award credits */
	while (match_count > 0)
	{
		match_count--;
		match_award ();
	}
}

