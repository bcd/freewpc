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

/**
 * \file
 * \brief Common audio routines.
 * This is a layer of abstraction above the raw kernel functions.
 * It works similar to the display effect functions. */

/** The background track stack.
 * Each element of this array represents a background track
 * that has been enabled for play on the background music channel.
 * Only the highest priority track is allowed to run at any time.
 */
const audio_track_t *audio_bg_track_table[NUM_STACKED_TRACKS];

/** The audio channel control table.
 * For each channel, there is a separate entry that describes the
 * current state of the channel. */
audio_channel_t audio_channel_table[NUM_AUDIO_CHANNELS];

#ifdef DEBUGGER
static const char *audio_channel_names[] = {
	"Sample 1", "Sample 2", "Sample 3",
	"BG", "Speech 1", "Speech 2", "FG", "Reserved",
};
#endif

/** Dump the audio data to the debugger port. */
void audio_dump (void)
{
#ifdef DEBUGGER
	U8 i;

	/* Dump the channel table */
	db_puts ("\n");
	for (i=0; i < NUM_AUDIO_CHANNELS; i++)
	{
		dbprintf ("CH %d (%s) : %p\n",
			i, audio_channel_names[i], audio_channel_table[i].pid);
	}

	/* Dump the track table */
	for (i=0; i < NUM_STACKED_TRACKS; i++)
	{
		const audio_track_t *track = audio_bg_track_table[i];
		if (track)
		{
			dbprintf ("Track %d : %02X  P%d\n", i, track->code, track->prio);
		}
	}
#endif /* DEBUGGER */
}


/**
 * Start an audio clip.
 * channel_mask identifies the possible channels that may be used.
 * fn is the function that plays the clip.
 * fnpage is the ROM bank where the function resides.
 * data is task-specific data that can be initialized.
 */
void audio_start (
	U8 channel_mask,
	task_function_t fn,
	U8 fnpage,
	U16 data)
{
	U8 channel_id;
	audio_channel_t *ch;

	for (channel_id = 0; channel_id < NUM_AUDIO_CHANNELS; channel_id++)
	{
		/* Don't try to use this channel if it is not
		specified in the channel mask. */
		if ((channel_mask & (1 << channel_id)) == 0)
			continue;

		/* Don't use this channel if the channel is
		currently allocated.  TODO: look at priority here */
		ch = &audio_channel_table[channel_id];
		if (ch->pid != NULL)
			continue;

		/* OK, use this channel. */
		ch->pid = task_create_gid_while (GID_AUDIO_TASK, fn, TASK_DURATION_INF);
#ifdef __m6809__
		ch->pid->rom_page = fnpage;
#endif
		task_set_arg (ch->pid, data);
		return;
	}

	/* No channel found for this data! */
}


/** Stop the audio on a particular channel. */
void audio_stop (U8 channel_id)
{
	audio_channel_t *ch = &audio_channel_table[channel_id];
	if (ch->pid != NULL)
	{
		task_kill_pid (ch->pid);
		ch->pid = NULL;
	}
}


/** Exit an audio task.  This resets the channel, too. */
void audio_exit (void)
{
	U8 channel_id;

	for (channel_id = 0; channel_id < NUM_AUDIO_CHANNELS; channel_id++)
	{
		audio_channel_t *ch = &audio_channel_table[channel_id];
		if (ch->pid == task_getpid ())
		{
			ch->pid = NULL;
			break;
		}
	}
	task_exit ();
}


/** Initialize the audio subsystem. */
CALLSET_ENTRY (audio, init)
{
	memset (audio_channel_table, 0, sizeof (audio_channel_table));
	memset (audio_bg_track_table, 0, sizeof (audio_bg_track_table));
}


/** The predefined system track for the default background music */
static const audio_track_t default_music_track = {
	.prio = PRI_SCORES,
#ifdef MACHINE_BALL_IN_PLAY_MUSIC
	.code = MACHINE_BALL_IN_PLAY_MUSIC,
#else
	.code = 2,
#endif
};


/** The predefined system track for the start ball music */
static const audio_track_t start_ball_music_track = {
	.prio = PRI_NULL,
#ifdef MACHINE_START_BALL_MUSIC
	.code = MACHINE_START_BALL_MUSIC,
#else
	.code = 3,
#endif
};



CALLSET_ENTRY(audio, start_ball)
{
	music_start (start_ball_music_track);
}


CALLSET_ENTRY (audio, valid_playfield)
{
	/* TODO : optimize to a single call */
	music_stop (start_ball_music_track);
	music_start (default_music_track);
}


CALLSET_ENTRY (audio, bonus)
{
	music_stop_all ();
}


CALLSET_ENTRY (audio, end_ball)
{
	music_stop_all ();
}


CALLSET_ENTRY (audio, end_game)
{
	music_stop_all ();
	if (!in_test)
	{
		/* TODO - start timed with fade out */
#ifdef MACHINE_END_GAME_MUSIC
		music_set (MACHINE_END_GAME_MUSIC);
#endif
	}
}


CALLSET_ENTRY(audio, start_game)
{
#ifdef MACHINE_START_GAME_SOUND
	sound_send (MACHINE_START_GAME_SOUND);
#endif
}

