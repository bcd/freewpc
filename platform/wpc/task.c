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
 * or (2) the task jumped into the weeds and is never coming back.
 * Case (1) is theoretically OK, but we consider it just as bad as
 * case (2), which could lead to all kinds of weird behavior.
 * The IRQ will reset the system when this happens. */
__fastram__ bool task_dispatching_ok;

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

#ifdef IDLE_PROFILE
U8 idle_rtt_calls;
U16 idle_time;
U16 last_idle_time;
#endif

#ifdef TASK_CHAINING

 /* The current implementation uses a static array, and a flag
 * to indicate whether or not the task is allocated.  It would be
 * faster on average if we maintained separate chains of tasks,
 * so that we only had to scan one particular chain (e.g. the
 * running chain or the free chain) at a time.
 */

/** An index used to indicate end-of-list */
#define TASK_LIST_NULL -1

/** The index of the current task */
U8 to_current;

/** The index of the first free block */
U8 to_free;

/** The index of the first and last allocated task */
U8 to_alloc_head;
U8 to_alloc_tail;
#endif


/* Private functions written in assembly used internally. */
__noreturn__ void task_save (task_t *tp);
__noreturn__ void task_restore (task_t *tp);


#ifdef IDLE_PROFILE
void idle_profile_rtt (void)
{
	wpc_debug_write ('.');
	if (--idle_rtt_calls == 0)
	{
		last_idle_time += idle_time;
		idle_time = 0;
		idle_rtt_calls = 8;
	}
}

void idle_profile_idle (void)
{
	/* last_idle_time represents the number of idle loops done
	 * within the last 1 second (approximate).  Each loop is
	 * about 24 cycles now. */
	U16 printed_idle_time;

	/* Read and clear the idle loop count */
	disable_irq ();
	printed_idle_time = last_idle_time;
	last_idle_time = 0;
	enable_irq ();

	/* If nonzero, then print it */
	if (printed_idle_time != 0)
		dbprintf ("Idle %ld\n", printed_idle_time);
	else
		wpc_debug_write ('*');
}
#endif


/** For debugging, dump the entire contents of the task table to the
 * debug port.  This function is called automatically whenever the
 * system crashes.  It can also be triggered by pressing the 't'
 * key in the debugger. */
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
		if (tp->state != BLOCK_FREE)
		{
			dbprintf ("%p: ", tp);
			task_dispatching_ok = TRUE;

			if (tp->state & BLOCK_TASK)
			{
				dbprintf ("GID %02d  PC %p", tp->gid, tp->pc);
				dbprintf ("  ST %02X", tp->stack_size);
				dbprintf ("  ARG %04X\n", tp->arg);
			}
			else if (tp->state & BLOCK_MALLOC)
			{
				malloc_chunk_dump (tp);
			}
			else if (tp->state & BLOCK_STACK)
			{
				dbprintf ("aux stack\n");
			}
			else
			{
				dbprintf ("???\n");
			}
		}
	}
	dbprintf ("\n");
#endif
}


/** Allocate a dynamic block of memory, by searching the block
 * array (linearly) for the first entry that is in the BLOCK_FREE
 * state.  Returns a pointer to the newly allocated block.
 *
 * The 'aux_stack_block' field of a new block is initialized to
 * that block's index.
 */
task_t *block_allocate (void)
{
	register U8 t;
	register task_t *tp;

#ifdef TASK_CHAINING
#else
	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
#endif
		if (tp->state == BLOCK_FREE)
		{
			tp->state = BLOCK_USED;
			tp->aux_stack_block = t;
#ifdef TASK_CHAINING
			/* Remove it to the free list */
#endif
			return tp;
		}
	return NULL;
}


/** Free a dynamic block of memory. */
void block_free (task_t *tp)
{
	tp->state = BLOCK_FREE;
#ifdef TASK_CHAINING
	/* Return it to the free list */
#endif
}


/**
 * Allocate a block for a new task.  Failure to allocate a block
 * is considered fatal.  If successfully allocated, the block
 * is initialized to indicate that it is being used for
 * a task.
 */
