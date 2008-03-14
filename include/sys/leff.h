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

#ifndef _SYS_LEFF_H
#define _SYS_LEFF_H

typedef U8 leffnum_t;

typedef void (*leff_function_t) (void);

#define L_NORMAL	0x0

/** A running leff is long-lived and continues to be active
 * until it is explicitly stopped. */
#define L_RUNNING 0x1

/** A shared leff is lower priority and can only update a
 * subset of the lamps.  Multiple shared leffs can be running
 * concurrently.  If a shared leff can't get all of the lamps
 * that it wants, it doesn't run, otherwise it will.
 */
#define L_SHARED 0x2


/** A lamp effect descriptor */
typedef struct
{
	/** Miscellaneous flags */
	U8 flags;

	/** Its priority (higher value means more important) */
	U8 prio;

	/** The maximum set of lamps that it will want to control,
	 * expressed as a lamplist. */
	U8 lamplist;

	/** The maximum GI strings that it will want to control,
	 * expressed as a bitmask of GI string values. */
	U8 gi;

	/** The function implementing the leff */
	leff_function_t fn;

	/** The ROM page where the function is located */
	U8 page;
} leff_t;

#define MAX_QUEUED_LEFFS 8


/** Per-leff state variables */
typedef struct
{
	U8 apply_delay;
	U8 data;
	U8 flags;
	U8 id;
} leff_data_t;


#define lamplist_apply_delay	(task_current_class_data (leff_data_t)->apply_delay)
#define lamplist_private_data	(task_current_class_data (leff_data_t)->data)
#define leff_running_flags		(task_current_class_data (leff_data_t)->flags)
#define leff_self_id				(task_current_class_data (leff_data_t)->id)

extern inline void leff_create_peer (void (*fn)(void))
{
	task_pid_t tp = task_create_peer (fn);
	task_inherit_class_data (tp, leff_data_t);
}

leffnum_t leff_get_active (void);
void leff_start (leffnum_t dn);
void leff_stop (leffnum_t dn);
void leff_restart (leffnum_t dn);
task_pid_t leff_find_shared (leffnum_t dn);
void leff_start_highest_priority (void);
__noreturn__ void leff_exit (void);
void leff_init (void);
void leff_stop_all (void);

#endif /* _SYS_LEFF_H */

