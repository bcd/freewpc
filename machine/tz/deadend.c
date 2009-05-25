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

__local__ U8 dead_end_count;


CALLSET_ENTRY (deadend, start_player)
{
	dead_end_count = 0;
}


CALLSET_ENTRY (deadend, sw_dead_end)
{
	device_switch_can_follow (dead_end, slot, TIME_8S);
	event_can_follow (dead_end, camera, TIME_4S);

	if (lamp_test (LM_DEAD_END))
	{
		dead_end_count++;
		sound_send (SND_DEAD_END_SCREECH);
		task_sleep_sec (1);
		sound_send (SND_DEAD_END_CRASH);
		switch (dead_end_count)
		{
			case 1:
				score (SC_250K);
				timed_game_extend (15);
				break;
			case 2:
				score (SC_500K);
				timed_game_extend (20);
				break;
			case 3:
			default:
				score (SC_1M);
				timed_game_extend (30);
				break;
		}
		lamp_off (LM_DEAD_END);
	}
	else
	{
		score (SC_100K);
		sound_send (SND_TOWN_SQUARE_AWARD);
		timed_game_pause (TIME_3S);
	}
}

