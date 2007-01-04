/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

/* 
 * \brief Common audio routines.
 * This is a layer of abstraction above the raw kernel functions.   
 * It works similar to the display effect functions. */

/** The background track stack.
 * Each element of this array represents a background track
 * that has been enabled for play on the background music channel.
 * Only the highest priority track is allowed to run at any time.
 */
audio_track_t *audio_bg_track_table[NUM_STACKED_TRACKS];

/** The audio channel control table.
 * For each channel, there is a separate entry that describes the
 * current state of the channel. */
audio_channel_t audio_channel_table[NUM_AUDIO_CHANNELS];


/** Dump the audio data to the debugger port. */
void audio_dump (void)
{
	U8 i;

	/* Dump the channel table */
	for (i=0; i < NUM_AUDIO_CHANNELS; i++)
	{
		dbprintf ("Ch%d : %p\n", audio_channel_table[i].pid);
	}

	/* Dump the track table */
	for (i=0; i < NUM_STACKED_TRACKS; i++)
	{
		audio_track_t *track = audio_bg_track_table[i];
		if (track)
		{
			dbprintf ("Track %d : %02X  %d\n", track->code, track->prio);
		}
	}
}


/**
 * Start an audio clip.
 * id identifies the channel that should be used.
 * fn is the function that plays the clip.
 * fnpage is the bank where the function resides.
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
		ch->pid = task_create_gid (GID_AUDIO_TASK, fn);
#ifdef __m6809__
		ch->pid->rom_page = fnpage;
		ch->pid->flags = TASK_PROTECTED;
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
		if (ch->pid == task_current)
		{
			ch->pid = NULL;
			break;
		}
	}
	task_exit ();
}


static void bg_music_task (void)
{
	U8 i;
	audio_track_t *current = NULL;

	task_set_flags (TASK_PROTECTED);

	/* Determine which of the stacked tracks has the highest priority. */
	for (i=0 ; i < NUM_STACKED_TRACKS; i++)
	{
		audio_track_t *track = audio_bg_track_table[i];
		if (track)
		{
			if (current == NULL)
				current = track;
			else if (track->prio > current->prio)
				current = track;
		}
	}

	if (current == NULL)
	{
		music_off ();
		audio_exit ();
	}

	/* Play the track, and reinitialize it every so often */
	for (;;)
	{
		music_set (current->code);
		task_sleep_sec (1);
	}
}


void bg_music_start (audio_track_t *track)
{
	U8 i;

	for (i=0; i < NUM_STACKED_TRACKS; i++)
		if (audio_bg_track_table[i] == NULL)
		{
			audio_bg_track_table[i] = track;
			audio_stop (AUDIO_CH_BACKGROUND);
			audio_start (AUDIO_BACKGROUND_MUSIC, bg_music_task, COMMON_PAGE, 0);
			return;
		}
}

void bg_music_stop (audio_track_t *track)
{
	U8 i;

	for (i=0; i < NUM_STACKED_TRACKS; i++)
		if (audio_bg_track_table[i] == track)
		{
			audio_bg_track_table[i] = NULL;
			audio_stop (AUDIO_CH_BACKGROUND);
			audio_start (AUDIO_BACKGROUND_MUSIC, bg_music_task, COMMON_PAGE, 0);
		}
}


/** Initialize the audio subsystem. */
CALLSET_ENTRY (audio, init)
{
	memset (audio_channel_table, 0, sizeof (audio_channel_table));
	memset (audio_bg_track_table, 0, sizeof (audio_bg_track_table));
}

