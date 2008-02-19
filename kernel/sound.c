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
#include <queue.h>

/**
 * \file
 * \brief Sound and music control
 * This file implements the interface to the sound board hardware, both the WPC
 * and DCS versions.
 */


/** The length of the sound queue buffer.  These are bytes pending transmit from
 * the CPU board to the sound board. */
#define SOUND_QUEUE_LEN 8


/** The sound write queue.  head and tail are offsets; elems hold the actual
 * data bytes to be transmitted. */
__fastram__ struct {
	U8 head;
	U8 tail;
	U8 elems[SOUND_QUEUE_LEN];
} sound_write_queue;

/** The sound read queue, which works just like the write queue but takes
back data from the sound board. */
__fastram__ struct {
	U8 head;
	U8 tail;
	U8 elems[SOUND_QUEUE_LEN];
} sound_read_queue;

/** The last music code transmitted */
__fastram__ music_code_t current_music;

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

/** The default audio track to be played when setting volume. */
const audio_track_t volume_change_music_track = {
	.prio = PRI_VOLUME_CHANGE_MUSIC,
#ifdef MACHINE_VOLUME_CHANGE_MUSIC
	.code = MACHINE_VOLUME_CHANGE_MUSIC
#else
	.code = 2,
#endif
};


/** Renders the sound board version into the print buffer. */
bool sound_version_render (void)
{
	if (sound_error_code == 0xFF)
	{
		return FALSE;
	}
	else
	{
#ifdef MACHINE_DCS
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
	queue_init ((queue_t *)&sound_write_queue);
	enable_irq ();
}

/** Initialize the sound read queue */
static void sound_read_queue_init (void)
{
	disable_irq ();
	queue_init ((queue_t *)&sound_read_queue);
	enable_irq ();
}


/** Queues a byte for transmit to the sound board */
static void sound_write_queue_insert (U8 val)
{
	queue_insert ((queue_t *)&sound_write_queue, SOUND_QUEUE_LEN, val);
}


/** Dequeues a byte for transmit to the sound board */
static U8 sound_write_queue_remove (void)
{
	return queue_remove ((queue_t *)&sound_write_queue, SOUND_QUEUE_LEN);
}


/** Checks whether the write queue is empty or not */
inline bool sound_write_queue_empty_p (void)
{
	return queue_empty_p ((queue_t *)&sound_write_queue);
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
	U8 status = wpc_asic_read (WPCS_CONTROL_STATUS);
	U8 in_data;

	if (status & WPCS_READ_READY)
	{
		in_data = wpc_asic_read (WPCS_DATA);
		return (in_data);
	}
	else
		return (0xFF);
}


/** Read an expected value from the sound board.
 * If data is not available, a number of retries will be performed
 * before bailing and returning 0xFF. */
U8 sound_board_read (U8 retries)
{
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
	/* TODO : for DCS, cmd could be 16-bit!!! */
#ifndef CONFIG_NATIVE
	do {
		sound_write_queue_insert (cmd);
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
#if 0
	U8 in;
	/* TODO : do something like this elsewhere for sound syncing */
	if (sys_init_complete
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
	if (unlikely (wpc_asic_read (WPCS_CONTROL_STATUS) & WPCS_READ_READY))
	{
		queue_insert ((queue_t *)&sound_read_queue, SOUND_QUEUE_LEN, 
			wpc_asic_read (WPCS_DATA));
	}
}

void sound_write_rtt (void)
{
	/* Write a pending byte to the sound board */
	if (unlikely (!sound_write_queue_empty_p ()))
	{
		wpc_asic_write (WPCS_DATA, sound_write_queue_remove ());
	}
}


void sound_reset (void)
{
	music_off ();	
}


/** Initialize the sound board.  Because this involves a separate
device, this function is run in the background in a separate task. */
void sound_init (void)
{
	U8 sound_board_type;

	sound_read_queue_init ();

	/* Wait for the sound board to report its presence/type code */
	dbprintf ("Waiting for sound board...\n");
	if ((sound_board_type = sound_board_read (100)) == 0xFF)
	{
		dbprintf ("Error: sound board not detected\n");
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
		goto exit_func;
	}

	/* Initialize the write queue.  We cannot transmit anything before here. */
	sound_write_queue_init ();
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
	/* Use nvram value if it's sensible */
	if (current_volume_checksum == ~current_volume && current_volume < MAX_VOLUME)
		volume_set (current_volume);
	else
		volume_set (DEFAULT_VOLUME);

	sys_init_pending_tasks--;
	task_exit ();
}


/** Send a command to the sound board. */
void sound_send (sound_code_t code)
{
	U8 code_lo;
	U8 code_hi;

	if (current_volume == 0)
		return;

#ifdef __m6809__
	/* GCC still is doing this efficiently, so we are
	hand assembling it for now. */
	asm ("ldd\t%0" :: "m" (code));
	asm ("sta\t%0" :: "m" (code_hi));
	asm ("stb\t%0" :: "m" (code_lo));
#else
	code_lo = code & 0xFF;
	code_hi = code >> 8;
#endif

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


/** Send a volume set command to the sound board */
void volume_set (U8 vol)
{
	/* Adhere to the minimum volume override */
	if (vol < system_config.min_volume_control)
		vol = system_config.min_volume_control;

	/* Save the volume level in nvram. */
	wpc_nvram_get ();
	current_volume = vol;
	current_volume_checksum = ~vol;
	wpc_nvram_put ();

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


/** Decrease the master volume */
CALLSET_ENTRY (sound, volume_down)
{
	if (current_volume > MIN_VOLUME)
	{
		volume_set (current_volume-1);
		task_sleep (TIME_100MS);
	}
	music_start (volume_change_music_track);
	deff_restart (DEFF_VOLUME_CHANGE);
}


/** Increase the master volume */
CALLSET_ENTRY (sound, volume_up)
{
	if (current_volume < MAX_VOLUME)
	{
		volume_set (current_volume+1);
		task_sleep (TIME_100MS);
	}
	music_start (volume_change_music_track);
	deff_restart (DEFF_VOLUME_CHANGE);
}

