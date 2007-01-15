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

#include <freewpc.h>

/**
 * \file
 * \brief This module implements a round-robin, non-realtime, non-preemptive
 * task scheduler.
 *
 * A "task" is analogous to a process.  Tasks can be created, stopped,
 * killed, etc. as one would expect.  All tasks are "peers", as no
 * parent/child relationship exists among the tasks.
 * 
 * The process ID of a task is simply a pointer to its kernel task
 * structure; every task therefore has a unique pid.
 *
 * The group ID of a task can be declared when it is created; multiple
 * tasks can share the same group ID.  Killing a group ID kills all
 * tasks in the group.  Group IDs exist in order to make it easier to
 * find tasks based on a small fixed data value, rather than requiring
 * process IDs to be stored in costly RAM.
 *
 * Most of the module is written in portable C.  The task structure
 * and the register save/restore routines are written in lots of
 * assembler, though.
 *
 * Caveats:
 * 1) Inside functions that use a lot of assembly, we expect register
 * X to be preserved across calls (e.g. task_save -> task_dispatcher).
 * Please do NOT make any ordinary function calls in these types of
 * routines, as they are likely to trash registers in unexpected ways.
 *
 * TODO:
 * The current implementation uses a static array, and a flag
 * to indicate whether or not the task is allocated.  It would be
 * faster on average if we maintained separate chains of tasks,
 * so that we only had to scan one particular chain (e.g. the
 * running chain or the free chain) at a time.
 */


/** task_current points to the control structure for the current task. */
__fastram__ task_t *task_current;

/** When saving a task's registers during dispatch, we need some
 * static storage to help compensate for the lacking of registers.
 * Don't allocate on the stack, since we need to preserve the
 * stack pointer carefully during dispatch. */
__fastram__ U16 task_save_U, task_save_X;

/** The static array of task structures */
task_t task_buffer[NUM_TASKS];

/** A flag that indicates that dispatching is working as expected.
 * This is set to 1 everytime we dispatch correctly, and to 0
 * periodically from the IRQ.  If the IRQ finds it at 0, that
 * means we went a long time without a dispatch; this probably
 * means that (1) some task has been running for a very long time,
 * or (2) the task is jumped into the weeds and is never coming back.
 * Case (1) is theoretically OK, but we consider it just as bad as
 * case (2), which could lead to all kinds of weird behavior.
 * The IRQ will reset the system when this happens. */
bool task_dispatching_ok;

__fastram__ U8 tick_start_count;

#ifdef CONFIG_DEBUG_STACK
/** For debug, this tells us the largest stack size that we've had
 * to deal with so far.  This helps to determine how big the stack
 * area in the task structure needs to be */
U8 task_largest_stack;

U16 task_small_stacks;
U16 task_medium_stacks;
U16 task_large_stacks;
#endif

/** Also for debug, this tracks the maximum number of tasks needed. */
#ifdef CONFIG_DEBUG_TASKCOUNT
U8 task_count;
U8 task_max_count;
#endif


/* Uncomment this to turn on dumping of entire task table.
 * Normally, only the running entries are displayed. */
//#define DUMP_ALL_TASKS


/* Private functions written in assembly used internally. */
__noreturn__ void task_save (void);
__noreturn__ void task_restore (void);


/** For debugging, dump the entire contents of the task table to the
 * debug port. */
void task_dump (void)
{
#ifdef DEBUGGER
	register U8 t;
	register task_t *tp;

	dbprintf ("\nCurrent = %p\n", task_current);
#ifdef CONFIG_DEBUG_TASKCOUNT
	dbprintf ("Max tasks = %d\n", task_max_count);
#endif
	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
	{
#ifndef DUMP_ALL_TASKS
		if (tp->state != TASK_FREE)
#endif
		{
			dbprintf ("%p: GID %02X  PC %p",
				tp, tp->gid, tp->pc);
			dbprintf ("  ST ");
			dbprintf ("%02X", tp->stack_size);
			dbprintf ("  ARG %04X", tp->arg);
			dbprintf ("  TD ");
			dbprintf ("%02X %02X %02X %02X\n",
				tp->thread_data[0], tp->thread_data[1],
				tp->thread_data[2], tp->thread_data[3]);
		}
	}
	dbprintf ("\n\n");
#endif
}


/**
 * Allocate a new task block, by searching the array (linearly)
 * for the first entry that is TASK_FREE.
 */
task_t *task_allocate (void)
{
	register U8 t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if ((tp->state == TASK_FREE) && (tp != task_current))
		{
			tp->state = TASK_USED;
			tp->delay = 0;
			tp->stack_size = 0;
			tp->flags = 0;
			return tp;
		}

	/* TODO : if there are no free blocks, it might be possible
	to kill some other non-critical tasks to make way for this
	one.  We have no idea what the criticality of the current task
	is, though, so this could actually be harmful.  Some tasks
	that are candidates for death by lethal injection:
	lamp effects (GID_LEFF, GID_LEFF_EXITING); switch lamp flickers
	(GID_SWITCH_LAMP_PULSE); pending switch handlers that have been
	started but not yet begun (GID_SW_HANDLER) -- this would be as if
	the switch never triggered. */
	fatal (ERR_NO_FREE_TASKS);
	return 0;
}


