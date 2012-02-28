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

/** A pointer to just beyond the last usable entry in the task table.
 * Normally, only tasks between the beginning of the table and the tail
 * will be scanned.  This makes many of the task APIs more efficient by
 * not having to scan the whole table, when many of the entries are
 * probably unused.  The table will only fill up when there are MANY
 * tasks running at once, which is not normally the case.  If the
 * table becomes full, the tail will extend out allowing more entries
 * to be used.  Periodically, we can scan the entire table to see if
 * is possible to rewind the tail once the number of tasks goes down again.
 */
task_t *task_tail;

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

__fastram__ U16 last_dispatch_time;

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

/** A count that represents accumulated idle time.
 * Whenever there are no tasks to be scheduled, this counter
 * is incremented.  Each tick represents about 32 CPU cycles.
 */
U16 idle_time;

/**
 * A conversion and snapshot of the above idle_time measurement.
 */
U8 idle_chunks;


/**
 * The number of task slots that are allocated/free at a time.
 */
#define TASK_CHUNK_SIZE 8

/**
 * The number of tasks that must be free in order for a chunk to
 * be freed.
 */
#define TASK_FREE_CHUNK_SIZE 10


/* Private functions written in assembly used internally. */
__attribute__((returns_twice)) void task_save (task_t *tp);
__noreturn__ void task_restore (task_t *tp);


/**
 * Take a snapshot of the number of idle cycles.
 *
 * This function MUST be done in interrupt context to be effective.  It runs
 * once per second.
 */
void idle_profile_rtt (void)
{
	/* Divide the idle tick count by 256 to smooth out the
	readings, ignore small fluctuations.
		Each of these "idle chunks" is now roughly 32x256 = 8192 CPU cycles, or
	about 4ms.
		If the system is completely busy, the value will be 0.  If the system were
	completely idle (impossible though), it would be 250.  As it turns out,
	the scaled value cannot overflow so 8-bits is sufficient.
		Divide idle_chunks by 2.5 for the percentage of CPU time that is free.
	*/
	idle_chunks = idle_time >> 8UL;
	idle_time = 0;
}


/**
 * When IDLE_PROFILE is defined, print out the
 * idle profiling data periodically.  (If not defined, the value
 * is still available in memory.)
 */
CALLSET_ENTRY (idle_profile, idle_every_100ms)
{
#ifdef IDLE_PROFILE
	/* Note that idle_chunks is unlikely to ever be 0xFF (a very idle
	system).  So we use that value to mark that we have printed the
	last computed value.  This function runs more frequently than
	chunks is computed, but only prints once per new value (every second). */
	if (unlikely (idle_chunks != 0xFF))
	{
		dbprintf ("I:%02X\n", idle_chunks);
		idle_chunks = 0xFF;
	}
#endif
}


/**
 * Called whenever the CPU is truly idle; there are no tasks to be
 * scheduled and all idle callbacks have returned.
 */
static inline void cpu_idle (void)
{
	barrier ();

	/* For profiling, keep a count of the number of times
	we have nothing to do.
		Each tick of this 16-bit counter is approximately 23 CPU cycles.
	That equates to 87 ticks per 1ms; therefore it will overflow
	after about 750ms. */
	idle_time++;
}


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
	for (t=0, tp = task_buffer; tp < task_tail; t++, tp++)
	{
		if (tp->state != BLOCK_FREE)
		{
			dbprintf ("%p: ", tp);
			task_dispatching_ok = TRUE;

			if (tp->state & BLOCK_TASK)
			{
				dbprintf ("DUR %02X  ", tp->duration);
				dbprintf ("GID %02d  PC %p", tp->gid, tp->pc);
				dbprintf ("  ST %02X", tp->stack_size);
				dbprintf ("  ARG %04X\n", tp->arg);
			}
#ifdef CONFIG_MALLOC
			else if (tp->state & BLOCK_MALLOC)
			{
				malloc_chunk_dump (tp);
			}
#endif
#ifdef CONFIG_EXPAND_STACK
			else if (tp->state & BLOCK_STACK)
			{
				dbprintf ("aux stack\n");
			}
#endif
			else
			{
				dbprintf ("???\n");
			}
		}
	}
	dbprintf ("task_tail = %p\n\n", task_tail);
