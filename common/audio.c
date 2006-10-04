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

/* Common audio routines.  This is a layer of abstraction
above the raw kernel functions.   It works similar to the display
effect functions. */

typedef enum {
	CH_SOUND1=0,
	CH_SOUND2,
	CH_SPEECH1,
	CH_SPEECH2,
	MAX_AUDIO_CHANNELS,
} audio_channel_id_t;

#define AUDIO_REF0	DEFF_CH_SOUND1
#define AUDIO_REF1	DEFF_CH_SOUND2
#define AUDIO_REF2	DEFF_CH_SPEECH1
#define AUDIO_REF3	DEFF_CH_SPEECH2


typedef struct {
	task_t *task;
	task_ticks_t gap;
	task_gid_t gid;
} audio_channel_t;


audio_channel_t audio_channel_table[MAX_AUDIO_CHANNELS];


void audio_channel_config (audio_channel_id_t id,
	task_ticks_t gap)
{
}


void audio_control_task (void)
{
	audio_channel_t *ch = (audio_channel_t *)task_get_arg ();

	task_sleep (ch->gap);

	ch->task = 0;
	task_exit ();
}


bool audio_start (audio_channel_id_t id,
	task_function_t fn,
	U8 fnpage)
{
	audio_channel_t *ch = &audio_channel_table[id];
	if (ch->task == NULL)
	{
		ch->task = task_create_gid (ch->gid, audio_control_task);
		task_set_arg (ch->task, (U16)ch);
		// *((U16 *)(&ch->task->thread_data[0])) = fn;
		////  *((U8 *)(&ch->task->thread_data[2])) = fnpage;
	}
}


void audio_init (void)
{
	memset (audio_channel_table, 0, sizeof (audio_channel_table));
}

