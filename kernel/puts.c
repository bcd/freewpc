/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
#else
#include <native/math.h>
#endif


/** Writes a constant string to the debugger port */
void db_puts (const char *s)
{
#ifdef DEBUGGER
	register U8 c;

	while ((c = *s++) != '\0')
	{
#ifdef CONFIG_PARALLEL_DEBUG
		pinio_parport_write (c);
#else
		wpc_debug_write (c);
#endif
	}
#endif /* DEBUGGER */
}