task_t *task_allocate (void)
{
	task_t *tp = block_allocate ();
	if (tp)
	{
		tp->state |= BLOCK_TASK;
		tp->delay = 0;
		tp->stack_size = 0;
		tp->aux_stack_block = -1;
		tp->sighandler = NULL;
#ifdef TASK_CHAINING
		/* Add to the task list */
#endif
		return tp;
	}
	else
	{
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
}


/** Expand the stack space for a given task. */ 
task_t *task_expand_stack (task_t *tp)
{
	task_t *sp;

	/* Make sure tp is of type BLOCK_TASK */
	if (!(tp->state & BLOCK_TASK))
		fatal (ERR_LIBC_ABORT);

	/* Allocate a block for the new stack */
	sp = block_allocate ();
	if (!sp)
		fatal (ERR_NO_FREE_TASKS);

	/* Mark it as a stack block, and associate it with the
	given stack */
	sp->state |= BLOCK_STACK;
	tp->aux_stack_block = sp->aux_stack_block;
	sp->aux_stack_block = -1;
	return sp;
}


/** Free a task block for a task that no longer exists. */
static __attribute__((noinline))
void task_free (task_t *tp)
{
	/* Free the auxiliary stack block first if it exists */
	if (tp->aux_stack_block != -1)
		block_free (&task_buffer[tp->aux_stack_block]);

#ifdef TASK_CHAINING
	/* Remove it from the task list */
#endif

	/* Free the task block */
	block_free (tp);
}


/** Create a task with a specific group ID (GID). */
task_t *task_create_gid (task_gid_t gid, task_function_t fn)
{
	register task_function_t fn_x asm ("x") = fn;
	register task_t *tp asm ("x");

	/* This cryptic statement invokes the assembler function 'task_create',
	 * passing it the function pointer in X, returning the task pointer
	 * in the same register (note: 'r' may not be the safest thing to use
	 * here).  It also declares that 'd' is destroyed by the call. */
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
	register task_t *tp = task_current;

	/* Fail if the idle function tries to sleep. */
	if (tp == 0)
		fatal (ERR_IDLE_CANNOT_SLEEP);

#ifdef PARANOID
	/* TODO - verify that interrupts are not disabled when calling this */
#endif

	/* Mark the task as blocked, set the time at which it blocks,
	and set how long it should wait for. 

	TODO : the accuracy here is a bit wonky.  For example, a task could
	want to sleep 16ms (1 tick), and then almost right away, the 16th
	IRQ could fire and decrement the tick count, causing the task to
	wake up having slept nearly 0ms.  Essentially, accuracy is only
	good to within 16ms. 
	
	One solution is to keep these counters in terms of raw IRQs, but
	enforce a minimum sleep time of 16ms still.  Also, we could
	examine 'irq_count' here and adjust ticks accordingly, if we are
	close to the 16 IRQ cycle boundary.  (If 'irq_count' > 8, then
	increment ticks; this improves the resolution.)  Third, we could
	always bump ticks by 1, so that tasks always guarantee to sleep
	for *at least* the quantity asked for, but more frequently, they
	will always sleep a little longer (as much as 16ms longer).  For
	this last approach, consider bumping all of the TIME_ defines
	to make this a static change. */
	tp->delay = ticks;
	tp->asleep = get_ticks ();
	tp->state |= TASK_BLOCKED;

	/* Save the task, and start another one.  This call returns
	whenever the task is eventually unblocked. */
	task_save (tp);
}


/** Suspend the current task for a number of seconds.  This works
around the limitation that the 'delay' field is 8-bit and cannot
store large timeouts. */
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

	task_free (task_current);
	task_current = 0;
#ifdef CONFIG_DEBUG_TASKCOUNT
	task_count--;
#endif
	/* TODO: on entry to the next function, it is expected that 'X' has
	 * the current task pointer.  There is actually nothing that
	 * guarantees this though.  The call to task_free() above
	 * typically takes care of it, since X is needed for the
	 * function argument. */
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
task_t *task_find_gid_next (task_t *last, task_gid_t gid)
{
	register task_t *tp;
	for (tp = last+1; tp < &task_buffer[NUM_TASKS]; tp++)
		if ((tp->state & BLOCK_TASK) && (tp->gid == gid))
			return (tp);
	return (NULL);
}

task_t *task_find_gid (task_gid_t gid)
{
	return task_find_gid_next (task_buffer-1, gid);
}


/**
 * Kills the given task.
 * Killing yourself (suicide) is illegal, so don't do it.
 *
 * If the target has requested a signal handler, then the task is
 * not killed immediately, but will instead have its PC changed to
 * run the signal handler on the next invocation.  The handler
 * should call task_exit() when the task can finally die.
 *
 * If there is no signal handler, the task is killed right away.
 */
void task_kill_pid (task_t *tp)
{
	if (tp == task_current)
		fatal (ERR_TASK_KILL_CURRENT);
	else if (tp->sighandler == NULL)
	{
		task_free (tp);
		tp->gid = 0;
#ifdef CONFIG_DEBUG_TASKCOUNT
		task_count--;
#endif
	}
	else
	{
		tp->u = 1;
		tp->pc = (U16)tp->sighandler;

		/* TODO - this should be a synchronous operation.
		 * We should wait here until the task is truly gone.
		 * If the task is asleep, it should be woken up too. */
	}
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
				(tp->state & BLOCK_TASK) && 
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
				(tp->state & BLOCK_TASK) && 
				!(tp->state & TASK_PROTECTED) )
		{
			task_kill_pid (tp);
		}
}


