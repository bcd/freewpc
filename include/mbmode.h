/*
 * Copyright 2010-2011 by Brian Dominy <brian@oddchange.com>
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

/* The state of the multiball mode is one of these values.
   Each module that implements a mode must declare a variable of
	this type. */
enum mb_mode_state
{
	MB_INACTIVE,
	MB_ACTIVE,
	MB_IN_GRACE,
};


/* The definition of the mode. */
struct mb_mode_ops
{
	/* The update callback is invoked whenever the state of the
	multiball changes. */
	void (*update) (enum mb_mode_state state);
	U8 music;
	U8 deff_starting;
	U8 deff_running;
	U8 deff_ending;
	U8 prio;
	void_function active_task;
	task_gid_t gid_running;
	task_gid_t gid_in_grace;
	task_ticks_t grace_period;
	enum mb_mode_state *state;
};

/* The default values for mode fields */
#define DEFAULT_MBMODE \
	.update = NULL, \
	.music = MUS_OFF, \
	.deff_starting = DEFF_NULL, \
	.deff_running = DEFF_NULL, \
	.deff_ending = DEFF_NULL, \
	.prio = PRI_NULL, \
	.active_task = mb_mode_active_task, \
	.grace_period = TIME_500MS


struct mb_mode_task_config
{
	struct mb_mode_ops *ops;
};

U8 mb_mode_running_count (void);
void mb_mode_active_task (void);
void mb_mode_start (struct mb_mode_ops *ops);
void mb_mode_restart (struct mb_mode_ops *ops);
void mb_mode_single_ball (struct mb_mode_ops *ops);
void mb_mode_start_ball (struct mb_mode_ops *ops);
void mb_mode_end_ball (struct mb_mode_ops *ops);
void mb_mode_music_refresh (struct mb_mode_ops *ops);
void mb_mode_display_update (struct mb_mode_ops *ops);

extern inline enum mb_mode_state mb_mode_get_state (struct mb_mode_ops *ops)
{
	return *(ops->state);
}

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
