/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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
#include <search.h>

/* Fail if this machine does not support mute/pause mode. */
#if (MACHINE_FLIPTRONIC == 0)
#error "CONFIG_MUTE_AND_PAUSE unsupported because no computer-controlled flippers"
#endif

void mute_and_pause_monitor (void)
{
	flipper_hold_on ();
	lamp_on (LM_BUY_IN_BUTTON);
	kickout_lock (KLOCK_USER);
	music_disable ();
	for (;;)
	{
		ball_search_timer_reset ();
		/* TODO - this task should timeout after awhile */
		task_sleep_sec (5);
	}
	task_exit ();
}

void mute_and_pause_stop (void)
{
	task_kill_gid (GID_MUTE_AND_PAUSE);
	music_enable ();
	kickout_unlock (KLOCK_USER);
	lamp_off (LM_BUY_IN_BUTTON);
	flipper_hold_off ();
}


/**
 * Handle the push button that toggles the state of mute/pause if enabled.
 */
CALLSET_ENTRY (mute_and_pause, sw_buyin_button)
{
	if (feature_config.mute_pause == NO)
	{
		return;
	}
	else if (!in_live_game)
	{
		return;
	}
	else if (task_find_gid (GID_MUTE_AND_PAUSE))
	{
		/* Stop mute/pause mode */
		mute_and_pause_stop ();
		#ifdef MACHINE_TZ
			callset_invoke (tz_flipcode_entry_stop);
		#endif
	}
	else
	{
		/* Start mute/pause mode */
		task_create_gid (GID_MUTE_AND_PAUSE, mute_and_pause_monitor);
		#ifdef MACHINE_TZ
		if (!switch_poll (SW_LEFT_BUTTON) && !switch_poll (SW_RIGHT_BUTTON)
			&& feature_config.tz_flipcodes == YES
			&& system_config.tournament_mode != YES)
			callset_invoke (tz_flipcode_entry);
		#endif
	}
}


/*
 * Ensure that mute/pause is turned off at endball.
 */
CALLSET_ENTRY (mute_and_pause, end_ball, tilt)
{
	mute_and_pause_stop ();
}

