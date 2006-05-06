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

/** The maximum number of tasks that can be running at once.
 * Space for this many task structures is statically allocated.
 */
#define NUM_TASKS 32


/** Type for the group ID (gid) */
typedef uint8_t task_gid_t;

/** Type for the number of task ticks.  1 task tick is the
 * smallest amount that a task can sleep. */
typedef uint8_t task_ticks_t;

/** Type for the entry point to a new task */
typedef void (*task_function_t) (void);

/** Type for the task structure */
typedef struct
{
	task_gid_t		gid;
	uint8_t        unused;
	uint16_t			pc;
	uint16_t			next;
	uint16_t			x;
	uint16_t			y;
	U8					stack_word_count;
	U8					unused_3;
	uint16_t			u;
	uint8_t			delay;
	uint8_t			asleep;
	uint8_t			state;
#if 0
	uint8_t			a;
	uint8_t			b;
#else
	uint8_t			rom_page;
	uint8_t			unused_2;
#endif
	uint16_t			arg;
	uint8_t			stack[TASK_STACK_SIZE];
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

/*******************************/
/*   Large Stack Allocation    */
/*******************************/

/* These are obsolete macros */
#define stack_large_begin()
#define stack_large_end()

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
uint16_t task_get_arg (void);
void task_set_arg (task_t *tp, uint16_t arg);
__noreturn__ void task_dispatcher (void);

#define task_create_child(fn)		task_create_gid (task_getgid (), fn)

#define task_yield()					task_sleep (0)

#endif /* _SYS_TASK_H */
