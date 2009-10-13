/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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
#include <coin.h>

U8 effect_update_counter;

/* CALLSET_SECTION (effect_update, __effect__) */

static void update_complete (void)
{
	effect_update_counter = 5;
}

/**
 * A background task that calls all of the update functions.
 *
 * The order of the updates here is done from most important to
 * least important.  Note that if an update request is made while
 * this task is running, it can cause the task to be *restarted*, so
 * it is possible for the logic at the bottom of the function to
 * be starved temporarily.
 */
static void effect_update_task (void)
{
	/* Display and music are always updated, including during
	attract mode */
	deff_update ();
	music_refresh ();

	/* Lamp update is used for multiplexing different meanings
	to playfield lamps; this only makes sense in the context
	of a game. */
	if (in_live_game)
	{
		/* Sleep a bit to avoid starving other tasks */
		task_sleep (TIME_33MS);
		callset_invoke (lamp_update);
	}

	/* Update the start button lamp */
	if (!in_test)
		lamp_start_update ();

	task_exit ();
}


/**
 * Request that background effects be updated soon.
 *
 * Do not actually update right away!  Because of the way this is used,
 * it is possible for multiple requests to be made within a short
 * period of time.  In no case do we really need to do the update
 * more than once every 100ms.
 */
void effect_update_request (void)
{
	effect_update_counter = 0;
}


/**
 * Periodically check to see if effects need updating.
 */
CALLSET_ENTRY (effect_update, idle_every_100ms, start_ball, end_ball)
{
	if (!in_test)
	{
		/* Update devices frequently */
		if (in_game)
			/* TODO - don't device_update while ball_search is active */
			callset_invoke (device_update);

		/* Less frequently, update background display, music, and lamps.
		Normally this is done every 500ms.  If effect_update_request() is
		called, then it will occur on the next 100ms interval. */
		if (effect_update_counter == 0)
		{
			task_recreate_gid_while (GID_EFFECT_UPDATE, effect_update_task,
				TASK_DURATION_INF);
			update_complete ();
		}
		else
		{
			--effect_update_counter;
		}
	}
}

CALLSET_ENTRY (effect_update, init)
{
	update_complete ();
}


