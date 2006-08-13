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


__local__ U8 mpf_enable_count;


void mpf_battle_lamp_update (void)
{
	if (mpf_enable_count > 0)
		lamp_tristate_on (LM_RAMP_BATTLE);
	else
		lamp_tristate_off (LM_RAMP_BATTLE);
}

void mpf_enable (void)
{
	mpf_enable_count++;
	mpf_battle_lamp_update ();
}

void mpf_battle_running (void)
{
	task_exit ();
}

void mpf_start (void)
{
	task_create_gid1 (GID_BATTLE_RUNNING, mpf_battle_running);
}

void mpf_stop (void)
{
	task_kill_gid (GID_BATTLE_RUNNING);
}

CALLSET_ENTRY (mpf, sw_mpf_top)
{
	event_can_follow (mpf_top, camera, TIME_4S);
}

CALLSET_ENTRY (mpf, sw_mpf_enter)
{
	/* signal ramp divertor that ball passed */
	mpf_start ();
}

CALLSET_ENTRY (mpf, sw_mpf_exit)
{
	mpf_stop ();
}

CALLSET_ENTRY (mpf, sw_mpf_left)
{
}

CALLSET_ENTRY (mpf, sw_mpf_right)
{
}

CALLSET_ENTRY (mpf, sw_right_ramp)
{
}

CALLSET_ENTRY (mpf, start_player)
{
	mpf_enable_count = 0;
}

CALLSET_ENTRY (mpf, mpf_top_exit)
{
}

DECLARE_SWITCH_DRIVER (sw_mpf_top)
{
	DECLARE_SWITCH_EVENT (sw_mpf_top),
	.flags = SW_PLAYFIELD | SW_IN_GAME
};

DECLARE_SWITCH_DRIVER (sw_mpf_exit)
{
	DECLARE_SWITCH_EVENT (sw_mpf_exit),
	.flags = SW_PLAYFIELD | SW_IN_GAME
};

DECLARE_SWITCH_DRIVER (sw_mpf_enter)
{
	DECLARE_SWITCH_EVENT (sw_mpf_enter),
	.flags = SW_PLAYFIELD | SW_IN_GAME
};

DECLARE_SWITCH_DRIVER (sw_mpf_left)
{
	DECLARE_SWITCH_EVENT (sw_mpf_left),
	.flags = SW_PLAYFIELD | SW_IN_GAME
};

DECLARE_SWITCH_DRIVER (sw_mpf_right)
{
	DECLARE_SWITCH_EVENT (sw_mpf_right),
	.flags = SW_PLAYFIELD | SW_IN_GAME
};

