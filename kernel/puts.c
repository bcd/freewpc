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
 * \brief Debug output driver
 *
 * This module writes out debug messages (calls to dbprintf).
 * By default, these are sent to the parallel port, which works
 * in all versions of pinmame.
 *
 * If the Linux 'wpcdebug' program is running, messages are sent
 * to it instead, over the WPC debug port.
 *
 * On real hardware, there is no mechanism at present to see these
 * messages at all.
 */
#include <freewpc.h>
#ifdef __m6809__
#include <m6809/math.h>
#else
#include <native/math.h>
#endif

/** A pointer to the current output driver.  This must be
initialized during startup before debug messages can be written.
See db_init(). */
void (*puts_handler) (const char *s);


/** Writes a constant string to the debugger port */
#ifdef PINIO_HAVE_DEBUG_PORT
void puts_debug (const char *s)
{
	register U8 c;
	while ((c = *s++) != '\0')
		pinio_debug_write (c);
}
#endif


/** Writes a constant string to the parallel port */
#ifdef HAVE_PARALLEL_PORT
void puts_parallel (const char *s)
{
	register U8 c;
	while ((c = *s++) != '\0')
		pinio_parport_write (c);
}
#endif

