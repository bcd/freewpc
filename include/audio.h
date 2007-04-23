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

#ifndef __AUDIO_H
#define __AUDIO_H


/** A bitmask that says an audio clip should try to use a particular
channel. */
#define AUDIO_CH(n)	(1 << (n))

/* The 8 channels are allocated for different purposes.
 * The order of these bits is in order from most frequent to least. */
#define AUDIO_SAMPLE1           0x1
#define AUDIO_SAMPLE2           0x2
#define AUDIO_SAMPLE3           0x4
#define AUDIO_BACKGROUND_MUSIC  0x8
#define AUDIO_SPEECH1           0x10
#define AUDIO_SPEECH2           0x20
#define AUDIO_FOREGROUND_MUSIC  0x40
#define AUDIO_RESERVED          0x80

#define AUDIO_CH_SAMPLE1        0
#define AUDIO_CH_SAMPLE2        1
#define AUDIO_CH_SAMPLE3        2
#define AUDIO_CH_BACKGROUND     3
#define AUDIO_CH_SPEECH1        4
#define AUDIO_CH_SPEECH2        5
#define AUDIO_CH_FOREGROUND     6

/** The total number of audio channels supported.  At most 8 can
 * be supported due to the way channel bitmasks are used. */
#define NUM_AUDIO_CHANNELS 8


/** The total number of stacked background tracks */
#define NUM_STACKED_TRACKS 8

/** The audio channel structure. */
typedef struct {
	/** The task that currently owns the channel.
	 * If this is zero, the channel is free and may be allocated
	 * for a new clip. */
	task_pid_t pid;

	/** The priority of the clip that currently owns the channel. */
	priority_t prio;
} audio_channel_t;


/** A background track */
typedef struct {
	/** The priority of the track.  Only the highest priority track queued
	 * will be audible */
	priority_t prio;

	/** The sound board code for the track */
	U8 code;
} audio_track_t;

void audio_dump (void);
void audio_start (U8 channel_mask, task_function_t fn, U8 fnpage, U16 data);
void audio_stop (U8 channel_id);
void audio_exit (void);
void bg_music_start (const audio_track_t *track);
void bg_music_stop (const audio_track_t *track);
void bg_music_stop_all (void);

/* New style */


#define MAX_TRACKS 8
__common__ void music_start (const audio_track_t track);
__common__ void music_stop (const audio_track_t track);
__common__ void music_stop_all (void);

#endif /* __AUDIO_H */
