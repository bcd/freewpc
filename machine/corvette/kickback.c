/*
 * Copyright 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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
#include "kickback_driver.h"

void kickback_relit_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "KICKBACK IS LIT");
	dmd_show_low ();
	task_sleep (TIME_1500MS);
	deff_exit ();
}

void kickback_enable (void)
{
	lamp_on(LM_KICKBACK_ARROW);
	kickback_driver_enable();
}

void kickback_disable (void)
{
	lamp_off(LM_KICKBACK_ARROW);
	lamp_flash_on(LM_LITE_KICKBACK);
	lamp_flash_on(LM_RIGHT_STANDUP_ARROW);
	kickback_driver_disable();
}

static inline bool kickback_enabled (void)
{
	return lamp_test (LM_KICKBACK_ARROW);
}


void kickback_finish (void)
{
	task_sleep_sec (3);
	kickback_disable ();
	task_exit ();
}


CALLSET_ENTRY (kickback, sw_kickback)
{
	if (kickback_enabled ())
	{
		sound_start (ST_SAMPLE, SND_ENGINE_REV_03, SL_3S, PRI_GAME_QUICK3);
		task_recreate_gid (GID_KICKBACK_FINISH, kickback_finish);
	}
	else
	{
		callset_invoke (sw_left_out_lane);
	}
}

CALLSET_ENTRY (kickback, sw_right_standup)
{
	if (lamp_flash_test (LM_LITE_KICKBACK)) {
		lamp_flash_off(LM_LITE_KICKBACK);
		lamp_flash_off(LM_RIGHT_STANDUP_ARROW);
		sound_start (ST_SAMPLE, SND_BLASTER, SL_1S, PRI_GAME_QUICK3);
		kickback_enable ();
	} else {
		sound_start (ST_SAMPLE, SND_BOING, SL_1S, PRI_GAME_QUICK3);
	}
}

CALLSET_ENTRY (kickback, start_player)
{
	kickback_enable ();
}

CALLSET_ENTRY (kickback, start_ball)
{
	if (kickback_enabled ())
	{
		kickback_enable ();
	}
	else
	{
		kickback_disable ();
	}
}