#endif
}


/** Allocate a dynamic block of memory, by searching the block
 * array (linearly) for the first entry that is in the BLOCK_FREE
 * state.  Returns a pointer to the newly allocated block, or
 * NULL if none can be found.
 *
 * The 'aux_stack_block' field of a new block is initialized to
 * that block's index.
 */
task_t *block_allocate (void)
{
	register U8 t;
	register task_t *tp;

	/* Scan the table for an unused block */
	for (t = 0, tp = task_buffer; tp < task_tail; t++, tp++)
	{
		if (tp->state == BLOCK_FREE)
		{
success:
			tp->state = BLOCK_USED;
#ifdef CONFIG_EXPAND_STACK
			tp->aux_stack_block = t;
#endif
			return tp;
		}
	}

	/* No block was found, but maybe the tail pointer can be extended. */
	if (task_tail < &task_buffer[NUM_TASKS])
	{
		/* The tail points to the first unused entry, so it must be free. */
		tp = task_tail;
		task_tail += TASK_CHUNK_SIZE;
		goto success;
	}

	/* The table is truly full.  Return an error. */
	return NULL;
}


/**
 * Scan the task table to see if the tail pointer can be rewound,
 * so that fewer task entries need to be scanned when doing things
 * like task_find_gid().  There's no need to do very often.
 */
CALLSET_ENTRY (task_chunk, amode_start, end_ball)
{
	U8 free_count;
	task_t *tp;

	/* Only try this when the tail is not already at its minimum. */
	if (task_tail <= &task_buffer[TASK_CHUNK_SIZE])
		return;

	/* Count how many free task slots exist from the top of the table,
	moving down.  Stop scanning if we reach the minimum tail value. */
	free_count = 0;
	tp = task_tail-1;
	while (tp->state == BLOCK_FREE)
	{
		free_count++;
		tp--;
		if (tp == &task_buffer[TASK_CHUNK_SIZE])
			break;
	}

	/* See if enough tasks are free to warrant moving the tail pointer.
	We require that a few more tasks are free than the chunk size, to
	avoid constant expansion/shrinking if the task count is equal to
	the chunk size. */
	while (free_count >= TASK_FREE_CHUNK_SIZE)
	{
		task_tail -= TASK_CHUNK_SIZE;
		free_count -= TASK_CHUNK_SIZE;

		/* GCC6809 is trying to optimize away this loop
		and turn it into just two subtractions, but
		the code generation is pretty bad.  Force a
		barrier here to prevent that. */
#ifdef __m6809__
		barrier ();
#endif
	}
}


/** Free a dynamic block of memory. */
#ifndef CONFIG_MALLOC
static
#endif
void block_free (task_t *tp)
{
	tp->state = BLOCK_FREE;
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
		tp->stack_size = 0;
#ifdef CONFIG_EXPAND_STACK
		tp->aux_stack_block = -1;
#endif
		tp->duration = TASK_DURATION_BALL;
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

		/* At present, just halt the system when this happens.
		No callers are checking return codes at this point. */
		fatal (ERR_NO_FREE_TASKS);
		return 0;
	}
}


#ifdef CONFIG_EXPAND_STACK
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
#endif


