/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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

extern __permanent__ bool new_fatal_error;

#ifdef CONFIG_BPT
/** Nonzero when the system (all tasks except for interrupts)
is paused */
U8 db_paused;

U8 *bpt_mem_addr;

U8 bpt_mem_value;

U16 bpt_debounce_timer;

U8 bpt_repeat_count;

U8 db_tilt_flag;
#endif


#ifdef DEBUGGER
void db_dump_all (void)
{
	VOIDCALL (task_dump);
	VOIDCALL (dump_game);
	VOIDCALL (dump_deffs);
	switch_queue_dump ();
	VOIDCALL (triac_dump);
	SECTION_VOIDCALL (__common__, device_debug_all);
}
#endif


#ifdef CONFIG_BPT
/**
 * Debounce a button press, waiting for it to clear.
 */
U8 button_check (U8 sw)
{
	volatile U16 x;

	if (!switch_poll (sw))
		return 0;

	for (x=bpt_debounce_timer; x ; --x)
	{
		task_runs_long ();
		barrier ();
	}
	switch_periodic ();
	if (!switch_poll (sw))
	{
		bpt_repeat_count = 0;
		bpt_debounce_timer = 0x1C00;
		return 1;
	}
	else
	{
		bpt_repeat_count++;
		if (bpt_repeat_count >= 32)
		{
			bpt_repeat_count = 32;
			bpt_debounce_timer = 0x80;
			return 8;
		}
		else if (bpt_repeat_count >= 4)
		{
			bpt_debounce_timer = 0x600;
			return 4;
		}
		else
			return 1;
	}
}

void bpt_display (void)
{
	dmd_alloc_low_clean ();

	sprintf ("%p", bpt_mem_addr);
	font_render_string_left (&font_bitmap8, 0, 0, sprintf_buffer);

	sprintf ("%02X %02X %02X %02X",
		bpt_mem_addr[0], bpt_mem_addr[1], bpt_mem_addr[2], bpt_mem_addr[3]);
	font_render_string_left (&font_bitmap8, 40, 0, sprintf_buffer);

	if (new_fatal_error)
	{
		sprintf ("ERR %d GID %d",
			(U8)system_audits.lockup1_addr, (U8)system_audits.lockup1_pid_lef);
		font_render_string_left (&font_bitmap8, 0, 8, sprintf_buffer);
	}
	else if (task_getpid ())
	{

		sprintf ("PID %p GID %d", task_getpid (), task_getgid ());
		font_render_string_left (&font_bitmap8, 0, 8, sprintf_buffer);
		sprintf ("%02X%02X %02X", bpt_addr[0], bpt_addr[1]-2, bpt_addr[2]);
		font_render_string_left (&font_bitmap8, 0, 16, sprintf_buffer);
	}
	else
	{
		font_render_string_left (&font_bitmap8, 0, 8, "BREAK");
	}

	sprintf ("C%04lX", prev_log_callset);
	font_render_string_left (&font_bitmap8, 0, 24, sprintf_buffer);
	dmd_show_low ();
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
	U8 key;

	db_paused = 1 - db_paused;
	if (db_paused == 1)
	{
		callset_invoke (debug_enter);
		db_tilt_flag = in_tilt;
		in_tilt = FALSE;
		bpt_display ();
	}
	else
	{
		in_tilt = db_tilt_flag;
		callset_invoke (debug_exit);
	}

	while (db_paused == 1)
	{
		if ((key = button_check (SW_ENTER)))
		{
			/* Enter = change active field */
			bpt_display ();
		}
		else if ((key = button_check (SW_UP)))
		{
			/* Up = increase field value */
			bpt_mem_addr += key * 4;
			bpt_mem_addr = (void *) (((U16)bpt_mem_addr) & 0x1FFFUL);
			bpt_display ();
		}
		else if ((key = button_check (SW_DOWN)))
		{
			/* Down = decrease field value */
			bpt_mem_addr -= key * 4;
			bpt_mem_addr = (void *) (((U16)bpt_mem_addr) & 0x1FFFUL);
			bpt_display ();
		}
		else
		{
			switch_periodic ();
			db_periodic ();
			task_runs_long ();
		}
	}
	dmd_alloc_low_clean ();
	dmd_show_low ();
}
#endif /* CONFIG_BPT */


/** Check for debug input periodically */
void db_periodic (void)
{
#ifdef CONFIG_BPT
	if (!in_test && button_check (SW_ESCAPE))
		bpt_stop ();
#endif

#ifdef DEBUGGER
	if (pinio_debug_read_ready ())
	{
		char c = pinio_debug_read ();
		puts_handler = puts_debug;
		switch (c)
		{
			case 'a':
				/* Dump all debugging information */
				db_dump_all ();
				break;

#ifdef CONFIG_BPT
			case 'p':
				/* Stop the system */
				bpt_stop ();
				break;
#endif

			default:
				break;
		}
	}
#endif /* DEBUGGER */
}


/** Initialize the debugger */
void db_init (void)
{
#ifdef CONFIG_BPT
	db_paused = 0;
	bpt_mem_addr = 0;
	bpt_debounce_timer = 0x1C00;
#endif

#ifdef DEBUGGER
	/* Signal the debugger that the system has just reset. */
	if (pinio_debug_read_ready ())
	{
		pinio_debug_write (0);
		puts_handler = puts_debug;
	}
	else
	{
		puts_handler = puts_parallel;
	}

	/* Announce myself to the world. */
	dbprintf ("FreeWPC\n");
	dbprintf ("System Version %s.%s\n",
		C_STRING (MACHINE_MAJOR_VERSION), C_STRING (MACHINE_MINOR_VERSION));
	dbprintf ("%s\n\n", MACHINE_NAME);
#endif /* DEBUGGER */
}

