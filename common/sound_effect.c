/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/** Tracks the status of each audio channel */
sound_channel_t chtab[MAX_SOUND_CHANNELS];

/** The highest priority music requested during the current refresh */
U8 music_prio;

/** The best music code requested so far during the current refresh */
sound_code_t music_requested;

/** The music that is currently playing, or zero if none */
sound_code_t music_active;


/**
 * Called by a music_refresh handler to say that
 * a certain track is eligible to be played at a given
 * priority.
 */
void music_request (sound_code_t music, U8 prio)
{
	if (prio > music_prio)
	{
		dbprintf ("New music 0x%04lX at prio %d\n", music, prio);
		music_requested = music;
	}
}


/**
 * Handles music refresh in the background.
 */
void music_refresh_task (void)
{
	dbprintf ("Refreshing music\n");

	music_prio = 0;
	music_requested = 0;

	if (!in_test)
		callset_invoke (music_refresh);

	if (music_requested != music_active)
	{
		if (music_requested != 0)
		{
			dbprintf ("Music now %04lX\n", music_requested);
			sound_write (music_requested);
		}
		else
		{
			dbprintf ("Music now off\n");
			music_off (); /* TODO - careful here, may have a sound running */
		}
		music_active = music_requested;
	}
	task_exit ();
}


/**
 * Invoked periodically to update the background music.
 */
void music_refresh (void)
{
	task_recreate_gid (GID_MUSIC_REFRESH, music_refresh_task);
}


/**
 * Invoked periodically to see if any running sound effects
 * have stopped.
 */
CALLSET_ENTRY (sound_effect, idle_every_100ms)
{
	U8 chid;

	for (chid = 0; chid < MAX_SOUND_CHANNELS; chid++)
	{
		sound_channel_t *ch = ch = chtab + chid;
		if (ch->timer)
		{
			if (--ch->timer == 0)
			{
				dbprintf ("Sound on channel %d done.\n", chid);
				ch->prio = 0;
				if (chid == MUSIC_CHANNEL)
				{
					music_refresh ();
				}
				/* TODO - when supporting sound strings, this would cue
				 * the next sample */
			}
		}
	}
}


/**
 * Start a sound effect.
 * CHANNELS is one or more channels that it may be allocated to,
 *    provided as a bitmask.
 *
 * CODE says which sound to play.
 *
 * Two additional arguments, DURATION and PRIORITY, are
 * global variables rather than actual parameters, to workaround
 * 6809 compiler limitations.  Use the macros rather than this
 * function directly to make sure these are set OK.
 *
 * DURATION says how long the sound will take to complete.
 *    If zero, the sound could be preempted at any time and is
 *    not tracked.  Otherwise, the channel used for the
 *    sound cannot be used by any other sound effects in the
 *    meantime.
 *
 * PRIORITY controls whether or not the call will be made,
 * if another sound call is in progress.  When a higher
 * priority sound call is made, it can stop other calls,
 * if no channels are free.
 */

U8 sound_start_duration;
U8 sound_start_prio;

void sound_start1 (U8 channels, sound_code_t code)
{
	U8 chid;
	U8 chbit;

	for (chid = 0, chbit = 0x1; chid < MAX_SOUND_CHANNELS; chid++, chbit <<= 1)
	{
		sound_channel_t *ch;

		/* Skip this channel if it is not in the list that the caller
		 * suggested. */
		if (!(chbit & channels))
			continue;

		/* Is this channel free?   If not, can we take it due to priority? */
		ch = chtab + chid;
		if (ch->timer == 0 || sound_start_prio >= ch->prio)
		{
			/* Yes, we can use it */
			/* TODO - preemption should try to use a free channel
			first, before overriding */

			/* If a duration was given, then reserve the channel until
			 * it is done. */
			if (sound_start_duration != 0)
			{
				ch->timer = sound_start_duration;
				ch->prio = sound_start_prio;
			}

			/* If a sound call uses the music channel, this will
			kill the background music.  Note this so that the music
			can be restarted later. */
			if (chid == MUSIC_CHANNEL)
				music_active = 0;

			/* Cancel any sound running on the channel now */
			//sound_write (SND_INIT_CH0 + chid);

			/* Write to the sound board and return */
			dbprintf ("Start sound %04lX on channel %d\n", code, chid);
			sound_write (code);
			return;
		}
	}
}


/**
 * A default music refresh handler.
 * This function gets involved anytime a music refresh is required,
 * in addition to any machine-specific handlers.  It ensures that
 * the MACHINE_xxx_MUSIC defines for default music get started,
 * if there is nothing else to play.
 */
CALLSET_ENTRY (sound_effect, music_refresh)
{
	if (!in_live_game || in_bonus)
		return;

#ifdef MACHINE_BALL_IN_PLAY_MUSIC
	else if (valid_playfield)
		music_request (MACHINE_BALL_IN_PLAY_MUSIC, PRI_SCORES);
#endif

#ifdef MACHINE_START_BALL_MUSIC
	else
		music_request (MACHINE_START_BALL_MUSIC, PRI_SCORES);
#endif
}


/**
 * Always cause a music refresh at certain well-known points in the
 * game.
 */
CALLSET_ENTRY (sound_effect, bonus, end_ball)
{
	music_refresh ();
}


CALLSET_ENTRY (sound_effect, end_game)
{
	music_refresh ();
	if (!in_test)
	{
		/* TODO - start timed with fade out */
#ifdef MACHINE_END_GAME_MUSIC
		music_set (MACHINE_END_GAME_MUSIC);
#endif
	}
}


CALLSET_ENTRY (sound_effect, start_game)
{
#ifdef MACHINE_START_GAME_SOUND
	sound_send (MACHINE_START_GAME_SOUND);
#endif
}


CALLSET_ENTRY (sound_effect, init)
{
	memset (chtab, 0, sizeof (chtab));
	music_active = 0;
	music_refresh ();
}