/** Initialize the thread data of a different task to have the same values
 * as the thread data of the currently running task. */
void task_inherit_thread_data (task_t *tp)
{
	U8 i;
	for (i=0; i < 4; i++)
		tp->thread_data[i] = task_current->thread_data[i];
}


/** Create a task with a specific group ID (GID). */
task_t *task_create_gid (task_gid_t gid, task_function_t fn)
{
	register task_function_t fn_x asm ("x") = fn;
	register task_t *tp asm ("x");

	__asm__ volatile ("jsr\t_task_create" : "=r" (tp) : "0" (fn_x) : "d");
	tp->gid = gid;
	tp->arg = 0;
#ifdef CONFIG_DEBUG_TASKCOUNT
	task_count++;
	if (task_count > task_max_count)
		task_max_count = task_count;
#endif
	return (tp);
}


/** Create a task, but not if a task with the same GID already exists.
 * The previous task will continue to run. */
task_t *task_create_gid1 (task_gid_t gid, task_function_t fn)
{
	task_t *tp = task_find_gid (gid);
	if (tp) 
		return (tp);
	return task_create_gid (gid, fn);
}


/** Create a task with a given GID, ensuring that only one task
 * with that GID exists.  Any tasks with the same GID are killed
 * prior to starting the new task.  */
task_t *task_recreate_gid (task_gid_t gid, task_function_t fn)
{
	task_kill_gid (gid);
	if (task_find_gid (gid))
		fatal (ERR_TASK_KILL_FAILED);
	return task_create_gid (gid, fn);
}


/** Change the GID of the currently running task */
void task_setgid (task_gid_t gid)
{
	task_current->gid = gid;
}


/** Suspend the current task for a period of time */
void task_sleep (task_ticks_t ticks)
{
	extern uint8_t tick_count;
	register task_t *tp = task_current;

	if (tp == 0)
		fatal (ERR_IDLE_CANNOT_SLEEP);

	tp->delay = ticks;
	tp->asleep = tick_count;

#if 0
	if (tp->state != TASK_USED)
		fatal (99);
#endif
	tp->state = TASK_BLOCKED+TASK_USED; /* was |= */

	task_save ();
}


/** Suspend the current task for a number of seconds */
void task_sleep_sec (int8_t secs)
{
	do {
		task_sleep (TIME_1S);
	} while (--secs > 0);
}


/**
 * Exit the current task, and return to the dispatcher to select
 * another task to start running now.
 */
__naked__ __noreturn__ 
void task_exit (void)
{
	if (task_current == 0)
		fatal (ERR_IDLE_CANNOT_EXIT);

	task_current->state = TASK_FREE;
	task_current = 0;
#ifdef CONFIG_DEBUG_TASKCOUNT
	task_count--;
#endif
	task_dispatcher ();
}


/**
 * Find the task that has the given group ID.
 *
 * If more than one task matches, only the first can be returned by
 * this API.  Which one is first in the table is indeterminable.
 *
 * If no task is found, then NULL is returned; otherwise, a pointer
 * to the task structure is returned.
 */
task_t *task_find_gid (task_gid_t gid)
{
	register task_t *tp;
	for (tp = task_buffer; tp < &task_buffer[NUM_TASKS]; tp++)
		if ((tp->state != TASK_FREE) && (tp->gid == gid))
			return (tp);
	return (NULL);
}


/**
 * Find the task that has private data 'val' at location 'off'.
 */
task_t *task_find_gid_data (task_gid_t gid, U8 off, U8 val)
{
	register task_t *tp;
	for (tp = task_buffer; tp < &task_buffer[NUM_TASKS]; tp++)
		if ((tp->state != TASK_FREE) 
			&& (tp->gid == gid) 
			&& (tp->thread_data[off] == val))
			return (tp);
	return (NULL);
}


/**
 * Kills the given task.
 */
void task_kill_pid (task_t *tp)
{
	if (tp == task_current)
		fatal (ERR_TASK_KILL_CURRENT);
	tp->state = TASK_FREE;
	tp->gid = 0;
#ifdef CONFIG_DEBUG_TASKCOUNT
	task_count--;
#endif
}


/**
 * Kill all tasks with the given group ID.
 * A task can call this function with its own group ID: that will not
 * kill itself, but will kill all peer tasks that share its group ID.
 */
bool task_kill_gid (task_gid_t gid)
{
	register U8 t;
	register task_t *tp;
	bool rc = FALSE;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if (	(tp != task_current) &&
				(tp->state != TASK_FREE) && 
				(tp->gid == gid) )
		{
			task_kill_pid (tp);
			rc = TRUE;
		}
	return (rc);
}


/**
 * Kills all tasks that are not protected.
 *
 * Protected tasks are marked with the flag TASK_PROTECTED and are
 * immune to this call.
 */
