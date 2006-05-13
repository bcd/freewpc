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

#ifndef _SYS_LEFF_H
#define _SYS_LEFF_H

typedef uint8_t leffnum_t;

typedef void (*leff_function_t) (void) __taskentry__;

#define L_NORMAL	0x0

/** A running leff is long-lived and continues to be active
 * until it is explicitly stopped. */
#define L_RUNNING 0x1

typedef struct
{
	U8 flags;
	U8 prio;
	U8 lampset;
	U8 gi;
	leff_function_t fn;
} leff_t;

#define MAX_QUEUED_LEFFS 8

uint8_t leff_get_active (void);
void leff_start (leffnum_t dn);
void leff_stop (leffnum_t dn);
void leff_restart (leffnum_t dn);
void leff_start_highest_priority (void);
__noreturn__ void leff_exit (void);
void leff_init (void);
void leff_stop_all (void);

#endif /* _SYS_LEFF_H */

