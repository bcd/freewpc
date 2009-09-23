/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
 * \brief Simple debugger interface
 *
 * This module reads/writes to the WPC debugger port.  A simple
 * companion utility has been written to display these messages,
 * and to pass input keystrokes to the running program.  This
 * requires a patched version of pinmame.  On real hardware, these
 * functions are not required.
 *
 * Define DEBUGGER if you want to compile in debugger support.
 * Without it, the program will run slightly faster, and be
 * considerably smaller.
 *
 * Also, you can now define CONFIG_PARALLEL_DEBUG if you want the
 * debug output to go out the parallel port instead.
 */
#include <freewpc.h>


/** Nonzero when the system (all tasks except for interrupts)
is paused */
U8 db_paused;

#ifndef CONFIG_INSPECTOR
void inspector_deff (void) {}
#endif


/** Read a character from the debug port, and wait for it if
 * it isn't there yet. */
U8 db_read_sync (void)
{
	while (!wpc_debug_read_ready ())
	{
		task_dispatching_ok = TRUE;
	}
	return wpc_debug_read ();
}

#ifdef DEBUGGER
void db_dump_all (void)
{
	VOIDCALL (task_dump);
	VOIDCALL (dump_game);
	VOIDCALL (dump_deffs);
	switch_queue_dump ();
	VOIDCALL (sol_req_dump);
	VOIDCALL (triac_dump);
	SECTION_VOIDCALL (__common__, device_debug_all);
}
#endif


/** Check for debug input at idle time */
void db_idle (void)
{
#ifdef DEBUGGER
	{
		if (wpc_debug_read_ready ())
		{
			char c = wpc_debug_read ();
			switch (c)
			{
				case 'a':
					/* Dump everything */
					db_dump_all ();
					break;

				case 'm':
					/* Dump the multiball devices */
					SECTION_VOIDCALL (__common__, device_debug);
					break;

				case 't':
					/* Dump the task table */
					VOIDCALL (task_dump);
					break;

				case 'g':
					/* Dump the game state */
					VOIDCALL (dump_game);
					break;

				case 'd':
					/* Dump the running/queued display effects */
					VOIDCALL (dump_deffs);
					break;

				case 'q':
					/* Dump the switch queue */
					switch_queue_dump ();
					break;

				case 'S':
					/* Dump the solenoid request queue */
					VOIDCALL (sol_req_dump);
					break;

				case 'T':
					/* Dump the triac states */
					VOIDCALL (triac_dump);
					break;

				case 's':
				{
					/* Simulate a switch closure.  The switch column/row must be given
					in ASCII.  This completes bypassing the real switch matrix and
					just calls the handler. */
					U8 row, col, sw;
					task_pid_t tp;

					row = db_read_sync ();
					col = db_read_sync ();
					sw = MAKE_SWITCH (col - '0', row - '0');

					tp = task_create_gid (GID_SW_HANDLER, switch_sched_task);
					task_set_arg (tp, sw);
				}

				case 'p':
				{
					/* Toggle the pause state.  When paused, tasks
					do not run and the system polls for debugger
					commands in a hard loop. */
#ifdef CONFIG_NATIVE
					extern char linux_interface_readchar (void);
					while (linux_interface_readchar () != 'p')
					{
						task_sleep (TIME_16MS);
					}
#else
					db_paused = 1 - db_paused;
					while (db_paused == 1)
					{
						task_dispatching_ok = TRUE;
						db_idle ();
					}
#endif
					break;
				}

				default:
				{
#ifdef MACHINE_DEBUGGER_HOOK
					/* Allow the machine to define additional commands.
					 * This function must reside in the system page. */
					extern void MACHINE_DEBUGGER_HOOK (U8);
					MACHINE_DEBUGGER_HOOK (c);
#endif
					break;
				}
			}
		}
	}
#endif /* DEBUGGER */
}


/** Initialize the debugger */
void db_init (void)
{
	db_paused = 0;

#ifdef DEBUGGER
	/* Signal the debugger that the system has just reset. */
	wpc_debug_write (0);

	/* Announce myself to the world. */
	dbprintf ("FreeWPC\n");
	dbprintf ("System Version %s.%s\n",
		C_STRING (MACHINE_MAJOR_VERSION), C_STRING (MACHINE_MINOR_VERSION));
	dbprintf ("%s\n\n", MACHINE_NAME);
#endif /* DEBUGGER */
}

