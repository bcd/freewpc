
/*
 * Copyright 2007 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Voice APIs
 *
 * This module is a wrapper around the sound APIs to be used when
 * making voice calls.  The purpose is to avoid making speech calls
 * that overlap or conflict in some way, to make it more aesthetically
 * pleasing.  The caller must provide a voice ID in addition to the
 * sound call.  The voice module can detect when the sound call completes
 * and not allow another voice call to be made in the interim.
 */


U16 voice_call_pending;


void voice_running_task (void)
{
	sound_send (voice_call_pending);
	/* Wait for the sound call to finish */
	task_sleep_sec (1);
	task_exit ();
}


void voice_send (U8 voice_id, U16 sound_call)
{
	task_pid_t tp;

	tp = task_find_gid (GID_VOICE_CALL_RUNNING);
	if (tp == NULL)
	{
		voice_call_pending = sound_call;
		tp = task_create_gid (GID_VOICE_CALL_RUNNING, voice_running_task);
	}
}

