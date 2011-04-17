/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
#include <queue.h>

/**
 * \file
 * \brief Sound and music control
 * This file implements the interface to the sound board hardware, both the WPC
 * and DCS versions.
 *
 * Because the sound board runs asynchronously to the CPU board, all input/output
 * is buffered.  A fast running realtime function pulls data from a "write queue"
 * and transmits it to the sound board, at the required rate.  Another
 * realtime function reads from the sound board and places it into a "read queue",
 * where it can be processed by higher-layer logic at a slower pace (but hopefully
 * not so slow that the queue overflows).
 *
 * The WPC sound board uses 8-bit commands for most things; one of the command
 * values acts as an escape, though, and causes the next 8-bit value to be
 * interpreted (differently) instead.
 */


/** The length of the sound queue buffer.  These are bytes pending transmit from
 * the CPU board to the sound board. */
#define SOUND_QUEUE_LEN 8


/** The sound write queue.  head and tail are offsets; elems hold the actual
 * data bytes to be transmitted. */
__fastram__ struct {
	queue_t header;
	U8 elems[SOUND_QUEUE_LEN];
} sound_write_queue;

/** The sound read queue, which works just like the write queue but takes
back data from the sound board. */
__fastram__ struct {
	queue_t header;
	U8 elems[SOUND_QUEUE_LEN];
} sound_read_queue;

/** The last music code transmitted */
music_code_t current_music;

/** The current master volume.  Individual sound clips may override this
 * temporarily, but this is the default. */
__nvram__ U8 current_volume;
__nvram__ U8 current_volume_checksum;


/** -1 if the sound board booted OK, otherwise an error code returned from
 * the sound board indicating the problem */
U8 sound_error_code;

/** The major version of the sound board */
U8 sound_version_major;

#ifdef MACHINE_DCS
/** The minor version of the sound board, on DCS machines */
U8 sound_version_minor;
#endif

U8 sound_board_type;


/** The default audio track to be played when setting volume. */
#ifndef MACHINE_VOLUME_CHANGE_MUSIC
#define MACHINE_VOLUME_CHANGE_MUSIC 2
#endif

/** Renders the sound board version into the print buffer. */
bool sound_version_render (void)
{
	if (sound_error_code != 0xFF)
	{
		sprintf ("SOUND TYPE %02X ERR %02X",
			sound_board_type, sound_error_code);
		return FALSE;
	}
	else
	{
#if (MACHINE_DCS == 1)
		sprintf ("%d.%d", sound_version_major, sound_version_minor);
#else
		sprintf ("L-%d", sound_version_major);
#endif
		return TRUE;
	}
}


/** Initialize the sound write queue */
static void sound_write_queue_init (void)
{
	disable_irq ();
	queue_init (&sound_write_queue.header);
	enable_irq ();
}

/** Initialize the sound read queue */
static void sound_read_queue_init (void)
{
	disable_irq ();
	queue_init (&sound_read_queue.header);
	enable_irq ();
}


/** Queues a byte for transmit to the sound board */
static __attribute__((noinline)) void sound_write_queue_insert (U8 val)
{
	queue_insert (&sound_write_queue.header, SOUND_QUEUE_LEN, val);
}


/** Dequeues a byte for transmit to the sound board */
static U8 sound_write_queue_remove (void)
{
	return queue_remove (&sound_write_queue.header, SOUND_QUEUE_LEN);
}


/** Checks whether the write queue is empty or not */
inline bool sound_write_queue_empty_p (void)
{
	return queue_empty_p (&sound_write_queue.header);
}


void music_set (music_code_t code)
{
	/* Don't send the command again if the same music is already
	running. */
	if ((code != MUS_OFF) && (current_music == code))
		return;

	/* Update cache of currently running music */
	current_music = code;

	/* Write to the sound hardware.
	 * Music codes are not emitted if volume is set to zero, or
	 * if game music has been disabled.  But MUS_OFF is always
	 * sent to the sound board, regardless of everything else. */
	if (((current_volume > 0)
			&& (system_config.game_music == ON))
		|| (code == MUS_OFF))
	{
#if (MACHINE_DCS == 1)
		sound_write_queue_insert (0);
#endif
		sound_write_queue_insert (current_music);
	}
}


