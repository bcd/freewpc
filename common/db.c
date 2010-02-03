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

void bpt_hit (void);

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


/**
 * Toggle the system pause.
 */
void db_toggle_pause (void)
{
	db_paused = 1 - db_paused;
	barrier ();
}


/** Check for debug input periodically */
void db_periodic (void)
{
	extern void MACHINE_DEBUGGER_HOOK (U8);

#ifdef CONFIG_BPT
	if (!in_test && switch_poll (SW_ESCAPE))
		bpt_hit ();
#endif

#ifdef DEBUGGER
	if (wpc_debug_read_ready ())
	{
		char c = wpc_debug_read ();
		db_puts = db_puts_orkin;
		switch (c)
		{
			case 'a':
				/* Dump all debugging information */
				db_dump_all ();
				break;

			case 'p':
				/* Stop the system */
				bpt_hit ();
				break;

			default:
#ifdef MACHINE_DEBUGGER_HOOK
				/* Allow the machine to define additional commands.
				 * This function must reside in the system page. */
				MACHINE_DEBUGGER_HOOK (c);
#endif
				break;
		}
	}
#endif /* DEBUGGER */
}


/**
 * Debounce a button press, waiting for it to clear.
 */
void key_debounce (U8 sw)
{
	U16 x;
	for (x=0x1000; x ; --x)
		task_runs_long ();
	while (switch_poll (sw))
		switch_idle ();
}

/**
 * Handle a breakpoint.  The system is stopped until the user forces it
 * to continue, either by pressing 'p' in the debug console, or presses
 * the Escape Button.  Interrupt-level functions continue to run while
 * paused; only regular task scheduling is paused.  In order to poll for
 * the continue, we have to invoke the switch and debugger periodic
 * functions.
 */
void bpt_hit (void)
{
	db_toggle_pause ();
	key_debounce (SW_ESCAPE);
	while (db_paused == 1)
	{
		if (switch_poll (SW_ESCAPE))
		{
			key_debounce (SW_ESCAPE);
			dbprintf ("Debug: Exit\n");
			/* Escape = exit debugger */
			db_toggle_pause ();
		}
		else if (switch_poll (SW_ENTER))
		{
			key_debounce (SW_ENTER);
			dbprintf ("Debug: Enter\n");
			/* Enter = change active field */
		}
		else if (switch_poll (SW_UP))
		{
			key_debounce (SW_UP);
			dbprintf ("Debug: Up\n");
			/* Up = increase field value */
		}
		else if (switch_poll (SW_DOWN))
		{
			key_debounce (SW_DOWN);
			dbprintf ("Debug: Down\n");
			/* Down = decrease field value */
		}
		else
		{
			switch_idle ();
			db_periodic ();
			task_runs_long ();
		}
	}
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

