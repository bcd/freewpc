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

U8 log_head;

U8 log_tail;

struct log_event log_entry[MAX_LOG_EVENTS];


/** Add an entry to the event log. */
void log_event1 (U16 module_event, U8 arg)
{
	struct log_event *ev = &log_entry[log_tail];

	/* The timestamp is stored as the number of ticks since
	the last event. */

	/* Save the event data. */
	ev->timestamp = 0;
	ev->module_event = module_event;
	ev->arg = arg;

	/* Advance the log forward */
	log_tail++;
	if (log_tail >= MAX_LOG_EVENTS)
	{
		log_tail = 0;
	}

	/* See if a breakpoint has been set on the module_event.  This halts
	all user task scheduling and enters the builtin debugger until
	it is exited. */
}


void log_init (void)
{
	log_head = log_tail = 0;
}

