/*
 * Copyright 2010 by Ewan Meadows <sonny_jim@hotmail.com>
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


/* CALLSET_SECTION (outhole, __machine2__) */

#include <freewpc.h>
/* How many balls have drained in three seconds */
U8 multidrain_count;
bool multidrain_awarded;

CALLSET_ENTRY (outhole, sw_outhole)
{	
	if (in_live_game && !timer_find_gid(GID_OUTHOLE_DEBOUNCE))
	{
		timer_restart_free (GID_OUTHOLE_DEBOUNCE, TIME_1S);
		/* Timer to check if 3 balls drain quickly */
		if (!timer_find_gid (GID_MULTIDRAIN) && multi_ball_play () && !ballsave_test_active ())
		{
			multidrain_count = 0;
			timer_restart_free (GID_MULTIDRAIN, TIME_10S);
		}
	
		if (timer_find_gid (GID_MULTIDRAIN))
		{
			/* There are 6 balls installed normally */
			bounded_increment (multidrain_count, 6);
			if (multidrain_count == 3)
				multidrain_awarded = TRUE;
		}
		deff_start (DEFF_BALL_EXPLODE);
	}
	
}

CALLSET_ENTRY (outhole, ball_start)
{
	multidrain_count = 0;
	multidrain_awarded = FALSE;
	timer_kill_gid (GID_MULTIDRAIN);
}
