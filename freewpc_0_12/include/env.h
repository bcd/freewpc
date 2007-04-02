/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

/* Declare that certain things exist.  This is true of the 6809
 * but may change later as we port to different architectures. */
#ifdef CONFIG_PLATFORM_WPC
#define HAVE_NVRAM_SECTION
#define HAVE_LOCAL_SECTION
#define HAVE_PAGING
#define HAVE_INTERRUPT_ATTRIBUTE
#endif

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

#ifdef HAVE_NVRAM_SECTION
#define __nvram__ __attribute__((section ("nvram")))
#else
#define __nvram__
#endif

#ifdef HAVE_LOCAL_SECTION
#define __local__ __attribute__((section ("local")))
#else
#define __local__
#endif

#ifdef HAVE_PAGING
#define __far__(x)      __attribute__((far(x)))
#define __system__		__attribute__((section("sysrom")))
#define __common__		__far__(C_STRING(COMMON_PAGE))
#define __event__		   __far__(C_STRING(EVENT_PAGE))
#define __transition__	__far__(C_STRING(TRANS_PAGE))
#define __test__			__far__(C_STRING(TEST_PAGE))
#define __machine__		__far__(C_STRING(MACHINE_PAGE))
#define __xbmprog__     __far__(C_STRING(PRG_PAGE))
#else
#define __far__(x)
#define __system__
#define __common__
#define __event__
#define __transition__
#define __test__
#define __machine__
#define __xbmprog__
#endif

#ifdef HAVE_INTERRUPT_ATTRIBUTE
#define __interrupt__ __attribute__((interrupt))
#else
#define __interrupt__
#endif

#ifdef CONFIG_PLATFORM_WPC
#define __naked__       __attribute__((naked))
#else
#define __naked__
#endif

#define SECTION_VOIDCALL(section,fn) \
{ \
	extern section void fn (void); \
	fn (); \
}

#define VOIDCALL(fn) \
{ \
	extern void fn (void); \
	fn (); \
}

#endif /* _ENV_H */
