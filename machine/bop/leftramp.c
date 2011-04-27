/*
 * Copyright 2010 by Ewan Meadows (sonny_jim@hotmail.com)
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
#include <gate.h>

void shuttle_launch_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "SHUTTLE LAUNCH");
	seg_write_row_center (1, "WOOOSH");
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

void head_divert_to_mpf (void)
{
	gate_start ();	
	task_sleep_sec (4);
	gate_stop ();
}

CALLSET_ENTRY (leftramp, sw_left_ramp_enter)
{
	if (!event_did_follow (left_ramp, left_ramp_fail))
	{
		sound_send (SND_SHUTTLE_LAUNCH);
		deff_start (DEFF_SHUTTLE_LAUNCH);
		head_divert_to_mpf ();
	}
	else
	{
		sound_send (SND_ABORT_ABORT);
	}
	event_can_follow (left_ramp, left_ramp_fail, TIME_2S);
}
