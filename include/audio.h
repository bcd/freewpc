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

#ifndef __AUDIO_H
#define __AUDIO_H


/** The total number of audio channels supported.  At most 8 can
 * be supported due to the way channel bitmasks are used. */
#define NUM_AUDIO_CHANNELS 8


/** A background track */
typedef struct {
	/** The priority of the track.  Only the highest priority track queued
	 * will be audible */
	priority_t prio;

	/** The sound board code for the track */
	U8 code;
} audio_track_t;


/* New style */

#define MAX_TRACKS 8
__common__ void music_start (const audio_track_t track);
__common__ void music_stop (const audio_track_t track);
__common__ void music_stop_all (void);

/* Really new sound system */

#define MAX_SOUND_CHANNELS 4

#define MUSIC_CHANNEL 0

#define ST_MUSIC   0x1
#define ST_SPEECH  0x2
#define ST_SAMPLE  0x4
#define ST_EFFECT  0x8
#define ST_ANY     (ST_SAMPLE | ST_EFFECT)

#define SP_NORMAL  PRI_NULL

#define SL_100MS   1
#define SL_500MS   5
#define SL_1S      10
#define SL_2S      20
#define SL_3S      30
#define SL_4S      40

typedef struct
{
	/** The time, in 100ms units, until this channel becomes
	 * free. */
	U8 timer;

	/** The current priority of the sound that is using
	 * this channel right now. */
	U8 prio;
} sound_channel_t;

__effect__ void music_refresh (void);
__effect__ void music_request (sound_code_t music, U8 prio);
__effect__ void sound_start1 (U8 channels, sound_code_t code);

extern inline void sound_start (U8 channels, sound_code_t code, U8 duration, U8 prio)
{
	extern U8 sound_start_duration;
	extern U8 sound_start_prio;

	sound_start_duration = duration;
	sound_start_prio = prio;
	sound_start1 (channels, code);
}

extern inline void speech_start (sound_code_t code, U8 duration)
{
	sound_start (ST_SPEECH, code, duration, SP_NORMAL);
}

extern inline void music_effect_start (sound_code_t code, U8 duration)
{
	sound_start (ST_MUSIC, code, duration, SP_NORMAL);
}

#endif /* __AUDIO_H */
