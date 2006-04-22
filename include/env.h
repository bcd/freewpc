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

#ifndef _ENV_H
#define _ENV_H

/** noreturn is a standard GCC attribute and is always
 * available */
#define __noreturn__ __attribute__((__noreturn__))

/* The remaining attributes are gcc6809 specific and may
 * or may not be available depending on the compiler
 * version used. */

#ifdef HAVE_FASTRAM_ATTRIBUTE
#define __fastram__ __attribute__((section("direct")))
#else
#define __fastram__
#endif

#ifdef HAVE_TASKENTRY_ATTRIBUTE
#define __taskentry__ __attribute__((__taskentry__))
#else
#define __taskentry__
#endif

#define __nvram__ __attribute__((section ("nvram")))

#ifdef CONFIG_MULTIPLAYER
#define __local__ __attribute__((section ("local")))
#else
#define __local__
#endif

#define VOIDCALL(fn) \
{ \
	extern void fn (void); \
	fn (); \
}

#endif /* _ENV_H */