/** Free a task block for a task that no longer exists. */
static void task_free (task_t *tp)
{
#ifdef CONFIG_EXPAND_STACK
	/* Free the auxiliary stack block first if it exists */
	if (tp->aux_stack_block != -1)
		block_free (&task_buffer[tp->aux_stack_block]);
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
	tp->wakeup = 0;
	tp->arg.u16 = 0;
#ifdef CONFIG_DEBUG_TASKCOUNT
	task_count++;
	if (task_count > task_max_count)
		task_max_count = task_count;
#endif
	log_event (SEV_DEBUG, MOD_TASK, EV_TASK_START, gid);
	return (tp);
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

	/* Fail if a periodic function tries to sleep. */
	if (tp == 0)
		fatal (ERR_IDLE_CANNOT_SLEEP);

#ifdef PARANOID
	/* TODO - verify that interrupts are not disabled when calling this */
#endif

	/* Mark the task as blocked, and set the time at which it
	should be awakened. */
	tp->wakeup = get_sys_time () + ticks;
	tp->state |= TASK_BLOCKED;

	/* Save the task, and start another one.  This call returns
	whenever the sleeping task is eventually unblocked. */
	task_save (tp);
}


/** Suspend the current task for a number of seconds.
	This function is not normally called directly; use task_sleep_sec()
instead.  If the number of seconds is small enough, it will call
task_sleep() instead, which is more efficient. */
void task_sleep_sec1 (U8 secs)
{
	register task_t *tp = task_current;
	if (tp == 0)
		fatal (ERR_IDLE_CANNOT_SLEEP);
	tp->wakeup = get_sys_time () + ((U16)secs * TIME_1S);
	tp->state |= TASK_BLOCKED;
	task_save (tp);
}


/** Set the time at which the task should be awakened, AFTER
the next call to task_sleep_periodic().  Together, these two
functions allow a task to specify its timeout in advance of
doing work, which allows it to be regularly scheduled. */
void task_set_periodic (task_ticks_t ticks)
{
	task_current->wakeup = get_sys_time () + ticks;
}


/** Sleep, using the wakeup time already calculated. */
void task_sleep_periodic (void)
{
	register task_t *tp = task_current;
	tp->state |= TASK_BLOCKED;
	task_save (tp);
}


/**
 * Exit the current task, and return to the dispatcher to select
 * another task to start running now.
 */
