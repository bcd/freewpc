/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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
 * \brief Manage the knocker.
 */

#include <freewpc.h>

/* For sanity, don't allow more than this many knocks to queue up */
#define MAX_PENDING_KNOCKS 5

/** The number of pending knocks */
U8 knock_count;


/** Fires the knocker */
static void knocker_fire_task (void)
{
	task_sleep (TIME_500MS);
	do
	{
#ifdef MACHINE_KNOCKER_SOLENOID
		sol_request (MACHINE_KNOCKER_SOLENOID);
#endif
#ifdef MACHINE_KNOCKER_SOUND
		sound_send (MACHINE_KNOCKER_SOUND);
#endif
		task_sleep (TIME_500MS);
	} while (--knock_count > 0);
	task_exit ();
}


/** Requests that the knocker be fired.
 * We bump a count and then let a background task do the firing,
 * taking care to pause between thwacks.
 *
 * Also, if the knocker is being used for coin metering, then don't
 * use it for free play awards. */
void knocker_fire (void)
{
	if ((knock_count < MAX_PENDING_KNOCKS) /* prevent runaway knocking */
		&& (price_config.coin_meter_units == 0))
	{
		knock_count++;
		task_create_gid1 (GID_KNOCKER_FIRE, knocker_fire_task);
	}
}


CALLSET_ENTRY (knocker, init)
{
	knock_count = 0;
}

