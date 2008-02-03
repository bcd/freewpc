/*
 * Copyright 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Common lamp effects
 */


/** The arguments to a lamp countdown effect task */
struct countdown_leff_config
{
	/** The lamp that should be flickered */
	U8 lamp;

	/** A pointer to the timer */
	U8 *timer;

	/** The generic lamp effect arguments.  All
	 * lamp effects must declare this. */
	leff_data_t leffdata;
};


/** The task for running a lamp countdown */
void lamp_countdown_task (void)
{
	struct countdown_leff_config *cfg;

	cfg = task_current_class_data (struct countdown_leff_config);
	
	cfg->leffdata.flags = L_SHARED;

	/* Allocate the lamp.  TODO - this may fail */
	lamp_leff2_allocate (cfg->lamp);
	if (1)
	{
		/* Monitor the timer */
		while (*cfg->timer != 0)
		{
			U8 sleep_time;
	
			leff_toggle (cfg->lamp);
	
			/* When the timer is at 1s, sleep 33ms between flashes.
			2s = 66ms, etc.  But sleep at most 500ms */
			sleep_time = *cfg->timer * 2;
			if (sleep_time > TIME_500MS)
				sleep_time = TIME_500MS;
			task_sleep (sleep_time);
		}
	
		/* TODO : set sighandler here */
		/* Ensure the lamp is turned off when done */
		leff_off (cfg->lamp);
		lamp_leff2_free (cfg->lamp);
	}
	task_exit ();
}


/** Start a lamp countdown effect.  LAMP says which lamp should
flash to reflect the state of a timer.  TIMER is a pointer to the
byte variable that contains the current time (counting towards
zero) in seconds.  The lamp is flashed at an increasing rate as 
the timer approaches expiration. */
void lamp_countdown (const U8 lamp, U8 * const timer)
{
	struct countdown_leff_config *cfg;
	task_pid_t tp;

	/* Create the countdown task */
	tp = task_create_anon (lamp_countdown_task);

	/* Initialize its class data with the configuration */
	cfg = task_init_class_data (tp, struct countdown_leff_config);
	cfg->lamp = lamp;
	cfg->timer = timer;
}

