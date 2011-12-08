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

#ifndef _CALLSET_H
#define _CALLSET_H

#ifndef GENCALLSET

#define CALLSET_ENTRY(module,set,...) \
	void module ## _ ## set (void)

#define CALLSET_BOOL_ENTRY(module,set) \
	bool module ## _ ## set (void)

#define callset_invoke(set)	SECTION_VOIDCALL(__event__, callset_ ## set)

#define callset_invoke_boolean(set)	\
({ \
	extern __event__ bool callset_ ## set (void); \
	callset_ ## set (); \
})

/* WARNING : this function won't work if the caller is in a different page
from EVENT_PAGE. */
#define callset_pointer_invoke(callset_ptr)	call_far (EVENT_PAGE, (*callset_ptr) ())

/*
 * gencallset emits a callset_debug() before each event receiver is called.
 * The default is for this to do nothing, but sometimes for debugging it
 * is nice to have this print a message or log the event.
 */

#ifdef __m6809__
#define callset_debug(id) \
	do { \
		extern U16 log_callset; \
		if ((id & 0x3F) == 0) \
			log_callset = id; \
		else \
			asm ("inc\t_log_callset+1"); \
	} while (0)
#else
#define callset_debug(id) do { extern U16 log_callset; log_callset = id; } while (0)
#endif

#endif /* GENCALLSET */

#endif /* _CALLSET_H */
