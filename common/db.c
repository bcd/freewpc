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
#ifdef __m6809__
#include <m6809/math.h>
#endif

/** Initially zero, this is set to 1 when a remote debugger is
 * detected */
U8 db_attached;

U8 db_paused;

#ifdef DEBUGGER

void db_puts (const char *s)
{
	if (db_attached)
	{
		while (*s)
		{
#ifdef CONFIG_PARALLEL_DEBUG
			wpc_parport_write (*s++);
#else
			wpc_debug_write (*s++);
#endif
		}
	}
}

#endif /* DEBUGGER */


void db_idle (void)
{
#ifdef DEBUGGER
	if (!db_attached)
	{
		if (wpc_debug_read_ready ())
		{
			wpc_debug_write (0);
			db_attached = 1;
			db_puts ("\n\n"
				"----------------------------------------------------------\n"
				"FREEWPC DEBUGGER\n"
				"----------------------------------------------------------\n\n");
		}
	}
	else
	{
		if (wpc_debug_read_ready ())
		{
			char c = wpc_debug_read (); 
			switch (c)
			{
				case 'a':
				{
					extern __common__ void audio_dump (void);
					audio_dump ();
					break;
				}

				case 't':
				{
					extern void task_dump (void);
					task_dump ();
					break;
				}

				case 'm':
				{
					slow_timer_dump ();
					break;
				}

				case 'g':
				{
					extern void dump_game (void);
					dump_game ();
					break;
				}

				case 's':
				{
					extern void switch_check_masks (void);
					switch_check_masks ();
					break;
				}

#ifdef MACHINE_TZ
				case 'c':
				{
					extern void tz_dump_clock (void);
					tz_dump_clock ();
					break;
				}
#endif

				case 'p':
				{
					db_paused = 1 - db_paused;
					while (db_paused == 1)
					{
						task_dispatching_ok = TRUE;
						db_idle ();
					}
					break;
				}
			}
		}
	}
#endif /* DEBUGGER */
}


void db_init (void)
{
#ifdef CONFIG_PLATFORM_LINUX
	db_attached = 1;
#else
#ifdef CONFIG_PARALLEL_DEBUG
	db_attached = 1;
#else
	db_attached = 0;
#endif
#endif
	db_paused = 0;
}

