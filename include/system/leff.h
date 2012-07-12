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

#ifndef _SYS_LEFF_H
#define _SYS_LEFF_H

typedef U8 leffnum_t;

typedef void (*leff_function_t) (void);


/* More informative names for the first two fields? */

#define LEFF_SHOW(prio)  L_NORMAL, (prio)
#define LEFF_BG(prio)    L_RUNNING, (prio)
#define LEFF_SHARED      L_SHARED, PRI_NULL


/** A lamp effect descriptor */
typedef struct
{
	/** Its priority (higher value means more important).  Priority
	is only meaningful for non-shared leffs. */
	U8 prio;

	/** The function implementing the leff */
	leff_function_t fn;

	/** The ROM page where the function is located */
	U8 page;

	/* The set of resources that it needs */
	lamplist_id_t llid;

	U8 gi;
} leff_t;


/** Per-leff state variables.  These are per-process variables kept
in the task structure, at the top of stack. */
typedef struct
{
	/* The amount of delay in between each iteration in a lamplist
	traversal.  Default is zero, or no delay. */
	U8 apply_delay;

	/* A private data field for use by the leff.  This is provided to
	allow the task creator to pass in an argument. */
	U8 data;

	/* The flags from the original lamp effect descriptor. */
	U8 flags;
} __attribute__((may_alias)) leff_data_t;


/* Macros for accessing the above fields more conveniently */

#define lamplist_apply_delay	(task_current_class_data (leff_data_t)->apply_delay)
#define lamplist_private_data	(task_current_class_data (leff_data_t)->data)
#define leff_running_flags		(task_current_class_data (leff_data_t)->flags)
#define leff_self_id				(task_current_class_data (leff_data_t)->id)

#define MAX_RUNNING_LEFFS 8

/**
 * Create a subtask from an existing lamp effect task.
 *
 * It is important to call this instead of just task_create(), because
 * the per-leff state data needs to be inherited, so the new task will know
 * how to behave.
 */
extern inline void leff_create_peer (void (*fn)(void))
{
	task_pid_t tp = task_create_peer (fn);
	task_inherit_class_data (tp, leff_data_t);
}

void leff_start (leffnum_t dn);
void leff_stop (leffnum_t dn);
void leff_restart (leffnum_t dn);
__noreturn__ void leff_exit (void);
bool leff_running_p (leffnum_t dn);
void leff_stop_all (void);
void leff_reset (void);
bool leff_quick_alloc (lampnum_t lamp);
void leff_quick_free (lampnum_t lamp);
void leff_init (void);

#endif /* _SYS_LEFF_H */

