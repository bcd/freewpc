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
struct {
	U8 head;
	U8 tail;
	U8 elems[SOUND_QUEUE_LEN];
} sound_queue;

/** The last music code transmitted */
__fastram__ music_code_t current_music;

/** The current master volume.  Individual sound clips may override this
 * temporarily, but this is the default. */
__nvram__ U8 current_volume;

U8 sound_error_code;

U8 sound_version_major;

U8 sound_version_minor;

#define sound_version sound_version_major


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


void music_change (music_code_t code)
{
	if (code != current_music)
		music_set (code);
}



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

U8 sound_board_read (U8 retries)
{
	do {
		U8 in = sound_board_poll ();
		if (in != 0xFF)
			return (in);
		else
			task_sleep (TIME_66MS);
	} while (--retries != 0);
	return (0xFF);
}


/** Real time task for the sound board.
 * Transmit one pending byte of data to the sound board. */
void sound_rtt (void)
{
	/* PinMAME is generating FIRQs continuously if we don't read
	 * this register occasionally. */
	(void) wpc_asic_read (WPCS_CONTROL_STATUS);

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
	U8 i;
	U16 j;
	U8 sound_board_type;

#if (MACHINE_DCS == 0)
	static U8 init_string[] = {
		0x00, 0x8C, 0xB2, 0x7B, 0x40, 0x49, 0xFB, 0xE5, 0xAF, 0x59, 0x7B,
		0xC4, 0xAA, 0x83, 0x37, 0x28, 0xC8, 0xE6, 0xE7, 0xD4, 0x85,
		0xD9, 0x16, 0x10, 0x64, 0x58, 0xC6, 0xCC, 0x93, 0x85, 0x0F,
		0x7C
	};

	for (i=0; i < sizeof (init_string); i++)
	{
		wpc_asic_write (WPCS_CONTROL_STATUS, init_string[i]);
		task_sleep (TIME_16MS);
	}
#endif


	/* Wait for the sound board to report its presence/type code */
	dbprintf ("Waiting for sound board...\n");
	i = 100; /* wait for up to 10 seconds */
	for (;;)
	{
		if ((sound_board_type = sound_board_poll ()) != 0xFF)
			break;
		else
		{
			task_sleep (TIME_100MS);
			if (i-- == 0)
			{
				dbprintf ("Error: sound board not detected\n");
				goto exit_func;
			}
		}
	}
	(void)sound_board_poll ();
	dbprintf ("Sound board type: %02X\n", sound_board_type);

	/* Read its boot code as well */
	dbprintf ("Checking boot code...\n");
	i = 20; /* wait for up to 2 seconds */
	for (;;)
	{
		if ((sound_error_code = sound_board_poll ()) != 0xFF)
			break;
		else
		{
			task_sleep (TIME_100MS);
			if (i-- == 0)
				break;
		}
	}
	(void)sound_board_poll ();
	dbprintf ("Sound board boot code: %02X\n", sound_error_code);

	/* Initialize the sound queue.  We cannot transmit anything before here. */
	sound_queue_init ();
	task_sleep_sec (1);

	/* Read the sound board version.  DCS does two reads here; WPC only one? */
	i = 8;
	do {
		sound_send (SND_GET_VERSION_CMD);
		task_sleep (TIME_100MS);
		sound_version = sound_board_poll ();
	} while ((sound_version == 255) && (--i != 0));
	(void)sound_board_poll ();

#if (MACHINE_DCS == 1)
	i = 8;
	do {
		sound_send (SND_GET_UNKNOWN_CMD);
		task_sleep (TIME_100MS);
		sound_version_minor = sound_board_poll ();
	} while ((sound_version_minor == 255) && (--i != 0));
	(void)sound_board_poll ();
#endif

#if (MACHINE_DCS == 1)
	dbprintf ("Detected %d.%d sound.\n", sound_version >> 4, sound_version & 0x0F);
#else
	dbprintf ("Detected L-%d sound.\n", sound_version);
#endif

	/* TODO  - use nvram value if it's sensible */
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
	/* Save the volume level in nvram.
	 * TODO : checksum this? */
	wpc_nvram_get ();
	current_volume = vol;
	wpc_nvram_put ();

	if (current_volume == 0)
	{
		music_change (MUS_OFF);
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


void volume_change_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	sprintf ("VOLUME %d", current_volume);
	font_render_string_center (&font_fixed6, 64, 13, sprintf_buffer);
	dmd_show_low ();

	if (current_music == MUS_OFF)
	{
#ifdef MACHINE_VOLUME_CHANGE_MUSIC
		music_change (MACHINE_VOLUME_CHANGE_MUSIC);
#else
		music_change (1);
#endif
		task_sleep_sec (4);
		music_off ();
	}
	else
	{
		task_sleep_sec (4);
	}
	deff_exit ();
}


/** Decrease the master volume */
void volume_down (void)
{
	if (current_volume > MIN_VOLUME)
	{
		volume_set (current_volume-1);
	}
	deff_restart (DEFF_VOLUME_CHANGE);
}


/** Increase the master volume */
void volume_up (void)
{
	if (current_volume < MAX_VOLUME)
	{
		volume_set (current_volume+1);
	}
	deff_restart (DEFF_VOLUME_CHANGE);
}

