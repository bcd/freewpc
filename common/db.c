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
 */
#include <freewpc.h>


/** Nonzero when the system (all tasks except for interrupts)
is paused */
U8 db_paused;

#ifndef CONFIG_INSPECTOR
void inspector_deff (void) { deff_exit (); }
#endif


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


/** Check for debug input periodically */
void db_periodic (void)
{
#ifdef DEBUGGER
	{
		if (wpc_debug_read_ready ())
		{
			char c = wpc_debug_read ();
			db_puts = db_puts_orkin;
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
						task_runs_long ();
						db_periodic ();
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
	if (wpc_debug_read_ready ())
	{
		wpc_debug_write (0);
		db_puts = db_puts_orkin;
	}
	else
	{
		db_puts = db_puts_parallel;
	}

	/* Announce myself to the world. */
	dbprintf ("FreeWPC\n");
	dbprintf ("System Version %s.%s\n",
		C_STRING (MACHINE_MAJOR_VERSION), C_STRING (MACHINE_MINOR_VERSION));
	dbprintf ("%s\n\n", MACHINE_NAME);
#endif /* DEBUGGER */
}

