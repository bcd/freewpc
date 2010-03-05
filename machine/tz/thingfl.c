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


/* CALLSET_SECTION (thingfl, __machine2__) */
#include <freewpc.h>

__local__ U8 thing_flips_enable_count;
U8 thing_flips_successes;
U8 thing_flips_attempts;
U8 delay;
extern U8 loop_time;
extern void flipper_override_pulse (U8 switches);

void thing_flips_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf ("%d DELAY", delay);
	font_render_string_left (&font_mono5, 48, 10, sprintf_buffer);
	sprintf("%d SPEED", loop_time);
	font_render_string_center (&font_mono5, 48, 18, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (3);
	deff_exit ();
}

CALLSET_ENTRY (thingfl, thing_flips)
{
	if (multi_ball_play ())
		return;
	timer_restart_free (GID_THING_FLIPS_SUCCESS, TIME_2S);
	//4, 31 too quick
	if (loop_time < 27)
		delay = 4;
	else if (loop_time < 35)
		delay = 5;
	else if (loop_time < 50)
		delay = 6;
	else if (loop_time < 60)
		delay = 7;
	else
		delay = TIME_100MS;
	
	//if (in_test)
//		delay = thingfl_test_delay;
	/*delay = loop_speed / 100 ;
	delay += TIME_33MS;*/
	//if (flag_test(FLAG_PB_ALONE_IN_PLAY))
		//delay = delay - TIME_33MS;
	//delay = (TIME_33MS + loop_speed);	
	//TODO Check for powerball and adjust accordingly
	//TODO loop.c already checks for a right loop and thing_flips_enable_count, should it be here?
	task_sleep (delay);//Use a magic number here?
	flipper_override_pulse (WPC_UL_FLIP_SW);
	if (in_live_game)
	//	deff_start (DEFF_THING_FLIPS);
	thing_flips_attempts++;
}

CALLSET_ENTRY (thingfl, sw_piano)
{
	
	if (timer_kill_gid (GID_THING_FLIPS_SUCCESS))
	{	
		thing_flips_successes++;
		sound_send (SND_ADDAMS_FASTLOCK_STARTED);
		score (SC_10M);
	}
	
}
//TODO Maybe apologise for missing the shot?
CALLSET_ENTRY (thingfl, sw_standup_4)
{
	if (timer_kill_gid (GID_THING_FLIPS))
	{	
		sound_send (SND_KACHING);
	}
	
}

CALLSET_ENTRY (thingfl, sw_power_payoff)
{
	if (timer_kill_gid (GID_THING_FLIPS))
	{	
		sound_send (SND_KACHING);
	}
	
}

CALLSET_ENTRY (thingfl, start_player)
{
	thing_flips_enable_count = 0;
}

