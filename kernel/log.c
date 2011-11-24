/*
 * Copyright 2008, 2009, 2010, 2011 by Brian Dominy <brian@oddchange.com>
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

/** The last CALLSET_ENTRY that was invoked */
__permanent__ U16 log_callset;
__permanent__ U16 prev_log_callset;

#ifdef DEBUG_LOG

/** The offset in the buffer of the next event to be read */
U8 log_head;

/** The offset in the buffer of the next slot for an event to be written */
U8 log_tail;

/** An array of log entries */
struct log_event log_entry[MAX_LOG_EVENTS];

char *log_module_names[] = {
	[MOD_DEFF] = "Deff",
	[MOD_LAMP] = "Lamp",
	[MOD_SOUND] = "Sound",
	[MOD_TASK] = "Task",
	[MOD_SWITCH] = "Sw",
	[MOD_TRIAC] = "Triac",
	[MOD_GAME] = "Game",
	[MOD_SYSTEM] = "Sys",
	[MOD_PRICING] = "Pricing",
	[MOD_SOL] = "Sol",
};

char *log_get_format (U16 module_event)
{
	switch (module_event & 0xF)
	{
		case EV_START:
			return "start";
		case EV_STOP:
			return "stop";
		case EV_EXIT:
			return "exit";
	}

	switch (module_event)
	{
		case make_module_event(MOD_LAMP, EV_BIT_ON):
			return "flag on";
		case make_module_event(MOD_LAMP, EV_BIT_OFF):
			return "flag off";
		case make_module_event(MOD_LAMP, EV_BIT_TOGGLE):
			return "flag toggle";

		case make_module_event(MOD_TASK, EV_TASK_RESTART):
			return "restart";
		case make_module_event(MOD_TASK, EV_TASK_START1):
			return "start1";

		case make_module_event(MOD_SYSTEM, EV_SYSTEM_NONFATAL):
			return "nonfatal";

		case make_module_event(MOD_SYSTEM, EV_SYSTEM_FATAL):
			return "fatal";

		case make_module_event(MOD_SOL, EV_DEV_ENTER):
			return "deventer";
		case make_module_event(MOD_SOL, EV_DEV_KICK):
			return "devkick";
	}

	return "?";
}
#endif /* DEBUG_LOG */


/** Add an entry to the event log. */
#ifdef CONFIG_LOG
void log_event1 (U16 module_event, U8 arg)
{
	struct log_event *ev;

	/* TODO : If logging has been disabled for this type of event,
	 * stop now. */

	ev = &log_entry[log_tail];

	/* The timestamp is stored as the number of ticks since
	the last event. */

	/* Save the event data. */
	ev->timestamp = 0;
	ev->module_event = module_event;
	ev->arg = arg;

	/* Advance the log forward.  When it reaches the end, we always
	 * wrap around, overwriting previous entries. */
	log_tail++;
	if (log_tail >= MAX_LOG_EVENTS)
	{
		log_tail = 0;
	}

	/* TODO : See if a breakpoint has been set on the module_event.  This halts
	all user task scheduling and enters the builtin debugger until
	it is exited. */

	/* If the builtin debugger is present (in simulation),
	write a message as well.  This replaces the older printfs
	that were scattered through the code. */
#ifdef DEBUG_LOG
	dbprintf ("%s %s %02X\n", log_module_names[module_part(module_event)],
		log_get_format (module_event), arg);
#endif
}
#endif /* CONFIG_LOG */


/** Initialize the event log. */
void log_init (void)
{
#ifdef CONFIG_LOG
	log_head = log_tail = 0;
#endif

	/* Save the last event logged from the previous run. */
	prev_log_callset = log_callset;
	dbprintf ("Last event: %04lX\n", prev_log_callset);
}

