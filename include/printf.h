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

#ifndef _PRINTF_H
#define _PRINTF_H

/* TODO : much of this is implementing a 'varargs' type facility.
 * Split that into a separate header. */

/** va_list is just a byte pointer onto the stack */
typedef U8 *va_list;

/* Ensure that the system versions of the va functions are not being used. */
#undef va_start
#undef va_arg
#undef va_end

/** Begins a variable argument list access */
#define va_start(va, fmt) \
do { \
	va = (va_list)((unsigned char *)&fmt + sizeof (fmt)); \
} while (0) \

/** Access the next argument in the va_list 'va' with type 'type'. */
#ifdef CONFIG_NATIVE
#define va_arg(va, type)	((va += sizeof (int)), (type)(((int *)va)[-1]))
#else
#define va_arg(va, type)	((va += sizeof (type)), ((type *)va)[-1])
#endif

/** Ends a variable argument list access.  Nothing required. */
#define va_end(va)



/** The size of the single print buffer */
#define PRINTF_BUFFER_SIZE		48

/** The name of the single print buffer */
extern char sprintf_buffer[PRINTF_BUFFER_SIZE];

#ifdef CONFIG_NATIVE
#undef sprintf
#define sprintf freewpc_sprintf
void freewpc_sprintf (const char *format, ...);
#else
#define printf printf_is_bad
void sprintf (const char *format, ...);
#endif
void sprintf_far_string (const char **srcp);
void sprintf_score (const U8 *score);
void dbprintf1 (void);
void message_write (const char *msg, U8 page);

#define sprintf_current_score() sprintf_score (current_score)

/** psprintf() is like sprintf() but it has TWO format control
 * strings.  The first is used when the value is singular, and
 * the second when it is plural.  You can only printf a single
 * value at a time this way. */
#define psprintf(fmt1, fmtn, val) \
do { \
	if (val == 1) \
		sprintf (fmt1, val); \
	else \
		sprintf (fmtn, val); \
} while (0)


#ifdef DEBUGGER
#define dbprintf(format, rest...) \
	do { \
		sprintf (format, ## rest ); \
		dbprintf1 (); \
	} while (0)
#else
#define dbprintf(format, rest...)
#endif /* DEBUGGER */

/**
 * interrupt_dbprintf() can be used in IRQ handlers.
 * This is never safe to do on real hardware, but it can be useful
 * for debugging in simuation.
 */
#ifdef CONFIG_NATIVE
#define interrupt_dbprintf dbprintf
#else
#define interrupt_dbprintf(format, rest...)
#endif

#endif /* _PRINTF_H */
