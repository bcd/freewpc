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

#ifndef _SYS_SOUND_H
#define _SYS_SOUND_H

/* Predefined, fixed system sound values */

#if (MACHINE_DCS == 0)

typedef U8 sound_cmd_t;

#define SND_TEST_DOWN			0x50
#define SND_TEST_UP				0x51
#define SND_TEST_ABORT			0x52
#define SND_TEST_CONFIRM		0x53
#define SND_TEST_ALERT			0x54
#define SND_TEST_HSRESET		0x55
#define SND_TEST_CHANGE			0x56
#define SND_TEST_ENTER			0x57
#define SND_TEST_ESCAPE			0x58
#define SND_TEST_SCROLL			0x59

/* The following sound codes don't actually play sounds, but
 * rather issue commands to the sound board. */

/** Drops the volume of the D/A converter */
#define SND_DROP_DAC_VOLUME(x)		(0x20 + (x))

/** Causes the running music to go faster */
#define SND_MUSIC_FASTER		0x5A

/** Causes the running music to go slower */
#define SND_MUSIC_SLOWER		0x5B

/** Requests the version number of the sound code */
#define SND_GET_VERSION_CMD	0x5F

/** Drops the volume of the running music */
#define SND_DROP_DSP_VOLUME(x)		(0x60 + (x))

/** Enables an event notification from the sound board
 * when the next sound call finishes. */
#define SND_EVENT_ENABLE		0x76

/** Requests volume change.  The next 2 bytes written
 * will be interpreted as a volume level and a checksum. */
#define SND_SET_VOLUME_CMD    0x79

/** Says that an extended sound call is being made.  Normally,
 * a single byte can be used to make a sound call, but that
 * limits to 256 (theoretically).  After the EXTENDED code,
 * the next byte specifies one of a second set of 256 calls. */
#define SND_START_EXTENDED		0x7A

#define SND_GET_VERSION_CMD2	0x7B

/** Disables event notifications */
#define SND_EVENT_DISABLE		0x7C

/** Requests all sounds to stop playing */
#define SND_STOP_SOUND			0x7D

/** Requests that music stop */
#define SND_STOP_MUSIC			0x7E 

/** Requests that the DAC stop */
#define SND_STOP_DAC          0x7F

#else /* MACHINE_DCS == 1 */

typedef U16 sound_cmd_t;

#define MUSIC_SND(x)				((0UL << 8) + (x))
#define SPEECH_SND(x)			((1UL << 8) + (x))
#define TEST_SND(x)				((3UL << 8) + (x))

#define SND_TEST_DOWN			TEST_SND(0xD4)
#define SND_TEST_UP				TEST_SND(0xD5)
#define SND_TEST_ABORT			TEST_SND(0xD6)
#define SND_TEST_CONFIRM		TEST_SND(0xD7)
#define SND_TEST_ALERT			TEST_SND(0xD8)
#define SND_TEST_HSRESET		TEST_SND(0xD9)
#define SND_TEST_CHANGE			TEST_SND(0xDA)
#define SND_TEST_ENTER			TEST_SND(0xDB)
#define SND_TEST_ESCAPE			TEST_SND(0xDC)
#define SND_TEST_SCROLL			TEST_SND(0xDD)
#define SND_INIT_CH0          TEST_SND(0xE3)
#define SND_INIT_CH1          TEST_SND(0xE4)
#define SND_INIT_CH2          TEST_SND(0xE5)
#define SND_INIT_CH3          TEST_SND(0xE6)
#define SND_GET_VERSION_CMD	TEST_SND(0xE7)
#define SND_GET_MINOR_VERSION_CMD TEST_SND(0xE8)

#endif /* MACHINE_DCS */


#define MUS_OFF					0

#define MIN_VOLUME				0
#define MAX_VOLUME				31
#define DEFAULT_VOLUME			8

/** Returned by the sound board when the last sound clip finishes. */
#define SND_FINISHED				0x80

typedef U16 music_code_t, sound_code_t;

void music_off (void);
void music_set (music_code_t code);
void sound_rtt (void);
void sound_init (void);
void sound_board_init (void);
void sound_send (sound_code_t code);
void sound_reset (void);
void volume_set (U8);
bool sound_version_render (void);

#endif /* _SYS_SOUND_H */
