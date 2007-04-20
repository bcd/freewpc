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


/** The sound queue structure.  head and tail are offsets; elems hold the actual
 * data bytes to be transmitted. */
__fastram__ struct {
	U8 head;
	U8 tail;
	U8 elems[SOUND_QUEUE_LEN];
} sound_queue;

/** The last music code transmitted */
__fastram__ music_code_t current_music;

/** The current master volume.  Individual sound clips may override this
 * temporarily, but this is the default. */
__nvram__ U8 current_volume;
__nvram__ U8 current_volume_checksum;

U8 sound_error_code;

U8 sound_version_major;

U8 sound_version_minor;

U8 sound_board_return;

#define sound_version sound_version_major


/** The default audio track to be played when setting volume. */
const audio_track_t volume_change_music_track = {
	.prio = PRI_NULL,
#ifdef MACHINE_VOLUME_CHANGE_MUSIC
	.code = MACHINE_VOLUME_CHANGE_MUSIC
#else
	.code = 1,
#endif
};


/** Initialize the sound transmit code */
static void sound_queue_init (void)
{
	disable_irq ();
	queue_init ((queue_t *)&sound_queue);
	enable_irq ();
}


/** Queues a byte for transmit to the sound board */
static void sound_queue_insert (U8 val)
{
	queue_insert ((queue_t *)&sound_queue, SOUND_QUEUE_LEN, val);
}


/** Dequeues a byte for transmit to the sound board */
static U8 sound_queue_remove (void)
{
	return queue_remove ((queue_t *)&sound_queue, SOUND_QUEUE_LEN);
}


/** Empties the sound transmit queue */
inline bool sound_queue_empty (void)
{
	return queue_empty ((queue_t *)&sound_queue);
}


void music_set (music_code_t code)
{
	current_music = code;

	/* Music codes are not emitted if volume is set to zero, or
	 * if game music has been disabled.  But MUS_OFF is always
	 * sent to the sound board, regardless of everything else. */
	if (((current_volume > 0)
			&& (system_config.game_music == ON))
		|| (code == MUS_OFF))
	{
#if (MACHINE_DCS == 1)
		sound_queue_insert (0);
#endif
		sound_queue_insert (current_music);
	}
}

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

#if (MACHINE_DCS == 1)
	if (status & 0x80)
#else
	if (status & 0x1)
#endif
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
	do {
		U8 in = sound_board_poll ();
		if (in != 0xFF)
			return (in);
		else
			task_sleep (TIME_100MS);
	} while (--retries != 0);
	return (0xFF);
}


U8 sound_board_command (U8 cmd, U8 retries)
{
	do {
		sound_queue_insert (cmd);
		task_sleep (TIME_33MS);
		U8 in = sound_board_poll ();
		if (in != 0xFF)
			return (in);
		else
			task_sleep (TIME_100MS);
	} while (--retries != 0);
	return (0xFF);
}


CALLSET_ENTRY (sound, idle)
{
	static U8 last_return = 0;
	if (sound_board_return != last_return)
	{
		dbprintf ("Sound board return: %02Xh\n", sound_board_return);
		last_return = sound_board_return;
	}
}


/** Real time task for the sound board.
 * Transmit one pending byte of data to the sound board. */
void sound_rtt (void)
{
	/* PinMAME is generating FIRQs continuously if we don't read
	 * this register occasionally. */
	if (wpc_asic_read (WPCS_CONTROL_STATUS) & 0x1)
	{
		sound_board_return = wpc_asic_read (WPCS_DATA);
	}

	if (!sound_queue_empty ())
	{
		wpc_asic_write (WPCS_DATA, sound_queue_remove ());
	}
}


void sound_reset (void)
{
	music_off ();	
}


void sound_init (void)
{
	U8 sound_board_type;

	/* Wait for the sound board to report its presence/type code */
	dbprintf ("Waiting for sound board...\n");
	if ((sound_board_type = sound_board_read (50)) == 0xFF)
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

	/* Initialize the sound queue.  We cannot transmit anything before here. */
	sound_queue_init ();
	task_sleep_sec (1);

	/* Read the sound board version. */
	sound_version = sound_board_command (SND_GET_VERSION_CMD, 20);
#if (MACHINE_DCS == 1)
	sound_version_minor = sound_board_command (SND_GET_MINOR_VERSION_CMD, 20);
#endif

#if (MACHINE_DCS == 1)
	dbprintf ("Detected %d.%d sound.\n", sound_version >> 4, sound_version & 0x0F);
#else
	dbprintf ("Detected L-%d sound.\n", sound_version);
#endif

	/* Use nvram value if it's sensible */
	if (current_volume_checksum == ~current_volume)
		volume_set (current_volume);
	else
		volume_set (DEFAULT_VOLUME);

exit_func:
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
		sound_queue_insert (code_lo);
	}
	else
#endif
	{
#if (MACHINE_DCS == 1)
		sound_queue_insert (code_hi);
#else
		sound_queue_insert (SND_START_EXTENDED);
#endif
		sound_queue_insert (code_lo);
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
		/* TODO : kill bg audio task */
		music_off ();
	}
	else
	{
#if (MACHINE_DCS == 1)
		U8 code = current_volume * 8;
		sound_queue_insert (0x55);
		sound_queue_insert (0xAA);
		sound_queue_insert (code);
		sound_queue_insert (~code);
#else
		sound_queue_insert (SND_SET_VOLUME_CMD);
		sound_queue_insert (current_volume);
		sound_queue_insert (~current_volume);
#endif
	}
}


/** Decrease the master volume */
CALLSET_ENTRY (sound, volume_down)
{
	if (current_volume > MIN_VOLUME)
	{
		volume_set (current_volume-1);
	}
	bg_music_start (&volume_change_music_track);
	deff_restart (DEFF_VOLUME_CHANGE);
}


/** Increase the master volume */
CALLSET_ENTRY (sound, volume_up)
{
	if (current_volume < MAX_VOLUME)
	{
		volume_set (current_volume+1);
	}
	bg_music_start (&volume_change_music_track);
	deff_restart (DEFF_VOLUME_CHANGE);
}

