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
 * tasks in the group, e.g.
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

/** Uncomment this to turn on dumping of entire task table.
 * Normally, only the running entries are displayed. */
//#define DUMP_ALL_TASKS


void task_dump (void)
{
#ifdef DEBUGGER
	register long int t;
	register task_t *tp;

	dbprintf ("\nCurrent = %p\n", task_current);
#ifdef CONFIG_DEBUG_TASKCOUNT
	dbprintf ("Max tasks = %d\n", task_max_count);
#endif
	db_puts ("----------------------\n");
	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
	{
#ifndef DUMP_ALL_TASKS
		if (tp->state != TASK_FREE)
#endif
		{
			dbprintf ("PID %p  State %02X  GID %02X  PC %p",
				tp, tp->state, tp->gid, tp->pc);
			db_puts ("  STKW ");
			db_put2x (tp->stack_word_count);
			dbprintf ("  ARG %04X\n", tp->arg);
		}
	}
	db_puts ("----------------------\n");
#endif
}


/**
 * Allocate a new task block, by searching the array (linearly)
 * for the first entry that is TASK_FREE.
 */
task_t *task_allocate (void)
{
	register short t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if ((tp->state == TASK_FREE) && (tp != task_current))
		{
			tp->state = TASK_USED;
			tp->delay = 0;
			tp->stack_word_count = 0;
			tp->flags = 0;
			return tp;
		}
	fatal (ERR_NO_FREE_TASKS);
	return 0;
}


#ifndef TASK_LL_SUPPORT
/**
 * Save the current execution state into the current task block.
 */
__naked__
void task_save (void)
{
	register U16 __u asm ("u");
	register task_t * __x asm ("x");
	register U8 __b asm ("d");

	/* Save U, X immediately to memory to free up some regs for
	 * the rest of the function */
	task_save_U = (U16)__u;
#ifdef SAVE_X
	task_save_X = (U16)__x;
#endif

	/* Get the PC by popping it off the stack */
	__asm__ volatile ("puls\tu");

	/* Force load of the current task structure into X */
	__x = task_current;

	/* Save PC (in U) */
	__x->pc = __u;

	/* Save U (in task_save_U).  Use U to hold the data. */
	__x->u = __u = task_save_U;

#ifdef SAVE_X
	/* Save X (in task_save_X).  Use U to hold the data. */
	__x->x = __u = task_save_X;
#endif

	/* Save D and Y, already in registers */
#if 0 /* A,B are volatile and do not need to be saved */
	__asm__ volatile ("sta	%0" :: "m" (__x->a));
	__asm__ volatile ("stb	%0" :: "m" (__x->b));
#endif
	__asm__ volatile ("sty	%0" :: "m" (__x->y));

	/* Save current stack */
	/* In the new scheme, tasks execute on a system stack, and
	 * during swap out, the stack data is copied into the task
	 * block.  This allows tasks to use large stack space as
	 * long as they don't sleep in the middle of such usage.
	 * This also protects the task data a little better; stack
	 * overflows won't corrupt anything critical.
	 */

	/* Get current stack pointer in u */
	__asm__ volatile ("leau\t,s");

	/* Get stack save area pointer in y */
	__asm__ volatile ("leay\t,%0" :: "a" (__x->stack));

	__b = 0;
	while (__u < STACK_BASE)
	{
		/* TODO : use X register to transfer data faster */
		__asm__ volatile ("lda\t,u+");
		__asm__ volatile ("sta\t,y+");
		__b ++;
		/* TODO : check for overflow during copy */
	}

#ifdef CONFIG_DEBUG_STACK
	if ((__x->stack_word_count = __b) > task_largest_stack)
		task_largest_stack = __b;

#if 0
	if (__b > 32)
		task_large_stacks++;
	else if (__b > 16)
		task_medium_stacks++;
	else
		task_small_stacks++;
#endif

#else
	__x->stack_word_count = __b;
#endif

	/* Save current ROM page */
	__x->rom_page = wpc_get_rom_page ();

	/* TODO : add test for stack overflow */

	/* Jump to the task dispatcher */
	__asm__ volatile ("jmp _task_dispatcher");
}


__naked__
void task_restore (void)
{
	register task_t * __x asm ("x");
	register U16 __u asm ("u") __attribute__ ((unused));
	register U8 __b asm ("d");

	task_current = __x;

	/* Restore stack.  This must be done with interrupts disabled. */
	disable_irq ();
	disable_firq ();

	/* Get stack save area pointer in y */
	__asm__ volatile ("leay\t,%0" :: "a" (__x->stack + __x->stack_word_count));

	/* Get live stack area pointer in u.
	 * Note, this must be done AFTER the previous statement, since it might
	 * use the U register in the calculation (seen in newer GCC builds).
	 */
	__asm__ volatile ("ldu\t%0" :: "i" (STACK_BASE));

	/* Copy */
	__b = __x->stack_word_count;
	while (__b != 0)
	{
		/* TODO : use X register to transfer data */
		__asm__ volatile ("lda\t,-y");
		__asm__ volatile ("sta\t,-u");
		__b --;
	}

	/* Save stack pointer to S */
	__asm__ volatile ("leas\t,u");

	enable_firq ();
	enable_irq ();

	/* Restore ROM page register */
	wpc_set_rom_page (__x->rom_page);

	__asm__ volatile ("ldu	%0" :: "m" (__x->pc));
	__asm__ volatile ("pshs\tu");
	__asm__ volatile ("ldy	%0" :: "m" (__x->y));
#if 0
	__asm__ volatile ("lda	%0" :: "m" (__x->a));
	__asm__ volatile ("ldb	%0" :: "m" (__x->b));
#endif
	__asm__ volatile ("ldu	%0" :: "m" (__x->u));

	__x->delay = 0;

#ifdef SAVE_X
	__asm__ volatile ("ldx	%0" :: "m" (__x->x));
#endif

	__asm__ volatile ("rts");
}


