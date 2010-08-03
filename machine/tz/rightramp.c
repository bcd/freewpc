/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
#include <bridge_open.h>

extern __machine__ bool mpf_ready_p (void);
extern void award_unlit_shot (U8 unlit_called_from);

U8 right_ramps_entered;
/* Used to remind the player where to shoot to advance
 * after they shoot it 3 times unsucessfully */
U8 unlit_right_ramps;

void shoot_hitch_deff (void)
{
	dmd_alloc_low_clean ();
	dmd_sched_transition (&trans_scroll_right);	
	font_render_string_center (&font_mono5, 64, 6, "SHOOT HITCHHIKER");
	font_render_string_center (&font_mono5, 64, 22, "TO UNLOCK POWER");
	dmd_show_low ();
	task_sleep_sec (1);
	deff_exit ();
}


void sw_right_ramp_enter_task (void)
{
	/* Decide whether to let the ball onto the mini-playfield,
	or dump it.  Do this once for each balls that enters the
	ramp. */
	do {
		/* Let it through to the mpf field if allowed */
		if (mpf_ready_p ())
		{
			unlit_right_ramps = 4;
			bridge_open_start ();
			task_sleep_sec (3);
			bridge_open_stop ();
		}
		else /* Show an animation hint if not enabled for mpf
			and drop ball */
		{
			bounded_increment (unlit_right_ramps, 4);
			if (unlit_right_ramps == 3 && !flag_test (FLAG_MULTIBALL_RUNNING))
				deff_start (DEFF_SHOOT_HITCH);
			else if (flag_test (FLAG_MULTIBALL_RUNNING)
					&& flag_test (FLAG_MB_JACKPOT_LIT))
			{
				deff_start (DEFF_SHOOT_JACKPOT);
				sound_send (SND_TWILIGHT_ZONE_SHORT_SOUND);
			}
			task_sleep_sec (2);
	
			/* Wait until allowed to kickout */
			while (kickout_locks > 0)
				task_sleep (TIME_100MS);
			sound_send (SND_RIGHT_RAMP_EXIT);
			bridge_open_start ();
			task_sleep (TIME_500MS);
			bridge_open_stop ();
		}
	} while (--right_ramps_entered > 0);
	task_exit ();
}


CALLSET_ENTRY (right_ramp, sw_right_ramp)
{
	/* Handle all balls entering the ramp unconditionally, so that
	they are disposed of properly. */
	right_ramps_entered++;
	task_recreate_gid_while (GID_RIGHT_RAMP_ENTERED,
		sw_right_ramp_enter_task, TASK_DURATION_INF);

	/* Scoring functions only happen during a game */
	if (!in_live_game)
		return;
	
	score (SC_10K);
	if (mpf_ready_p ())
		sound_send (SND_RAMP_ENTERS_POWERFIELD);
	else
	{	
		sound_send (SND_RIGHT_RAMP_DEFAULT_ENTER);
		award_unlit_shot (SW_RIGHT_RAMP);
	}
}



CALLSET_ENTRY (right_ramp, start_ball)
{
	right_ramps_entered = 0;
	unlit_right_ramps = 0;
}

/* The default driver doesn't seem to be working */
CALLSET_ENTRY (right_ramp, ball_search)
{
	/* Wait a bit in case a ball has been launched */
	task_sleep_sec (1);
	bridge_open_start ();
	task_sleep (TIME_500MS);
	bridge_open_stop ();
}
