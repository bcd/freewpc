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
typedef unsigned char bcd_t;
typedef char int8_t, I8, S8;
typedef unsigned char uint8_t, U8;
typedef long int16_t, I16, S16;
typedef unsigned long uint16_t, U16;


extern U8 sys_init_complete;
extern U8 sys_init_pending_tasks;
extern U8 irq_count;

/*
 * Define TRUE and FALSE to suitable values.
 */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

/*
 * Define macros that let us print #defines which are strings.
 */
#define C_STRING(x)	C_STR(x)
#define C_STR(x)		#x

/*
 * Define the size of the process stack (TODO:
 * move this elsewhere)
 *
 * This value + 21 should be a nice round number.
 * Original : 43 ==> Total size of 64
 */
#define TASK_STACK_SIZE		43
/// #define TASK_STACK_SIZE		75
/// #define TASK_STACK_SIZE		91

/* Include the standard header files that are needed
 * by most modules */

/* Main configuration - game specific */
#include <mach/config.h>

/* General stuff */
#include <asm-6809.h>
#include <wpc.h>
#include <version.h>

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

/* Game-specific defines.  'mach' should point to
 * the machine-specific directory, e.g. 'tz'. */
#include <mach/switch.h>
#include <mach/coil.h>
#include <mach/sound.h>
#include <mach/lamp.h>

/* Automatically generated header files */
#include <gendefine_gid.h>
#include <gendefine_deff.h>
#include <gendefine_leff.h>
#include <gendefine_lampset.h>
#include <gendefine_devno.h>
#include <../images/xbmproto.h>

/* From here on are any headers that are dependent on machine
 * specific features being defined */
#include <hook.h>

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

#ifdef __cplusplus
} 
#endif /* extern "C" */

#endif /* _FREEWPC_H */