__naked__ __noreturn__ 
void task_exit (void)
{
	log_event (SEV_DEBUG, MOD_TASK, EV_TASK_EXIT, task_current->gid);
	if (task_current == 0)
		fatal (ERR_IDLE_CANNOT_EXIT);

	task_free (task_current);
#ifdef CONFIG_DEBUG_TASKCOUNT
	task_count--;
#endif
	task_dispatcher (task_current);
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
	for (tp = last+1; tp < task_tail; tp++)
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
 */
void task_kill_pid (task_t *tp)
{
	if (tp == task_current)
		fatal (ERR_TASK_KILL_CURRENT);

	task_free (tp);
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
	register task_t *tp;
	bool rc = FALSE;

	log_event (SEV_DEBUG, MOD_TASK, EV_TASK_KILL, gid);
	for (tp = task_buffer; tp < task_tail; tp++)
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
 * Asserts that a task-ending condition has just occurred,
 * and kills all of the tasks that needs to be.
 */
void task_duration_expire (U8 cond)
{
	register task_t *tp;

	for (tp = task_buffer; tp < task_tail; tp++)
		if ((tp->state & BLOCK_TASK) && (tp->duration & cond))
		{
			task_kill_pid (tp);
		}
}


/**
 * Sets the duration for a given task.
 */
void task_set_duration (task_t *tp, U8 cond)
{
	tp->duration = cond;
}


/**
 * Add/remove duration flags.
 */
void task_add_duration (U8 cond)
{
	task_current->duration |= cond;
}


/**
 * Clears task flags.
 */
void task_remove_duration (U8 cond)
{
	task_current->duration &= ~cond;
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
	return task_current->arg.u16;
}


void *task_get_pointer_arg (void)
{
	return task_current->arg.ptr;
}


/**
 * Set the task argument word.
 */
void task_set_arg (task_t *tp, U16 arg)
{
	tp->arg.u16 = arg;
}


void task_set_pointer_arg (task_t *tp, void *arg)
{
	tp->arg.ptr = arg;
}


/** Allocate stack size from another task.  This should only be
called immediately after the task is created before it gets a chance
to run. */
void *task_alloca (task_t *tp, U8 size)
{
	tp->stack_size += size;
	return &tp->stack[TASK_STACK_SIZE - tp->stack_size];
}


/**
 * The task dispatcher.  This function selects a new task to run.
 *
 * This is called from two places: when a task exits, or when a
 * task sleeps/yields.  The parameter 'tp' points to
 * the previous task's task structure pointer.  The search for a new
 * task to run starts with the next task in the table.  Code then
 * jumps back to the first candidate that is eligible, via
 * the task_restore() assembly language routine.
 *
 * After the last entry in the task table is scanned, we execute all of the
 * periodic functions.  These functions do not run in task context and cannot
 * sleep.  They are for fixed system components that always need to be
 * scheduled.
 *
 * After the periodic functions finish, we ensure that the system time
 * (in 16ms units) has advanced at least 1 tick before dispatching again from
 * the top of the task table.  This ensures that the periodic functions do
 * not run more often than once per 16ms.
 *
 * Historical note: in earlier versions of FreeWPC, periodic functions were
 * called "idle functions", and they would only run if no tasks were queued.
 * That is, idle functions could be starved by running tasks.  Now, the
 * periodic functions are guaranteed to run more frequently, but there is
 * no guarantee _how_ frequently.
 */
__noreturn__
void task_dispatcher (task_t *tp)
{
	task_dispatching_ok = TRUE;
	task_current = 0;

	/* Go into an infinite loop looking for a task ready to run.
	Start with the task after the one that was just saved. */
	for (tp++; ; tp++)
	{
		/* When we reach the end of the task table, call the
		periodic functions. */
		if (unlikely (tp == task_tail))
		{
			/* Call the debugger.  This is not implemented as a true
			'idle' event below because it should _always_ be called,
			even when 'periodic_ok' is not true.  This lets us
			debug very early initialization. */
			db_periodic ();

			/* If the system is fully initialized, run the periodic functions. */
			if (likely (periodic_ok))
				do_periodic ();

			/* Wait for time to change before continuing.  This ensures that
			the task table+periodic functions are not scanned/called more
			frequently than once per 16ms. */
			while (likely (last_dispatch_time == get_sys_time ()))
				cpu_idle ();
			last_dispatch_time = get_sys_time ();
			task_dispatching_ok = TRUE;
			
			/* Reset to the beginning of the task table */
			tp = &task_buffer[0];
		}

		/* Only scan blocks that are currently used by a task.  This skips free blocks
		and blocks used for other purposes. */
		if (tp->state & (BLOCK_USED | BLOCK_TASK))
		{
			/* See if the task is asleep and should be enabled again.
			Compare the time at which it wants to wake up with the current time.
			The subtraction should yield a negative value when it is ready.  If
			still not ready, then move on to the next task. */
			if (tp->state & TASK_BLOCKED)
			{
				if (time_reached_p (tp->wakeup))
					tp->state &= ~TASK_BLOCKED;
				else
					continue;
			}

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
	/* Clean the memory for all task blocks */
	/* Note - removing this because the startup RAM test clears the memory
	already, and this is taking a long time to finish which can cause the
	hardware watchdog to expire. */
	// memset (task_buffer, 0, sizeof (task_buffer));

	/* No dispatching lockups so far */
	task_dispatching_ok = TRUE;

	/* Initialize the tail pointer.  The table is divided into
	chunks of TASK_CHUNK_SIZE tasks each; we initially only allow
	allocation from a single chunk.  If it becomes full, then we
	will add another chunk, and so on. */
	task_tail = &task_buffer[TASK_CHUNK_SIZE];

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

	last_dispatch_time = 0;
	idle_time = 0;

	/* Allocate a task for the first (current) thread of execution.
	 * The calling routine can then sleep and/or create new tasks
	 * after this point. */
	task_current = task_allocate ();
	task_current->gid = GID_FIRST_TASK;
	task_current->arg.u16 = 0;
}

