/*
 * Copyright 2009, 2010 by Brian Dominy <brian@oddchange.com>
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
#include <lamptimer.h>


/**
 * The instance data for a timed lamp effect.
 * This is used by the master task.
 */
struct lamp_timer_data
{
	lampnum_t lamp;
	U16 timer;
	task_pid_t effect;
};


/**
 * The instance data for the lamp update sub-task.
 * It is just a pointer to the master task's data.
 */
struct lamp_timer_effect_data
{
	struct lamp_timer_data *tdata;
	leff_data_t leffdata;
};


/**
 * The slave task which updates the lamp on a timer.
 */
static void lamp_timer_effect_task (void)
{
	struct lamp_timer_effect_data *edata;
	struct lamp_timer_data *tdata;

	edata = task_current_class_data (struct lamp_timer_effect_data);
	edata->leffdata.flags = L_SHARED;
	tdata = edata->tdata;

	if (lamp_leff2_test_and_allocate (tdata->lamp))
	{
		do {
			leff_toggle (tdata->lamp);
			task_sleep (TIME_50MS);
			if (tdata->timer >= TIME_9S)
				task_sleep (TIME_100MS);
			if (tdata->timer >= TIME_6S)
				task_sleep (TIME_100MS);
			if (tdata->timer >= TIME_3S)
				task_sleep (TIME_100MS);
		} while (tdata->timer != 0);
		lamp_leff2_free (tdata->lamp);
	}
	task_exit ();
}


/**
 * The master task which runs a timed lamp effect.
 * This task maintains the timer and counts it down.
 */
static void lamp_timer_task (void)
{
	struct lamp_timer_data *tdata;

	tdata = task_current_class_data (struct lamp_timer_data);
	while (tdata->timer >= TIME_166MS)
	{
		do {
			task_sleep (TIME_166MS);
		} while (system_timer_pause ());
		tdata->timer -= TIME_166MS;
	}

	tdata->timer = 0;
	task_sleep (TIME_500MS);
	task_exit ();
}


/**
 * Find the master task that is managed a timed lamp effect.
 */
task_pid_t
lamp_timer_find (lampnum_t lamp)
{
	task_pid_t tp = task_find_gid (GID_LAMP_TIMER);
	while (tp && ((task_class_data (tp, struct lamp_timer_data))->lamp != lamp))
		tp = task_find_gid_next (tp, GID_LAMP_TIMER);
	return tp;
}


/**
 * Start a timed lamp effect.
 *
 * LAMP is the lamp to be flashed on the timer.
 * SECS is the number of seconds to start on the timer.
 */
void
lamp_timer_start (struct lamptimer_args *args)
{
	task_pid_t tp;
	struct lamp_timer_data *tdata;
	struct lamp_timer_effect_data *edata;

	/* TODO - if the lamp timer is already running, it should be
	stopped first. */

	/* Create a task to count down the timer */
	tp = task_create_gid (GID_LAMP_TIMER, lamp_timer_task);
	tdata = task_init_class_data (tp, struct lamp_timer_data);
	tdata->lamp = args->lamp;
	tdata->timer = (U16)args->secs * TIME_1S;

	/* Create a second task for the lamp effect itself */
	tdata->effect = task_create_gid (GID_LAMP_TIMER_EFFECT, lamp_timer_effect_task);
	edata = task_init_class_data (tdata->effect, struct lamp_timer_effect_data);
	edata->tdata = tdata;
}


/**
 * Stop a timed lamp effect.
 *
 * LAMP is the lamp which could be flashing, which should be stopped.
 */
void
lamp_timer_stop (lampnum_t lamp)
{
	task_pid_t tp;
	struct lamp_timer_data *tdata;

	tp = lamp_timer_find (lamp);
	if (tp)
	{
		tdata = task_class_data (tp, struct lamp_timer_data);
		tdata->timer = 0;
		task_sleep (TIME_500MS);
		task_kill_pid (tp);
	}
}

