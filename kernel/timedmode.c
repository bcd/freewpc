/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

static void effect_update (void)
{
	music_refresh ();
	deff_update ();
}


static void timed_mode_exit_handler (struct timed_mode_ops *ops)
{
	if (in_live_game && ops->deff_ending)
		deff_start_retry (ops->deff_ending, 5);
	*ops->timer = 0;
	ops->exit ();
}


void timed_mode_monitor (void)
{
	struct timed_mode_task_config *cfg;
	struct timed_mode_ops *ops;

	cfg = task_current_class_data (struct timed_mode_task_config);
	ops = cfg->ops;

#define the_timer (*(ops->timer))

	effect_update ();

	while (the_timer > 0)
	{
		do {
			task_sleep (TIME_1S + TIME_33MS);
		} while (ops->pause ());
		the_timer--;
	}

	ops->timeout ();
	task_sleep_sec (1);
	effect_update ();
	if (ops->grace_timer > 1)
		task_sleep_sec (ops->grace_timer - 1);

	timed_mode_exit_handler (ops);
	task_exit ();
}


void timed_mode_begin (struct timed_mode_ops *ops)
{
	task_pid_t tp;
	struct timed_mode_task_config *cfg;

	*ops->timer = ops->init_timer;
	if (ops->deff_starting)
		deff_start (ops->deff_starting);
	tp = task_create_gid1 (ops->gid, timed_mode_monitor);
	cfg = task_init_class_data (tp, struct timed_mode_task_config);
	cfg->ops = ops;
	ops->init ();
}


void timed_mode_finish (struct timed_mode_ops *ops)
{
	ops->finish ();
	timed_mode_exit_handler (ops);
	task_kill_gid (ops->gid);
}


U8 timed_mode_get_timer (struct timed_mode_ops *ops)
{
	return *(ops->timer);
}

bool timed_mode_running_p (struct timed_mode_ops *ops)
{
	return in_live_game && task_find_gid (ops->gid);
}


void timed_mode_reset (struct timed_mode_ops *ops, U8 time)
{
	if (!task_find_gid (ops->gid))
		return;

	if (*ops->timer == 0)
	{
		task_pid_t tp;
		struct timed_mode_task_config *cfg;

		tp = task_recreate_gid (ops->gid, timed_mode_monitor);
		cfg = task_init_class_data (tp, struct timed_mode_task_config);
		cfg->ops = ops;
	}

	*ops->timer = time;
}


void timed_mode_add (struct timed_mode_ops *ops, U8 time)
{
	time += timed_mode_get_timer (ops);
	timed_mode_reset (ops, time);
}


void timed_mode_music_refresh (struct timed_mode_ops *ops)
{
	if (ops->music && *ops->timer > 0 && task_find_gid (ops->gid))
		music_request (ops->music, ops->prio);
}


void timed_mode_deff_update (struct timed_mode_ops *ops)
{
	if (ops->deff_running && *ops->timer > 0 && task_find_gid (ops->gid))
		deff_start_bg (ops->deff_running, ops->prio);
}


