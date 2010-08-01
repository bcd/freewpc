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

U8 centreramps;

void centre_ramp_deff (void)
{
	seg_alloc_clean ();
	seg_write_row_center (0, "HEARTBEAT");
	psprintf ("1 RAMP", "%d RAMPS", centreramps);
	seg_write_row_center (1, sprintf_buffer);
	seg_show ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (centreramp, sw_centre_ramp_enter)
{
	if (event_did_follow (centre_ramp_enter, centre_ramp_fail))
	{
		sound_send (SND_CENTRE_RAMP_FAIL);
	}
	else
	{
		sound_send (SND_CENTRE_RAMP_ENTER);
	}
	
	event_can_follow (centre_ramp_enter, centre_ramp_made, TIME_3S);
	event_can_follow (centre_ramp_enter, centre_ramp_fail, TIME_2S);
}

CALLSET_ENTRY (centreramp, sw_centre_ramp_made)
{
	if (event_did_follow (centre_ramp_enter, centre_ramp_made))
	{
		sound_send (SND_CENTRE_RAMP_1);
		bounded_increment (centreramps, 254);
		deff_start (DEFF_CENTRE_RAMP);
	}
	
}

CALLSET_ENTRY (centreramp, start_ball)
{
	centreramps = 0;
}