__naked__
void task_create (void)
{
	register task_t *tp asm ("x");

	__asm__ volatile ("pshs\td,u,y");
	
	__asm__ volatile ("pshs\td,u");
	__asm__ volatile ("tfr\tx,u");

	tp = task_allocate ();

	__asm__ volatile ("stu	%0" :: "m" (tp->pc));

	__asm__ volatile ("puls\td,u");

#if 0
	__asm__ volatile ("sta	%0" :: "m" (tp->a));
	__asm__ volatile ("stb	%0" :: "m" (tp->b));
#endif
	__asm__ volatile ("sty	%0" :: "m" (tp->y));
	__asm__ volatile ("stu	%0" :: "m" (tp->u));

	tp->gid = 0;
	tp->arg = 0;
	tp->rom_page = wpc_get_rom_page ();
	tp->stack_word_count = 0;

	/* TODO?? : push the address of task_exit onto the
	 * stack so that the task can simply return and it
	 * will exit automatically.  All tasks will need
	 * to do this and this will save the code space of
	 * a function call (though replaced by stack space)
	 */

	__asm__ volatile ("puls\td,u,y,pc");
}
#endif /* TASK_LL_SUPPORT */


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

void task_setgid (task_gid_t gid)
{
	task_current->gid = gid;
}

void task_sleep (task_ticks_t ticks)
{
	extern uint8_t tick_count;

	if (task_current == 0)
		fatal (ERR_IDLE_CANNOT_SLEEP);

	if (task_current->state != TASK_USED)
	{
		dbprintf ("Warning: task_current = %p, state = %02X\n",
			task_current, task_current->state);
	}

	task_current->delay = ticks;
	task_current->asleep = tick_count;
	task_current->state = TASK_BLOCKED+TASK_USED; /* was |= */

	__asm__ volatile ("jsr\t_task_save");
}


void task_sleep_sec (int8_t secs)
{
	while (secs > 0)
	{
		if (secs == 1)
		{
			task_sleep (TIME_1S);
			return;
		}
		else
		{
			secs -= 2;
			task_sleep (TIME_2S);
		}
	}
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
 * this API.
 *
 * If no task is found, then NULL is returned; otherwise, a pointer
 * to the task structure is returned.
 */
task_t *task_find_gid (task_gid_t gid)
{
	register short t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if ((tp->state != TASK_FREE) && (tp->gid == gid))
			return (tp);
	return (NULL);
}


task_t *task_find_gid_data (task_gid_t gid, U8 off, U8 val)
{
	register short t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
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
	register short t;
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
	register short t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if (	(tp != task_current) &&
				(tp->state != TASK_FREE) && 
				!(tp->flags & TASK_PROTECTED) )
		{
			task_kill_pid (tp);
		}
}


void task_set_flags (U8 flags)
{
	task_current->flags |= flags;
}


void task_clear_flags (U8 flags)
{
	task_current->flags &= ~flags;
}


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
 */
__naked__ __noreturn__
void task_dispatcher (void)
{
	extern U8 tick_count;
	register task_t *tp asm ("x");
	U8 tick_start_count;

	tick_start_count = tick_count;
	for (tp++;; tp++)
	{
		/* Increment counter for number of times we run
		 * the dispatcher code. */
		task_dispatching_ok = TRUE;

		/* Reset task pointer to top of list after scanning
		 * them all. */
		if (tp == &task_buffer[NUM_TASKS])
		{
			/* Wait for next task tick before continuing */
			while (tick_start_count == *(volatile U8 *)&tick_count);
	
			/* If the system is fully initialized, also
			 * run the idle tasks once every pass through
			 * the list. */
			if (sys_init_complete)
			{
				extern void nvram_idle_task (void);

				/* Execute idle tasks on system stack */
				set_stack_pointer (STACK_BASE);
	
				/* Call idle tasks */
#ifdef DEBUGGER
				db_idle_task ();
#endif
				callset_invoke (idle);
				/* TODO - change all of these to event catchers */
				switch_idle_task ();
				ac_idle_task ();
				nvram_idle_task ();
				rtc_idle_task ();
			} 

			/* Reset to beginning of the task list */
			tp = &task_buffer[0];
			tick_start_count = tick_count;
		}

		if (tp->state == TASK_FREE)
		{
			continue;
		}
		else if (tp->state == TASK_USED)
		{
			asm volatile ("jmp _task_restore");
		}
		else if (tp->state != TASK_USED+TASK_BLOCKED)
		{
			continue;
		}
		else 
		{
			register uint8_t ticks_elapsed = tick_count - tp->asleep;
			if (ticks_elapsed < tp->delay)
				continue;
			else
			{
				tp->state &= ~TASK_BLOCKED;
				asm volatile ("jmp _task_restore");
			}
		}
	}
}


/**
 * Initialize the task subsystem.
 */
void task_init (void)
{
	/* Clean the memory for all task blocks */
	memset (task_buffer, 0, sizeof (task_buffer));

	/* No dispatching lockups so far */
	task_dispatching_ok = TRUE;

	/* Init debugging of largest stack */
#ifdef CONFIG_DEBUG_STACK
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

