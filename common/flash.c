/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Flashlamp effects
 */

#include <freewpc.h>

struct flasher_action
{
	task_ticks_t pulse_time;
	U8 duty;
	task_ticks_t delay_time;
};

#define END_FLASHER_ACTION { 0, 0, 0 }

#define END_FLASHER_SET 0xff

#define ONTIME(x)  (4 * (x))
#define OFFTIME(x) (x)

struct flasher_action flasher_rapid_action[] =
{
	{ ONTIME(TIME_66MS), SOL_DUTY_25, OFFTIME(TIME_166MS) },
	END_FLASHER_ACTION
};


struct flasher_action flasher_pulsating_action[] =
{
	{ ONTIME(TIME_200MS), SOL_DUTY_12, OFFTIME(TIME_100MS) },
	{ ONTIME(TIME_200MS), SOL_DUTY_25, OFFTIME(TIME_100MS) },
	{ ONTIME(TIME_200MS), SOL_DUTY_50, OFFTIME(TIME_100MS) },
	{ ONTIME(TIME_200MS), SOL_DUTY_25, OFFTIME(TIME_100MS) },
	{ ONTIME(TIME_200MS), SOL_DUTY_12, OFFTIME(TIME_100MS) },
	END_FLASHER_ACTION
};


/**
 * Execute a single flasher action.
 */
void flasher_act (U8 id, struct flasher_action *action)
{
	while (action->pulse_time != 0)
	{
		sol_start_real (id, action->duty, action->pulse_time);
		task_sleep (action->delay_time);
		action++;
	}
}


/**
 * Execute an action on a set of flashers.
 */
void flasher_set_act (U8 *idlist, struct flasher_action *action)
{
	while (action->pulse_time != 0)
	{
		U8 *ids = idlist;
		do {
			sol_start_real (*ids++, action->duty, action->pulse_time);
		} while (*ids != END_FLASHER_SET);
		task_sleep (action->delay_time);
		action++;
	}
}


/**
 * Execute a single flasher action multiple times.
 */
void flasher_repeat_act (U8 id, struct flasher_action *action, U16 repeat_count)
{
	do {
		flasher_act (id, action);
	} while (--repeat_count > 0);
}


/**
 * Randomly pulse all flashers for the given duration, with the given delay
 * in between each pulse.
 */
void flasher_randomize (task_ticks_t delay, U16 duration)
{
	U8 id;
	duration *= TIME_1S;
	while (duration > 0)
	{
		do {
			id = random_scaled (PINIO_NUM_SOLS);
		} while (!MACHINE_SOL_FLASHERP (id));
		flasher_pulse (id);
		task_sleep (delay);
		duration -= delay;
	}
}


#if 0
void flash_test_task (void)
{
	flasher_act (FLASH_GOAL, flasher_pulsating_action);
	task_exit ();
}
#endif
