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

U8 knock_count;


/** Fires the knocker */
static void knocker_fire_task (void)
{
	do
	{
#ifdef MACHINE_KNOCKER_SOLENOID
		sol_pulse (MACHINE_KNOCKER_SOLENOID);
#endif
		task_sleep (TIME_500MS);
	} while (--knock_count > 0);
}


/** Requests that the knocker be fired. */
void knocker_fire (void)
{
	/* TODO - don't fire knocker in certain instances */
	knock_count++;
	task_create_gid1 (GID_KNOCKER_FIRE, knocker_fire_task);
}


void knocker_init (void)
{
	knock_count = 0;
}

