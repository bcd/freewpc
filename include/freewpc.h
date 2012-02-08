/*
 * Copyright 2006-2012 by Brian Dominy <brian@oddchange.com>
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
typedef unsigned char __boolean __attribute__ ((boolean));
#else
typedef unsigned char __boolean;
#endif
#undef bool
#define bool __boolean

/* 8-bit integers are always 'char' */
typedef unsigned char bcd_t;
typedef signed char I8, S8;
typedef unsigned char U8;

/* 16-bit integers depend on the compiler */
#if defined(__m6809__) && defined(__int16__)
typedef int I16, S16;
typedef unsigned int U16;
typedef U16 INTPTR;
typedef unsigned long U32;
typedef U16 size_t;
#elif defined(CONFIG_NATIVE)
typedef short I16, S16;
typedef unsigned short U16;
typedef unsigned int U32;
#else /* default assumes -mint8 on wpc */
typedef long I16, S16;
typedef unsigned long U16;
typedef unsigned long long U32;
typedef unsigned int INTPTR;
typedef U16 size_t;
#endif

typedef U16 IOPTR;

typedef union
{
	void *ptr;
	U16 u16;
} PTR_OR_U16;

/* After this point, do not permit native types like
 * short, int, long to be used; use one of the types
 * defined above instead. */
#ifndef CONFIG_NATIVE
#define short short_not_supported
#define int int_not_supported
#define long long_not_supported
#endif

/* TODO - move these elsewhere */
extern U8 sys_init_complete;
extern U8 sys_init_pending_tasks;
extern U8 periodic_ok;

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
#include <platform/native.h>
#endif

/* Include the standard header files that are needed
 * by most modules */

/* Main configuration - game specific.  All machines MUST define one */
#include <mach-config.h>

/* Processor specifics */
#include <limits.h>
#ifdef __m6809__

#include <m6809/m6809.h>
#ifndef NULL
#define NULL 0
#endif

#else

#define __blockcopy16(s1,s2,n) memcpy(s1,s2,n)
#define __blockclear16(s,n) memset(s,0,n)

#define _VA_LIST_DEFINED
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)


/* Build system information */
#include <env.h>
#include <system/io.h>

/* CPU specifics */
#include <system/irq.h>

/* Entry points to initialization */
__noreturn__ void warm_reboot (void);
__noreturn__ void freewpc_init (void);

/* Platform specifics */
#ifdef CONFIG_PLATFORM_WHITESTAR
#include <platform/whitestar.h>
#define __CPU_BOARD
#endif
#ifdef CONFIG_PLATFORM_WPC
#include <platform/wpc.h>
#define __CPU_BOARD
#endif

/* Core software structures */
#include <system/bitarray.h>
#include <system/errno.h>
#include <system/time.h>
#include <system/task.h>
#if (MACHINE_DMD == 1)
#include <system/font.h>
#endif

/* Basic data structures */

#include <misc.h>
//#include <list.h>
#include <log.h>

/* Hardware modules */
#ifdef __CPU_BOARD
#include <system/ac.h>
#include <system/sol.h>
#include <system/lamp.h>
#include <system/sound.h>
#include <system/switch.h>
#include <system/flip.h>
#include <system/display.h>
#ifdef CONFIG_GI
#include <system/triac.h>
#endif
#include <system/rtc.h>

/* Common software structures */
#include <priority.h>
#include <system/deff.h>
#include <system/leff.h>
#include <system/device.h>
#include <system/math.h>
#include <timer.h>
#include <score.h>
#include <game.h>
#include <stdadj.h>
#include <audit.h>
#ifndef NATIVE_SYSTEM
#include <printf.h>
#endif
#include <callset.h>
#include <filesystem.h>
#include <audio.h>
#include <timedmode.h>
#include <serve.h>
#include <mbmode.h>
#include <generic.h>
#include <lang.h>
#include <deffdata.h>

/* Uncommon software modules - TODO : shouldn't automatically include */
#ifdef CONFIG_PLATFORM_WPC
#include <system/debug.h>
#endif
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

/* Automatically include image IDs */
#ifdef CONFIG_PLATFORM_WPC
#ifndef NO_MAIN
#ifndef NATIVE_SYSTEM
#if (MACHINE_DMD == 1)
#include <imagemap.h>
#endif
#endif
#endif
#endif
#endif /* __CPU_BOARD */

/* This is ugly, but I can't figure out any other way to get 
 * pragmas working */
#ifndef CONFIG_NATIVE
#ifdef PAGE
#if (PAGE == 52)
#define PAGE_PRAGMA _Pragma ("section (\"page52\")")
#elif (PAGE == 53)
#define PAGE_PRAGMA _Pragma ("section (\"page53\")")
#elif (PAGE == 54)
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

#ifdef NOSTATIC
#define static
#endif

#ifdef __cplusplus
} 
#endif /* extern "C" */

#endif /* _FREEWPC_H */
