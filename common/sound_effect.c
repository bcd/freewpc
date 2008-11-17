/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (sound_effect, __effect__) */

U8 speech_prio;

U8 music_prio;

sound_code_t music_requested;

sound_code_t music_active;


/**
 * Called by a music_refresh handler to say that
 * a certain track is eligible to be played at a given
 * priority.
 */
void music_request (sound_code_t music, U8 prio)
{
	if (prio > music_prio)
		music_requested = music;
}


/**
 * Handles music refresh in the background.
 */
void music_refresh_task (void)
{
	music_prio = 0;
	music_requested = 0;
	callset_invoke (music_refresh);
	if (music_requested != music_active)
		sound_send (music_requested);
	music_active = music_requested;
	task_exit ();
}


/**
 * Invoked periodically to update the background music.
 */
void music_refresh (void)
{
	if (in_live_game)
	{
		task_create_gid1 (GID_MUSIC_REFRESH, music_refresh_task);
	}
	else
	{
		music_off ();
	}
}


/**
 * Invoke a sound board command for speech.
 * DURATION says how long the speech will take to complete.
 * PRIORITY controls whether or not the call will be made,
 * if another speech call is in progress.
 */
void speak (sound_code_t code,
				task_ticks_t duration,
				U8 prio)
{
	if (prio > speech_prio)
	{
	}
}


/**
 * Make a sound call then refresh the background music
 * when it has completed.
 */
void sound_play_with_refresh (sound_code_t code,
										task_ticks_t duration)
{
	sound_send (code);
	task_sleep (duration);
	music_refresh ();
}


CALLSET_ENTRY (sound_effect, init)
{
	speech_prio = 0;
	music_refresh ();
}