/** Stop whatever music is currently playing. */
void music_off (void)
{
	music_set (MUS_OFF);
}


/** Poll the sound board for data.
 * If no data is ready, returns 0xFF. */
U8 sound_board_poll (void)
{
	if (pinio_sound_ready_p ())
		return (pinio_read_sound ());
	else
		return (0xFF);
}


/** Read an expected value from the sound board.
 * If data is not available, a number of retries will be performed
 * before bailing and returning 0xFF. */
U8 sound_board_read (U8 retries)
{
	/* TODO - the sound board is not emulated in native mode */
#ifndef CONFIG_NATIVE
	do {
		if (queue_empty_p ((queue_t *)&sound_read_queue))
			task_sleep (TIME_100MS);
		else
		{
			U8 in = queue_remove ((queue_t *)&sound_read_queue, SOUND_QUEUE_LEN);
			return in;
		}
	} while (--retries != 0);
#endif
	return (0xFF);
}


/** Send a command to the sound board and wait for a reply.
 * If no reply is ready, a number of retries will be performed
 * before bailing and returning 0xFF.  On each retry, the
 * command is sent again. */
U8 sound_board_command (sound_cmd_t cmd, U8 retries)
{
#ifndef CONFIG_NATIVE
	do {
#if (MACHINE_DCS == 1)
		sound_write_queue_insert (cmd >> 8);
		sound_write_queue_insert (cmd & 0xFF);
#else
		sound_write_queue_insert (cmd);
#endif
		task_sleep (TIME_33MS);

		if (queue_empty_p ((queue_t *)&sound_read_queue))
			task_sleep (TIME_100MS);
		else
		{
			U8 in = queue_remove ((queue_t *)&sound_read_queue, SOUND_QUEUE_LEN);
			return in;
		}
	} while (--retries != 0);
#endif
	return (0xFF);
}


/** At idle time, poll the sound board for asynchronous events. */
CALLSET_ENTRY (sound, idle_every_100ms)
{
#if 1
	U8 in;
	/* TODO : do something like this elsewhere for sound syncing */
	if (!sys_init_pending_tasks
		&& !queue_empty_p ((queue_t *)&sound_read_queue))
	{
		in = queue_remove ((queue_t *)&sound_read_queue, SOUND_QUEUE_LEN);
		dbprintf ("Idle sound board read: %02Xh\n", in);
	}
#endif
}


/** Real time task for the sound board.
 * Transmit one pending byte of data to the sound board.
 * Receive up to one pending byte of data from it. */
void sound_read_rtt (void)
{
	/* Read back from sound board if bytes ready */
	if (unlikely (pinio_sound_ready_p ()))
	{
		queue_insert ((queue_t *)&sound_read_queue, SOUND_QUEUE_LEN,
			pinio_read_sound ());
	}
}

void sound_write_rtt (void)
{
	/* Write a pending byte to the sound board */
	if (unlikely (!sound_write_queue_empty_p ()))
	{
		pinio_write_sound (sound_write_queue_remove ());
	}
}


void sound_reset (void)
{
	music_off ();	
}


void volume_refresh (void)
{
	/* Use nvram value if it's sensible */
	if (current_volume_checksum == ~current_volume && current_volume < MAX_VOLUME)
		volume_set (current_volume);
	else
		volume_set (DEFAULT_VOLUME);
}


/** Initialize the sound board.  Because this involves a separate
device, this function is run in the background in a separate task. */
void sound_init (void)
{
	/* Initialize the input/output queues to the sound board. */
	sound_read_queue_init ();
	sound_write_queue_init ();
}


