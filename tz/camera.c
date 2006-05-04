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


void sw_camera_handler (void)
{
	if (task_kill_gid (GID_CAMERA_DISABLED_BY_DEAD_END))
	{
	}
	else
	{
		score_add_current_const (SCORE_10K * 3);
		sound_send (SND_CAMERA_PICTURE_EJECT_1);
		timer_restart_free (GID_SLOT_DISABLED_BY_CAMERA, TIME_4S);
	}
}


CALLSET_ENTRY(camera, start_ball)
{
	lamp_flash_on (LM_CAMERA);
}


DECLARE_SWITCH_DRIVER (sw_camera)
{
	.fn = sw_camera_handler,
	.flags = SW_PLAYFIELD | SW_IN_GAME,
};

