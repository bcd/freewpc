
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
 */

#define SOUND_QUEUE_LEN 8

struct {
	U8 head;
	U8 tail;
	U8 elems[SOUND_QUEUE_LEN];
} sound_queue;

#define MUSIC_STACK_SIZE 8

__fastram__ music_code_t music_stack[MUSIC_STACK_SIZE];
__fastram__ music_code_t *music_head;

__nvram__ U8 current_volume;

U8 sound_error_code;

U8 sound_version_major;
U8 sound_version_minor;

#define sound_version sound_version_major


static void sound_queue_init (void)
{
	disable_irq ();
	queue_init ((queue_t *)&sound_queue);
	enable_irq ();
}

static void sound_queue_insert (U8 val)
{
	queue_insert ((queue_t *)&sound_queue, SOUND_QUEUE_LEN, val);
}

static U8 sound_queue_remove (void)
{
	return queue_remove ((queue_t *)&sound_queue, SOUND_QUEUE_LEN);
}

inline bool sound_queue_empty (void)
{
	return queue_empty ((queue_t *)&sound_queue);
}


void music_set (music_code_t code)
{
	*music_head = code;

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
		sound_queue_insert (*music_head);
	}
}

void music_off (void)
{
	music_head = music_stack;
	music_set (MUS_OFF);
}

void music_change (music_code_t code)
{
	if (code != *music_head)
		music_set (code);
}


#if (MACHINE_DCS == 1)

U8 sound_board_poll (void)
{
	U8 status = *(volatile U8 *)WPCS_CONTROL_STATUS;
	U8 in_data;

	if (status & 0x80)
	{
		in_data = *(volatile U8 *)WPCS_DATA;
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

#endif /* DCS */

void sound_rtt (void)
{
	if (!sound_queue_empty ())
	{
		*(volatile U8 *)WPCS_DATA = sound_queue_remove ();
	}
}


void sound_reset (void)
{
	music_off ();	
}


void sound_ready (void)
{
	sound_queue_init ();
	music_off ();
	volume_set (DEFAULT_VOLUME);
}

void sound_init (void)
{
#if (MACHINE_DCS == 0)
	/* TODO : WPC sound also need to run as a background thread,
	 * waiting for sync from the sound board */
	*(U8 *)WPCS_CONTROL_STATUS = 0;
	sound_ready ();
#else
	int i;
	long int j;
#if 1
	static U8 dcs_init_string[] = {
		0x8C, 0xB2, 0x7B, 0x40, 0x49, 0xFB, 0xE5, 0xAF, 0x59, 0x7B,
		0xC4, 0xAA, 0x83, 0x37, 0x28, 0xC8, 0xE6, 0xE7, 0xD4, 0x85,
		0xD9, 0x16, 0x10, 0x64, 0x58, 0xC6, 0xCC, 0x93, 0x85, 0x0F,
		0x7C
	};

	for (i=0; i < sizeof (dcs_init_string); i++)
	{
		for (j=0; j < 4; j++)
		{
			*(U8 *)WPCS_CONTROL_STATUS = dcs_init_string[i];
			task_sleep (1); /* 16ms */
			*(U8 *)WPCS_CONTROL_STATUS = dcs_init_string[i];
			task_sleep (1); /* 16ms */
			*(U8 *)WPCS_CONTROL_STATUS = dcs_init_string[i];
			task_sleep (1); /* 16ms */
			*(U8 *)WPCS_CONTROL_STATUS = dcs_init_string[i];
			task_sleep (1); /* 16ms */
		}
	}
#endif

	/* Wait for the sound board to report its presence, by
	 * reading a value of 0x79. */
	dbprintf ("Waiting for sound board...\n");
	i = 100;
	for (;;)
	{
		if ((sound_board_poll ()) == 0x79)
			break;
		else
		{
			task_sleep (TIME_33MS);
			if (i-- == 0)
			{
				dbprintf ("Error: sound board not detected\n");
				goto exit_func;
			}
		}
	}
	(void)sound_board_poll ();

	dbprintf ("Checking for errors...\n");
	sound_error_code = sound_board_read (100);
	(void)sound_board_poll ();

	sound_queue_init ();
	task_sleep_sec (1);

	dbprintf ("Error flag is %02X ; checking version...\n", sound_error_code);
	i = 8;
	do {
		sound_send (SND_GET_VERSION_CMD);
		task_sleep (TIME_100MS);
		sound_version = sound_board_poll ();
	} while ((sound_version == 255) && (--i != 0));
	(void)sound_board_poll ();

	i = 8;
	do {
		sound_send (SND_GET_UNKNOWN_CMD);
		task_sleep (TIME_100MS);
		sound_version_minor = sound_board_poll ();
	} while ((sound_version_minor == 255) && (--i != 0));
	(void)sound_board_poll ();

#if (MACHINE_DCS == 1)
	dbprintf ("Detected %d.%d sound.\n", sound_version >> 4, sound_version & 0x0F);
#else
	dbprintf ("Detected L-%d sound.\n", sound_version);
#endif

	volume_set (DEFAULT_VOLUME);

exit_func:
	sys_init_pending_tasks--;
	task_exit ();
#endif
}

void sound_send (sound_code_t code)
{
	U8 code_lo;
	U8 code_hi;

	if (current_volume == 0)
		return;

	asm ("ldd\t%0" :: "m" (code));
	asm ("sta\t%0" :: "m" (code_hi));
	asm ("stb\t%0" :: "m" (code_lo));

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


void volume_set (U8 vol)
{
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


void volume_deff (void) __taskentry__
{
	dmd_alloc_low_clean ();
	sprintf ("VOLUME %d", current_volume);
	font_render_string_center (&font_fixed6, 64, 13, sprintf_buffer);
	if (!in_game)
#ifdef MACHINE_VOLUME_CHANGE_MUSIC
		music_change (MACHINE_VOLUME_CHANGE_MUSIC);
#else
		music_change (1);
#endif
	dmd_show_low ();
	task_sleep_sec (4);
	if (!in_game)
		music_off ();
	deff_exit ();
}


void volume_down (void)
{
	if (current_volume > MIN_VOLUME)
	{
		volume_set (current_volume-1);
	}
	deff_restart (DEFF_VOLUME_CHANGE);
}


void volume_up (void)
{
	if (current_volume < MAX_VOLUME)
	{
		volume_set (current_volume+1);
	}
	deff_restart (DEFF_VOLUME_CHANGE);
}


CALLSET_ENTRY(sound, start_game)
{
#ifdef MACHINE_START_GAME_SOUND
	sound_send (MACHINE_START_GAME_SOUND);
#endif
}


CALLSET_ENTRY(sound, start_ball)
{
#ifdef MACHINE_START_BALL_MUSIC
	music_set (MACHINE_START_BALL_MUSIC);
#endif
}


CALLSET_ENTRY (sound, ball_in_play)
{
#ifdef MACHINE_BALL_IN_PLAY_MUSIC
	music_set (MACHINE_BALL_IN_PLAY_MUSIC);
#endif
}


CALLSET_ENTRY(sound, end_ball)
{
	sound_reset ();
}


CALLSET_ENTRY(sound, end_game)
{
#ifdef MACHINE_END_GAME_MUSIC
	if (!in_test)
		music_set (MACHINE_END_GAME_MUSIC);
#endif
}

