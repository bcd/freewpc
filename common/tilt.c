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

/**
 * \file
 * \brief Common tilt functions
 *
 * This module handles all of the tilt switches, start game-specific
 * effects when they occur, and updates the core state machines
 * appropriately (in the case of a slam tilt, this will abort the
 * game).  It also provides the display and lamp effects that go along
 * with them.
 */

#include <freewpc.h>

/** The number of tilt warnings that have been issued on this ball. */
U8 tilt_warnings;

void tilt_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_cu17, 64, 13, "TILT");
	dmd_show_low ();
	leff_start (LEFF_TILT);

	/* Run the tilt deff for a minimum amount of time */
	task_sleep_sec (5);

	/* Now wait for the tilt condition to clear */
	while (in_tilt)
		task_sleep_sec (1);

	/* Cleanup and exit */
	leff_stop (LEFF_TILT);
	deff_exit ();
}


void tilt_warning_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_alloc_high_clean ();
	font_render_string_center (&font_mono5, 64, 13, 
			!(tilt_warnings % 1) ? "DANGER" : "DANGER  DANGER");
	dmd_show_low ();
	deff_swap_low_high (32, TIME_66MS);
	deff_exit ();
}


void slam_tilt_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed10, 64, 13, "SLAM TILT");
	dmd_show_low ();
	task_sleep_sec (5);
	deff_exit ();
}


CALLSET_ENTRY (tilt, sw_tilt)
{
	extern bool in_tilt;

	/* Ignore tilt switch activity while already in tilt state */
	if (in_tilt)
		return;

	else if (++tilt_warnings == system_config.tilt_warnings)
	{
		/* Warnings exceeded... tilt the current ball */
		sound_reset ();
		triac_disable (TRIAC_GI_MASK);
		deff_start (DEFF_TILT);
		in_tilt = TRUE;
		flipper_disable ();
		mark_ball_in_play ();
		audit_increment (&system_audits.tilts);
		audit_increment (&system_audits.plumb_bob_tilts);
		callset_invoke (tilt);
	}
	else
	{
		/* Give a warning this time */
		deff_start (DEFF_TILT_WARNING);
		callset_invoke (tilt_warning);
	}
}


CALLSET_ENTRY (tilt, sw_slam_tilt)
{
	/* Ignore right after a coin door open/close */
	if (event_did_follow (sw_coin_door_closed, sw_slam_tilt))
		return;

	/* Disable coins briefly, the whole point of the slam tilt */
	event_can_follow (sw_slam_tilt, sw_coin, TIME_5S);

	deff_start (DEFF_SLAM_TILT);
	audit_increment (&system_audits.tilts);
	audit_increment (&system_audits.slam_tilts);
	if (price_config.slamtilt_penalty)
		remove_credit ();
	/* TODO : abort current game */
}


CALLSET_ENTRY (tilt, start_ball)
{
	tilt_warnings = 0;
}