void task_kill_all (void)
{
	register U8 t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if (	(tp != task_current) &&
				(tp->state != TASK_FREE) && 
				!(tp->flags & TASK_PROTECTED) )
		{
			task_kill_pid (tp);
		}
}


/**
 * Sets task flags.
 *
 * TODO : this is a race condition here with regard to TASK_PROTECTED;
 * the task might be killed after it has been created, but before it
 * gets a chance to set TASK_PROTECTED.  The caller is probably aware
 * of the special status and should be able to set it at creation time.
 */
void task_set_flags (U8 flags)
{
	task_current->flags |= flags;
}


/**
 * Clears task flags.
 */
void task_clear_flags (U8 flags)
{
	task_current->flags &= ~flags;
}


/**
 * Get/set the task argument word.
 * TODO : why not just set the B/X register directly here and then
 * the task can take the arg as a normal function argument?  The
 * task block doesn't save B/X normally, but for the first call it
 * would be easier to have it this way.  Or maybe just push all
 * arguments onto the stack... at least the 'arg' overhead is not
 * required for every task this way.  (It would be like a variadic
 * function.)
 */
U16 task_get_arg (void)
{
	return task_current->arg;
}


void task_set_arg (task_t *tp, U16 arg)
{
	tp->arg = arg;
}


/**
 * The task dispatcher.  This function selects a new task to run.
 *
 * This is called from two places: when a task exits, or when a
 * task sleeps/yields.  The 'x' register is expected to be loaded with
 * the exiting task's task structure pointer.  The search for a new
 * task to run starts with the next task in the table.  Code then
 * jumps back to the first candidate that is eligible, via
 * task_restore().
 *
 * If the entire task table is scanned, and no task is ready to run,
 * then we wait until the tick count advances to indicate that 1ms
 * has elapsed.  Then we execute all of the idle functions.  This
 * means that idle functions are never called if there is also at
 * least one task in the ready state; it also limits their invocation
 * to once per second.  The 1ms delay is also useful because it is the
 * minimum delay before which a blocked task might reach the end of its
 * wait period; there is no sense checking more frequently.
 */
__naked__ __noreturn__
void task_dispatcher (void)
{
	extern U8 tick_count;
	register task_t *tp asm ("x");
	task_t *first = tp;

	tick_start_count = tick_count;
	task_dispatching_ok = TRUE;

	/* Set 'first' to the first task block to try. */
	for (tp++; ; tp++)
	{
		/* Reset task pointer to top of list after reaching the
		 * bottom of the table. */
		if (tp == &task_buffer[NUM_TASKS])
			tp = &task_buffer[0];

		/* All task blocks were scanned, and no free task was found. */
		if (first == tp)
		{
			/* Wait for next task tick before continuing.
			Ensure that the idle function is not invoked more than once
			per 16ms.  TODO - this is a long time!! */
			while (tick_start_count == *(volatile U8 *)&tick_count);
			
			db_idle ();
		
			/* If the system is fully initialized, also
			 * run the idle tasks once every pass through
			 * the list. */
			if (sys_init_complete)
					callset_invoke (idle);

			/* Reset timer and kick watchdog again */
			tick_start_count = tick_count;
			task_dispatching_ok = TRUE;

			/* Ensure that 'tp', which is in register X, is reloaded
			with the correct task pointer.  The above functions may
			trash its value. */
			tp = first;
		}

		if (tp->state == TASK_USED)
		{
			/* The task exits, and is not sleeping.  It can be
			started immediately. */
			task_restore ();
		}
		else if (tp->state == TASK_USED+TASK_BLOCKED)
		{
			/* The task exists, but is sleeping.
			 * tp->asleep holds the time at which it went to sleep,
			 * and tp->delay is the time for which it should sleep.
			 * Examine the current tick count, and see if it should
			 * be woken up.
			 */
			register uint8_t ticks_elapsed = tick_count - tp->asleep;
			if (ticks_elapsed >= tp->delay)
			{
				/* Yes, it is ready to run again. */
				tp->state &= ~TASK_BLOCKED;
				task_restore ();
			}
		}
	}
}


/**
 * Initialize the task subsystem.  This transforms the caller into a
 * legitimate task, which can then sleep, yield, etc.
 */
void task_init (void)
{
	/* Clean the memory for all task blocks */
	memset (task_buffer, 0, sizeof (task_buffer));

	/* No dispatching lockups so far */
	task_dispatching_ok = TRUE;

#ifdef CONFIG_DEBUG_STACK
	/* Init debugging of largest stack */
	task_largest_stack = 0;
	task_small_stacks = 0;
	task_medium_stacks = 0;
	task_large_stacks = 0;
#endif

#ifdef CONFIG_DEBUG_TASKCOUNT
	task_count = task_max_count = 1;
#endif

	/* Allocate a task for the first (current) thread of execution.
	 * The calling routine can then sleep and/or create new tasks
	 * after this point. */
	task_current = task_allocate ();
	task_current->state = TASK_USED;
	task_current->arg = 0;
	task_current->gid = GID_FIRST_TASK;
	task_current->delay = 0;
}