/**
 * Sets task flags.
 *
 * TODO : there is a race condition here with regard to TASK_PROTECTED;
 * the task might be killed after it has been created, but before it
 * gets a chance to set TASK_PROTECTED.  The caller is probably aware
 * of the special status and should be able to set it at creation time.
 */
void task_set_flags (U8 flags)
{
	task_current->state |= flags;
}


/**
 * Clears task flags.
 */
void task_clear_flags (U8 flags)
{
	task_current->state &= ~flags;
}


/**
 * Get the task argument word.
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


/**
 * Set the task argument word.
 */
void task_set_arg (task_t *tp, U16 arg)
{
	tp->arg = arg;
}


/** Allocate stack size from another task.  This should only be
called immediately after the task is created before it gets a chance
to run. */
void task_alloca (task_t *tp, U8 size)
{
	tp->stack_size = size;
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
 * then we wait until the tick count advances to indicate that 16ms
 * has elapsed.  Then we execute all of the idle functions.  This
 * means that idle functions are never called if there is always at
 * least one task in the ready state; it also limits their invocation to
 * once per 16ms.  This delay is also useful because it is the
 * minimum delay before which a blocked task might reach the end of its
 * wait period; there is no sense checking more frequently.
 */
__naked__ __noreturn__
void task_dispatcher (void)
{
	register task_t *tp asm ("x");
	task_t *first = tp;

	tick_start_count = get_ticks ();
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
			/* Call the debugger.  This is not implemented as a true
			'idle' event below because it should _always_ be called,
			even when 'sys_init_complete' is not true.  This lets us
			debug very early initialization. */
			db_idle ();
#ifdef IDLE_PROFILE
			idle_profile_idle ();
#endif	

			/* If the system is fully initialized, run
			 * the idle functions. */
			if (idle_ok)
			{
				do_idle ();
				callset_invoke (idle);
			}

			/* Reset timer and kick watchdog again */
			tick_start_count = get_ticks ();
			task_dispatching_ok = TRUE;

			/* Wait for next task tick before continuing.
			This ensures that the idle function is not invoked more than once
			per 16ms.  Do this AFTER calling the idle functions, so
			that we wait as little as possible; idle calls themselves may
			take a long time. */
			while (tick_start_count == get_ticks ())
			{
				barrier ();
#ifdef IDLE_PROFILE
				noop ();
				noop ();
				noop ();
				noop ();
				idle_time++;
#endif
			}
			
			/* Ensure that 'tp', which is in register X, is reloaded
			with the correct task pointer.  The above functions may
			trash its value. */
			tp = first;
		}

		if (tp->state & (BLOCK_TASK | TASK_BLOCKED))
		{
			/* The task exists, but is sleeping.
			 * tp->asleep holds the time at which it went to sleep,
			 * and tp->delay is the time for which it should sleep.
			 * Examine the current tick count, and see if it should
			 * be woken up.
			 */
			register U8 ticks_elapsed = get_ticks () - tp->asleep;
			if (ticks_elapsed >= tp->delay)
			{
				/* Yes, it is ready to run again. */
				tp->state &= ~TASK_BLOCKED;
				task_restore (tp);
			}
		}
		else if (tp->state & BLOCK_TASK)
		{
			/* The task exists, and is not sleeping.  It can be
			started immediately. */
			task_restore (tp);
		}
	}
}


/**
 * Initialize the task subsystem.  This transforms the caller into a
 * legitimate task, which can then sleep, yield, etc.
 */
void task_init (void)
{
	U8 to;

	/* Clean the memory for all task blocks */
	memset (task_buffer, 0, sizeof (task_buffer));

#ifdef TASK_CHAINING
	/* Initialize the free list with all tasks */
	to_free = 0;
	for (to = 0; to < NUM_TASKS-1; to++)
		task_buffer[to]->chain = to+1;
	task_buffer[NUM_TASKS]->chain = TASK_LIST_NULL;

	/* Initialize the ready and blocked lists to empty */
	to_ready = TASK_LIST_NULL;
	to_blocked = TASK_LIST_NULL;
#endif

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

#ifdef IDLE_PROFILE
	idle_time = 0;
	last_idle_time = 0;
	idle_rtt_calls = 8;
#endif

	/* Allocate a task for the first (current) thread of execution.
	 * The calling routine can then sleep and/or create new tasks
	 * after this point. */
	task_current = task_allocate ();
	task_current->gid = GID_FIRST_TASK;
	task_current->arg = 0;
}

