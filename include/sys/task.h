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

#ifndef _SYS_TASK_H
#define _SYS_TASK_H

#include <env.h>
#ifdef HAVE_LIBC
#include <sys/types.h>
#endif


/** Values for the 'state' field in the task structure */
#define TASK_FREE		0
#define TASK_USED		1
#define TASK_BLOCKED	2


/** Values for the 'flags' field */
#define TASK_PROTECTED   0x01


/** The maximum number of tasks that can be running at once.
 * Space for this many task structures is statically allocated.
 */
#define NUM_TASKS 48

/*
 * Define the size of the saved process stack.
 *
 * This value + 17 should equate a power of 2.
 */
#define TASK_STACK_SIZE		47

/** Type for the group ID (gid) */
typedef U8 task_gid_t;

/** Type for the number of task ticks.  1 task tick is the
 * smallest amount that a task can sleep. */
typedef U8 task_ticks_t;

/** Type for the entry point to a new task */
typedef void (*task_function_t) (void);

/** Type for the task structure.
 * For proper saving/restoring, only the registers that are live
 * across function calls need to be kept here.  The 6809 compiler
 * allows functions to trash D and X, so they aren't here -- just
 * U and Y.
 *
 * When adding/removing fields to this structure, adjust
 * TASK_STACK_SIZE above accordingly so that the total size is
 * a power of 2.
 */
typedef struct
{
	task_gid_t	gid;
	U16			next;
	U16			pc;
	U16			y;
	U16			u;
	U8				rom_page;
	U8				stack_word_count;
	U8				flags;
	U8				delay;
	U8				asleep;
	U8				state;
	U16			arg;
	U8				stack[TASK_STACK_SIZE];
} task_t;

typedef task_t *task_pid_t;

extern bool task_dispatching_ok;

extern task_t *task_current;


/********************************/
/*     Inline Macros            */
/********************************/

extern inline task_t *task_getpid (void)
{
	return task_current;
}

extern inline task_gid_t task_getgid (void)
{
	return task_current->gid;
}


/*******************************/
/*     Debug Timing            */
/*******************************/

#define debug_time_start() \
{ \
	U8 __debug_timer = irq_count; \


#define debug_time_stop() \
	dbprintf ("debug time: %02X %02X\n", \
		__debug_timer, irq_count); \
}


/********************************/
/*     Function Prototypes      */
/********************************/

void task_dump (void);
void task_init (void);
void task_create (void);
task_t *task_create_gid (task_gid_t, task_function_t fn);
task_t *task_create_gid1 (task_gid_t, task_function_t fn);
task_t *task_recreate_gid (task_gid_t, task_function_t fn);
task_t *task_getpid (void);
task_gid_t task_getgid (void);
void task_setgid (task_gid_t gid);
void task_sleep (task_ticks_t ticks);
void task_sleep_sec (int8_t secs);
__noreturn__ void task_exit (void);
task_t *task_find_gid (task_gid_t);
bool task_kill_gid (task_gid_t);
void task_kill_all (void);
void task_set_flags (U8 flags);
void task_clear_flags (U8 flags);
U16 task_get_arg (void);
void task_set_arg (task_t *tp, U16 arg);
__noreturn__ void task_dispatcher (void);

#define task_create_child(fn)		task_create_gid (task_getgid (), fn)

#define task_create_anon(fn)		task_create_gid (0, fn)

#define task_yield()					task_sleep (0)

#endif /* _SYS_TASK_H */