void sound_board_init (void)
{
	task_sleep_sec (2);

	/* Wait for the sound board to report its presence/type code */
	dbprintf ("Waiting for sound board...\n");
	if ((sound_board_type = sound_board_read (60)) == 0xFF)
	{
		dbprintf ("Error: sound board not detected\n");
		task_sleep_sec (1);
		goto exit_func;
	}
	dbprintf ("Sound board detected: type %02X\n", sound_board_type);

	/* Check for sound board errors.
	 * If no value is read, this is OK... otherwise it is an error
	 * code */
	dbprintf ("Checking boot code...\n");
	if ((sound_error_code = sound_board_read (20)) != 0xFF)
	{
		dbprintf ("Sound board boot code: %02X\n", sound_error_code);
		task_sleep_sec (1);
		goto exit_func;
	}

	task_sleep (TIME_200MS); /* TODO : needed? */

	/* Read the sound board version. */
	sound_version_major = sound_board_command (SND_GET_VERSION_CMD, 40);
#if (MACHINE_DCS == 1)
	sound_version_minor = sound_board_command (SND_GET_MINOR_VERSION_CMD, 40);
#endif

#if (MACHINE_DCS == 1)
	dbprintf ("Detected %d.%d sound.\n",
		sound_version_major >> 4, sound_version_major & 0x0F);
#else
	dbprintf ("Detected L-%d sound.\n", sound_version_major);
#endif

exit_func:
	volume_refresh ();
	sys_init_pending_tasks--;
	task_exit ();
}


/**
 * Write a 16-bit value to the sound board.
 */
__attribute__((noinline)) void sound_write (sound_code_t code)
{
	U8 code_lo;
	U8 code_hi;

	code_lo = code & 0xFF;
	code_hi = code >> 8;

#if (MACHINE_DCS == 0)
	if (code_hi == 0)
	{
		sound_write_queue_insert (code_lo);
	}
	else
#endif
	{
#if (MACHINE_DCS == 1)
		sound_write_queue_insert (code_hi);
#else
		sound_write_queue_insert (SND_START_EXTENDED);
#endif
		sound_write_queue_insert (code_lo);
	}
}


/** Send a command to the sound board. */
void sound_send (sound_code_t code)
{
	if (current_volume == 0)
		return;
	sound_start (ST_ANY, code, SL_500MS, 1);
}


/** Send a volume set command to the sound board */
void volume_set (U8 vol)
{
	/* Adhere to the minimum volume override */
	if (vol < system_config.min_volume_control)
		vol = system_config.min_volume_control;
	if (vol > MAX_VOLUME)
		vol = MAX_VOLUME;

	/* Save the volume level in nvram. */
	pinio_nvram_unlock ();
	current_volume = vol;
	current_volume_checksum = ~vol;
	pinio_nvram_lock ();

	if (current_volume == 0)
	{
		/* Note: if music is currently running, it is not
		explicitly stopped, although future sound board writes
		will not be transmitted. */
		music_off ();
	}
	else
	{
#if (MACHINE_DCS == 1)
		U8 code = current_volume * 8;
		sound_write_queue_insert (0x55);
		sound_write_queue_insert (0xAA);
		sound_write_queue_insert (code);
		sound_write_queue_insert (~code);
#else
		sound_write_queue_insert (SND_SET_VOLUME_CMD);
		sound_write_queue_insert (current_volume);
		sound_write_queue_insert (~current_volume);
#endif
	}
}


CALLSET_ENTRY (sound, factory_reset)
{
	volume_set (DEFAULT_VOLUME);
}


CALLSET_ENTRY (sound, music_refresh)
{
	if (!in_game && (deff_get_active () == DEFF_VOLUME_CHANGE))
		music_request (MACHINE_VOLUME_CHANGE_MUSIC, PRI_VOLUME_CHANGE_MUSIC);
}


/** Decrease the master volume */
CALLSET_ENTRY (sound, volume_down)
{
	if (current_volume > MIN_VOLUME)
		volume_set (current_volume-1);
	deff_restart (DEFF_VOLUME_CHANGE);
	effect_update_request ();
}


/** Increase the master volume */
CALLSET_ENTRY (sound, volume_up)
{
	if (current_volume < MAX_VOLUME)
		volume_set (current_volume+1);
	deff_restart (DEFF_VOLUME_CHANGE);
	effect_update_request ();
}

