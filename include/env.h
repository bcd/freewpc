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
 * \brief Define many macros based on the machine, platform, and simulation options.
 */

#ifndef _ENV_H
#define _ENV_H

/* Declare that certain things exist.  This is true of the 6809
 * but may change later as we port to different architectures. */
#ifdef __m6809__
#define HAVE_PAGING
#define HAVE_INTERRUPT_ATTRIBUTE
#endif

/** noreturn is a standard GCC attribute and is always
 * available.  This is just shorthand. */
#define __noreturn__ __attribute__((noreturn))

/* The remaining attributes are gcc6809 specific and may
 * or may not be available depending on the compiler
 * version used. */

#define __fastram__ __attribute__((section("direct")))
#define __nvram__ __attribute__((section ("nvram")))
#define __local__ __attribute__((section ("local")))
#define __permanent__ __attribute__((section ("permanent")))

/** Section declaration modifiers.  These attributes are used
 * on function prototypes and data externs to denote which
 * page of ROM the definitions will be found in.  These are
 * not used on the definitions themselves, as they would have
 * no effect; only by configuring the linker correctly will that
 * happen.  Take care to use these correctly to match what's being
 * done in the Makefile.
 */
#ifdef HAVE_PAGING
#define __far__(x)      __attribute__((far(x)))
#define __common__		__far__(C_STRING(COMMON_PAGE))
#define __common2__		__far__(C_STRING(COMMON2_PAGE))
#define __event__		   __far__(C_STRING(EVENT_PAGE))
#define __transition__	__far__(C_STRING(TRANS_PAGE))
#define __test__			__far__(C_STRING(TEST_PAGE))
#define __test2__			__far__(C_STRING(TEST2_PAGE))
#define __machine__		__far__(C_STRING(MACHINE_PAGE))
#define __effect__      __far__(C_STRING(EFFECT_PAGE))
#define __init__        __far__(C_STRING(INIT_PAGE))
#define __machine2__    __far__(C_STRING(MACHINE2_PAGE))
#define __machine3__    __far__(C_STRING(MACHINE3_PAGE))
#define __machine3__    __far__(C_STRING(MACHINE3_PAGE))
#define __machine4__    __far__(C_STRING(MACHINE4_PAGE))
#define __machine5__    __far__(C_STRING(MACHINE5_PAGE))
#define __deff__        __far__(C_STRING(DEFF_PAGE))
#define __leff__        __far__(C_STRING(DEFF_PAGE))
#else
#define __far__(x)
#define __common__
#define __common2__
#define __event__
#define __transition__
#define __test__
#define __test2__
#define __machine__
#define __effect__
#define __init__
#define __machine2__
#define __machine3__
#define __machine4__
#define __machine5__
#define __deff__
#define __leff__
#endif

#ifndef __attribute_deprecated__
#ifndef CONFIG_NO_DEPRECATED
#define __attribute_deprecated__ __attribute__((__deprecated__))
#else
#define __attribute_deprecated__
#endif
#endif

#ifndef CONFIG_NO_PURE
#define __pure__ __attribute__((pure))
#else
#define __pure__
#endif

#ifdef HAVE_INTERRUPT_ATTRIBUTE
#define __interrupt__ __attribute__((interrupt))
#else
#define __interrupt__
#endif

#ifdef __m6809__
#define __naked__       __attribute__((naked))
#else
#define __naked__
#endif

/* GCC6809 won't allocate the 'a' register, but we can sometimes
generate more efficient code if 'a' is used.  In these cases,
use __areg__ after the declaration (which must begin with 'register')
to force use of the 'a' register.  Likewise for other 6809-only registers. */
#ifdef __m6809__
#define __areg__    asm ("a")
#define __m1reg__   asm ("m1")
#else
#define __areg__
#define __m1reg__
#endif


/* noop() invokes an actual no-op instruction in assembly on
the target CPU */
#ifdef __m6809__
#define noop() asm ("nop" ::: "memory")
#else
#define noop()
#endif


/* barrier() is used in several places to tell the compiler not
to perform certain optimizations across both sides of the barrier. */
#ifdef __m6809__
#define barrier() asm ("; barrier" ::: "memory")
#else
#define barrier()
#endif

/* Some versions of the 6809 C compiler have issues.
 * Use __GCC6809_AT_LEAST__ to test for a minimum version. */
#define VERSION_CODE(major, minor, patchlevel) \
	((major * 10000UL) + (minor * 100UL) + patchlevel)

#define __GCC_VERSION_CODE__ \
	VERSION_CODE(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)

#ifdef __m6809__
#define __GCC6809_AT_LEAST__(major, minor, patchlevel) \
	(__GCC_VERSION_CODE__ >= VERSION_CODE(major, minor, patchlevel))
#else
#define __GCC6809_AT_LEAST__(major, minor, patchlevel) 1
#endif


/* The VOIDCALL macros let you call a function without declaring
a prototype. */

#define SECTION_VOIDCALL(section,fn) \
do { \
	extern section void fn (void); \
	fn (); \
} while (0)

#define VOIDCALL(fn) \
do { \
	extern void fn (void); \
	fn (); \
} while (0)


/* Far data accessor functions
 *
 * Inside any banked function which needs to access caller data
 * that might be in a different bank, first call far_read_access()
 * as the very first statement in the function.  Thereafter, you
 * can use far_read() to get access to caller data through a paged
 * pointer.
 *
 * The syntax is far_read (ptr, field), where ptr is a pointer
 * to a struct in a banked area, and field identifies which part
 * of the struct needs to be read.
 */

#define __fardata__

#ifdef __m6809__
#define far_read_access() \
	U8 __caller_page; asm ("sta\t%0" : "=m"(__caller_page))
#else
#define far_read_access()
#endif

#ifdef __m6809__
#define far_read(ptr, __field) \
	(__builtin_choose_expr ( (sizeof (ptr->__field) == 1), \
		(far_read8 (&ptr->__field, __caller_page)), \
		(typeof (ptr->__field))(far_read16 (&ptr->__field, __caller_page))))
#else
#define far_read(__ptr, __field) (__ptr->__field)
#endif

/* Types for linker variables.
   You can declare flags, global flags, and timers using the types below.
   They are resolved to IDs at link time.  Each type is associated with
   a pseudo-section. */
#define flag_t          __attribute__((section(".flag"))) U8
#define global_flag_t   __attribute__((section(".globalflag"))) U8
#define free_timer_id_t __attribute__((section(".freetimer"))) U8

#endif /* _ENV_H */
