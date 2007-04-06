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

#ifndef _SYS_DEFF_H
#define _SYS_DEFF_H


/** Type for a display effect ID */
typedef uint8_t deffnum_t;

/** Type for a display effect function */
typedef void (*deff_function_t) (void) __taskentry__;

#define D_NORMAL	0x0

/** A running deff is long-lived and continues to be active
 * until it is explicitly stopped. */
#define D_RUNNING 0x1

/** Type for a display effect definition */
typedef struct
{
	U8 flags;
	U8 prio;
	deff_function_t fn;
	U8 page;
} deff_t;


/** The maximum number of display effects that can be queued (i.e.
 * waiting to run, but blocked by a higher priority effect).
 */
#define MAX_QUEUED_DEFFS 16


#define DECLARE_DEFF(id, _flags, _prio, _page) \
	extern void id##_func (void); \
	const deff_t id##_deff = { \
		.flags = _flags, \
		.prio = _prio, \
		.fn = id##_func, \
		.page = _page, \
	}; \
	void id##_func (void) __taskentry__


U8 deff_get_count (void);
bool deff_is_running (deffnum_t dn);
uint8_t deff_get_active (void);
void deff_start (deffnum_t dn);
void deff_start_nopage (deffnum_t dn);
void deff_stop (deffnum_t dn);
void deff_restart (deffnum_t dn);
void deff_start_highest_priority (void);
__noreturn__ void deff_exit (void);
void deff_delay_and_exit (task_ticks_t ticks);
void deff_swap_low_high (int8_t count, task_ticks_t delay);
void deff_init (void);
void deff_stop_all (void);


#ifndef MACHINE_CUSTOM_AMODE
void default_amode_deff (void);
#endif

#ifndef DEFF_AMODE
#define DEFF_AMODE 1
#endif

#endif /* _SYS_DEFF_H */

