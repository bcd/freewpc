/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef _MBMODE_H
#define _MBMODE_H

enum mb_mode_state
{
	MB_INACTIVE,
	MB_ACTIVE,
	MB_IN_GRACE,
};

struct mb_mode_ops
{
	/* The update callback is invoked whenever the state of the
	multiball changes. */
	U8 callback_page;
	void (*update) (enum mb_mode_state state);
	U8 music;
	U8 deff_starting;
	U8 deff_running;
	U8 deff_ending;
	U8 prio;
	task_gid_t gid_running;
	task_gid_t gid_in_grace;
	task_ticks_t grace_period;
};

struct mb_mode_task_config
{
	struct mb_mode_ops *ops;
};

U8 mb_mode_running_count (void);

void mb_mode_start (struct mb_mode_ops *ops);
void mb_mode_restart (struct mb_mode_ops *ops);
void mb_mode_single_ball (struct mb_mode_ops *ops);
void mb_mode_end_ball (struct mb_mode_ops *ops);
void mb_mode_music_refresh (struct mb_mode_ops *ops);
void mb_mode_display_update (struct mb_mode_ops *ops);
enum mb_mode_state mb_mode_get_state (struct mb_mode_ops *ops);

extern inline bool mb_mode_effect_running_p (struct mb_mode_ops *ops)
{
	return mb_mode_get_state (ops) == MB_ACTIVE;
}

extern inline bool mb_mode_inactive_p (struct mb_mode_ops *ops)
{
	return mb_mode_get_state (ops) == MB_INACTIVE;
}

extern inline bool mb_mode_in_grace_p (struct mb_mode_ops *ops)
{
	return mb_mode_get_state (ops) == MB_IN_GRACE;
}

extern inline bool mb_mode_running_p (struct mb_mode_ops *ops)
{
	return mb_mode_get_state (ops) >= MB_ACTIVE;
}

#endif /* _MBMODE_H */
