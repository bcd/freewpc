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

/* All CPUs support sections for now */
#define HAVE_NVRAM_SECTION
#define HAVE_LOCAL_SECTION


/** noreturn is a standard GCC attribute and is always
 * available.  This is just shorthand. */
#define __noreturn__ __attribute__((noreturn))

/* The remaining attributes are gcc6809 specific and may
 * or may not be available depending on the compiler
 * version used. */

#ifdef HAVE_FASTRAM_ATTRIBUTE
#define __fastram__ __attribute__((section("direct")))
#else
#define __fastram__
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
#define __system__		__attribute__((section(".text")))
#define __common__		__far__(C_STRING(COMMON_PAGE))
#define __event__		   __far__(C_STRING(EVENT_PAGE))
#define __transition__	__far__(C_STRING(TRANS_PAGE))
#define __test__			__far__(C_STRING(TEST_PAGE))
#define __test2__			__far__(C_STRING(TEST2_PAGE))
#define __machine__		__far__(C_STRING(MACHINE_PAGE))
#define __effect__      __far__(C_STRING(EFFECT_PAGE))
#else
#define __far__(x)
#define __system__
#define __common__
#define __event__
#define __transition__
#define __test__
#define __test2__
#define __machine__
#define __effect__
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
to force use of the 'a' register. */
#ifdef __m6809__
#define __areg__    asm ("a")
#else
#define __areg__
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
#define barrier() asm ("; nop" ::: "memory")


/* The VOIDCALL macros let you call a function without declaring
a prototype. */

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


/***************************************************************
 * I/O accessor functions
 *
 * The intent of these functions is to encapsulate all I/O
 * reads and writes, so that they can be simulated in
 * environments where a direct memory map is not present.
 ***************************************************************/

extern inline void wpc_asic_write (U16 addr, U8 val)
{
#ifdef CONFIG_NATIVE
	extern void linux_asic_write (U16 addr, U8 val);
	linux_asic_write (addr, val);
#else
	*(volatile U8 *)addr = val;
#endif
}

extern inline U8 wpc_asic_read (U16 addr)
{
#ifdef CONFIG_NATIVE
	extern U8 linux_asic_read (U16 addr);
	return linux_asic_read (addr);
#else
	return *(volatile U8 *)addr;
#endif
}

extern inline void wpc_asic_xor (U16 addr, U8 val)
{
#ifdef CONFIG_NATIVE
	U8 reg = wpc_asic_read (addr);
	reg ^= val;
	wpc_asic_write (addr, val);
#else
	*(volatile U8 *)addr ^= val;
#endif
}

extern inline void wpc_asic_setbits (U16 addr, U8 val)
{
#ifdef CONFIG_NATIVE
	U8 reg = wpc_asic_read (addr);
	reg |= val;
	wpc_asic_write (addr, val);
#else
	*(volatile U8 *)addr |= val;
#endif
}

extern inline void wpc_asic_clearbits (U16 addr, U8 val)
{
#ifdef CONFIG_NATIVE
	U8 reg = wpc_asic_read (addr);
	reg &= ~val;
	wpc_asic_write (addr, val);
#else
	*(volatile U8 *)addr &= ~val;
#endif
}

/* TODO - rename this so that 'wpc' is not in the name */
#define writeb wpc_asic_write
#define readb wpc_asic_read


#endif /* _ENV_H */
