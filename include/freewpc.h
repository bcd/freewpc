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

#ifndef _FREEWPC_H
#define _FREEWPC_H

/* For C++ compilation.  Everything in the kernel is plain C. */
#ifdef __cplusplus
extern "C" {
#endif

/*
 * This is the top-level include file for FreeWPC.  All files
 * should include this one first in order to get all of the
 * required definitions and a lot of the more commonly used
 * features.
 */


/*
 * Standard types
 */
#ifndef __cplusplus
typedef unsigned char bool;
#endif

/* 8-bit integers are always 'char' */
typedef unsigned char bcd_t, BCD;
typedef signed char int8_t, I8, S8;
typedef unsigned char uint8_t, U8;

/* 16-bit integers depend on the compiler */
#if defined(__m6809__) && defined(__int16__)
typedef int int16_t, I16, S16;
typedef unsigned int uint16_t, U16;
typedef U16 INTPTR;
#elif defined(CONFIG_PLATFORM_LINUX)
typedef short int16_t, I16, S16;
typedef unsigned short uint16_t, U16;
#else /* default assumes -mint8 on wpc */
typedef long int16_t, I16, S16;
typedef unsigned long uint16_t, U16;
typedef unsigned int INTPTR;
#endif

/* TODO - move these elsewhere */
extern U8 sys_init_complete;
extern U8 sys_init_pending_tasks;
extern U8 irq_count;

/*
 * Define TRUE and FALSE to suitable values.
 */
#undef TRUE
#define TRUE ((bool)1)
#undef FALSE
#define FALSE ((bool)0)

/* Define NULL */
#define NULL 0UL

/*
 * Define macros that let us print #defines which are strings.
 */
#define C_STRING(x)	C_STR(x)
#define C_STR(x)		#x

#ifdef CONFIG_PLATFORM_LINUX
#define DIV10(x,q,r)     ({ q = x / 10; r = x % 10; })
#endif

/* Include the standard header files that are needed
 * by most modules */

/* Main configuration - game specific.  All machines MUST define one */
#include <mach/config.h>

/* Processor specifics */
#ifdef __m6809__
#include <m6809/m6809.h>
#else
#define __blockcopy16(s1,s2,n) memcpy(s1,s2,n)
#define __blockclear16(s,n) memset(s,'0',n)
#include <stdio.h>
#include <string.h>
#endif

/* Build system information */
#include <env.h>

/* Platform specifics */
#ifdef MACHINE_PLATFORM_WHITESTAR
#include <whitestar.h>
#else
#include <wpc.h>
#endif

#include <version.h>
#include <misc.h>
#include <math.h>

/* Core software structures */
#include <sys/bitarray.h>
#include <sys/errno.h>
#include <sys/task.h>
#include <sys/font.h>

/* Hardware modules */
#include <sys/ac.h>
#include <sys/sol.h>
#include <sys/lamp.h>
#include <sys/sound.h>
#include <sys/switch.h>
#include <sys/flip.h>
#include <sys/dmd.h>
#include <sys/triac.h>
#include <sys/irq.h>
#include <sys/rtc.h>

/* Other software structures */
#include <sys/deff.h>
#include <sys/leff.h>
#include <sys/debug.h>
#include <sys/device.h>
#include <timer.h>
#include <score.h>
#include <coin.h>
#include <game.h>
#include <player.h>
#include <stdadj.h>
#include <audit.h>
#include <printf.h>
#include <amode.h>
#include <window.h>
#include <callset.h>
#include <priority.h>
#include <eb.h>
#include <highscore.h>
#include <search.h>
#include <status.h>
#include <replay.h>
#include <knocker.h>
#include <csum.h>
#include <inspector.h>
#include <test.h>


/* Game-specific defines.  'mach' should point to the machine-specific 
 * directory.  These files are optional; if a machine does not need
 * one of these, it should define the corresponding flag.
 */
#ifndef MACHINE_LACKS_ALL_INCLUDES
#ifndef MACHINE_LACKS_SWITCH_H
#include <mach/switch.h>
#endif
#ifndef MACHINE_LACKS_COIL_H
#include <mach/coil.h>
#endif
#ifndef MACHINE_LACKS_SOUND_H
#include <mach/sound.h>
#endif
#ifndef MACHINE_LACKS_LAMP_H
#include <mach/lamp.h>
#endif
#ifndef MACHINE_LACKS_FLAGS_H
#ifdef MACHINE_INCLUDE_FLAGS
#include <mach/flags.h>
#endif
#endif
#ifndef MACHINE_LACKS_PROTOS_H
#include <mach/protos.h>
#endif
#endif /* MACHINE_LACKS_ALL_INCLUDES */

/* Automatically generated header files */
#include <gendefine_gid.h>
#include <gendefine_deff.h>
#include <gendefine_leff.h>
#include <gendefine_lampset.h>
#include <gendefine_devno.h>
#include <../images/xbmproto.h>

/* This is ugly, but I can't figure out any other way to get 
 * pragmas working */
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

#ifdef NOSTATIC
#define static
#endif

#ifdef __cplusplus
} 
#endif /* extern "C" */

#endif /* _FREEWPC_H */
