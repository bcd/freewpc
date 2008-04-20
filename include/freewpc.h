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
 * \brief Top-level include file for FreeWPC
 *
 * All source files should include this one first in order to 
 * get all of the required definitions and a lot of the more 
 * commonly used features.
 */

#ifndef _FREEWPC_H
#define _FREEWPC_H

/* For C++ compilation.  Everything in the kernel is plain C. */
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Standard types
 */

/* The boolean type is normally just a 'char' but the 6809 compiler
 * supports an optimal form. */
#if defined(__m6809__) && defined(HAVE_BOOLEAN_ATTRIBUTE)
#define bool __boolean_bit_register
typedef unsigned char __boolean_bit_register __attribute__ ((boolean));
#else
typedef unsigned char bool;
#endif

/* 8-bit integers are always 'char' */
typedef unsigned char bcd_t;
typedef signed char int8_t, I8, S8;
typedef unsigned char U8;

/* 16-bit integers depend on the compiler */
#if defined(__m6809__) && defined(__int16__)
typedef int int16_t, I16, S16;
typedef unsigned int U16;
typedef U16 INTPTR;
typedef U16 PTR_OR_U16;
typedef unsigned long U32;
#elif defined(CONFIG_NATIVE)
typedef short int16_t, I16, S16;
typedef unsigned short U16;
typedef unsigned long PTR_OR_U16;
typedef unsigned long U32;
#else /* default assumes -mint8 on wpc */
typedef long int16_t, I16, S16;
typedef unsigned long U16;
typedef unsigned long long U32;
typedef unsigned int INTPTR;
typedef U16 PTR_OR_U16;
#endif

/* After this point, do not permit native types like
 * short, int, long to be used; use one of the types
 * defined above instead. */
#ifndef CONFIG_NATIVE
#define short short_not_supported
#define int int_not_supported
#define long long_not_supported
#endif

#ifndef __FIF /* skip remainder for fast compile */

/* TODO - move these elsewhere */
extern U8 sys_init_complete;
extern U8 sys_init_pending_tasks;
extern U8 irq_count;
extern U8 idle_ok;

/*
 * Define TRUE and FALSE to suitable values.
 */
#undef TRUE
#define TRUE (1)
#undef FALSE
#define FALSE (0)


/*
 * Define macros that let us print #defines which are strings.
 */
#define C_STRING(x)	C_STR(x)
#define C_STR(x)		#x

#ifdef CONFIG_NATIVE
#include <platform/linux.h> /* TODO : rename this file */
#endif

/* Include the standard header files that are needed
 * by most modules */

/* Main configuration - game specific.  All machines MUST define one */
#include <mach-config.h>

/* Processor specifics */
#ifdef __m6809__

#include <m6809/m6809.h>
#ifndef NULL
#define NULL 0UL
#endif

#else

#define __blockcopy16(s1,s2,n) memcpy(s1,s2,n)
#define __blockclear16(s,n) memset(s,'0',n)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)


/* Build system information */
#include <env.h>
#include <version.h>

/* Platform specifics */
#ifdef MACHINE_PLATFORM_WHITESTAR
#include <platform/whitestar.h>
#else
#include <platform/wpc.h>
#endif


/* Core software structures */
#include <sys/bitarray.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/task.h>
#if (MACHINE_DMD == 1)
#include <sys/font.h>
#endif

/* Basic data structures */

#include <misc.h>
#include <list.h>

/* Hardware modules */
#include <sys/ac.h>
#include <sys/sol.h>
#include <sys/lamp.h>
#include <sys/sound.h>
#include <sys/switch.h>
#include <sys/flip.h>
#if (MACHINE_DMD == 1)
#include <sys/dmd.h>
#endif
#include <sys/triac.h>
#include <sys/irq.h>
#include <sys/rtc.h>

/* Common software structures */
#include <sys/deff.h>
#include <sys/leff.h>
#include <sys/device.h>
#include <math.h>
#include <timer.h>
#include <score.h>
#include <game.h>
#include <stdadj.h>
#include <audit.h>
#include <printf.h>
#include <callset.h>
#include <priority.h>
#include <eb.h>
#include <search.h>
#include <replay.h>
#include <csum.h>
#include <audio.h>
#include <mode.h>

/* Uncommon software modules - TODO : shouldn't automatically include */
#include <sys/debug.h>
#include <test.h> /* this one HAS to be here for now, for callset.c */

/* Game-specific defines.  'mach' should point to the machine-specific 
 * directory.  These files are optional; if a machine does not need
 * one of these, it should define the corresponding flag.
 */
#ifndef MACHINE_LACKS_ALL_INCLUDES
#ifndef MACHINE_LACKS_SOUND_H
#include <mach/sound.h>
#endif
#ifndef MACHINE_LACKS_PROTOS_H
#include <mach/protos.h>
#endif
#endif /* MACHINE_LACKS_ALL_INCLUDES */

/* Automatically generated header files */
#include <gendefine_gid.h>


/* This is ugly, but I can't figure out any other way to get 
 * pragmas working */
#ifndef CONFIG_NATIVE
#ifdef PAGE
#if (PAGE == 54)
#define PAGE_PRAGMA _Pragma ("section (\"page54\")")
#elif (PAGE == 55)
#define PAGE_PRAGMA _Pragma ("section (\"page55\")")
#elif (PAGE == 56)
#define PAGE_PRAGMA _Pragma ("section (\"page56\")")
#elif (PAGE == 57)
#define PAGE_PRAGMA _Pragma ("section (\"page57\")")
#elif (PAGE == 58)
#define PAGE_PRAGMA _Pragma ("section (\"page58\")")
#elif (PAGE == 59)
#define PAGE_PRAGMA _Pragma ("section (\"page59\")")
#elif (PAGE == 60)
#define PAGE_PRAGMA _Pragma ("section (\"page60\")")
#elif (PAGE == 61)
#define PAGE_PRAGMA _Pragma ("section (\"page61\")")
#else
#define PAGE_PRAGMA
#endif
PAGE_PRAGMA
#endif
#endif /* !CONFIG_NATIVE */

#endif /* __FIF */

#ifdef NOSTATIC
#define static
#endif

#ifdef __cplusplus
} 
#endif /* extern "C" */

#endif /* _FREEWPC_H */
