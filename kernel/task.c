
#include <freewpc.h>

/*
 * This module implements a round-robin, non-realtime, non-preemptive
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
 */


/* task_current points to the control structure for the current task. */
__fastram__ task_t *task_current;

/* When saving a task's registers during dispatch, we need some
 * static storage to help compensate for the lacking of registers.
 * Don't allocate on the stack, since we need to preserve the
 * stack pointer carefully during dispatch. */
__fastram__ uint16_t task_save_U, task_save_X;

/* The static array of task structures */
task_t task_buffer[NUM_TASKS];

/* A flag that indicates that dispatching is working as expected.
 * This is set to 1 everytime we dispatch correctly, and to 0
 * periodically from the IRQ.  If the IRQ finds it at 0, that
 * means we went a long time without a dispatch; this probably
 * means that (1) some task has been running for a very long time,
 * or (2) the task is jumped into the weeds and is never coming back.
 * Case (1) is theoretically OK, but we consider it just as bad as
 * case (2), which could lead to all kinds of weird behavior.
 * The IRQ will reset the system when this happens. */
bool task_dispatching_ok;

/* For debug, this tells us the largest stack size that we've had
 * to deal with so far.  This helps to determine how big the stack
 * area in the task structure needs to be */
U8 task_largest_stack;

/* Uncomment this to turn on dumping of entire task table.
 * Normally, only the running entries are displayed. */
//#define DUMP_ALL_TASKS


void task_dump (void)
{
#ifdef DEBUGGER
	register long int t;
	register task_t *tp;

	dbprintf ("\nCurrent = %p\n", task_current);
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


/*
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
			return tp;
		}
	fatal (ERR_NO_FREE_TASKS);
	return 0;
}


/*
 * Save the current execution state into the current task block.
 */
#pragma naked
void task_save (void)
{
	register uint16_t __u asm ("u");
	register task_t * __x asm ("x");
	register U8 __b asm ("d");

	/* Save U, X immediately to memory to free up some regs for
	 * the rest of the function */
	task_save_U = (uint16_t)__u;
	task_save_X = (uint16_t)__x;

	/* Get the PC by popping it off the stack */
	__asm__ volatile ("puls\tu");

	/* Force load of the current task structure into X */
	__x = task_current;

	/* Save PC (in U) */
	__x->pc = __u;

	/* Save U (in task_save_U).  Use U to hold the data. */
	__x->u = __u = task_save_U;

	/* Save X (in task_save_X).  Use U to hold the data. */
	__x->x = __u = task_save_X;

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

	if ((__x->stack_word_count = __b) > task_largest_stack)
	{
		task_largest_stack = __b;
	}

	/* Save current ROM page */
	__x->rom_page = wpc_get_rom_page ();

	/* TODO : add test for stack overflow */

	/* Jump to the task dispatcher */
	__asm__ volatile ("jmp _task_dispatcher");
}


#pragma naked
void task_restore (void)
{
	register task_t * __x asm ("x");
	register uint16_t __u asm ("u") __attribute__ ((unused));
	register U8 __b asm ("d");

	task_current = __x;

	/* Restore stack */
	disable_irq ();
	disable_firq ();
	/* Get live stack area pointer in u */
	__asm__ volatile ("ldu\t%0" :: "i" (STACK_BASE));

	/* Get stack save area pointer in y */
	__asm__ volatile ("leay\t,%0" :: "a" (__x->stack + __x->stack_word_count));

	/* Copy */
	__b = __x->stack_word_count;
	while (__b != 0)
	{
		/* Use X register to transfer data */
		__asm__ volatile ("lda\t,-y");
		__asm__ volatile ("sta\t,-u");
		__b --;
	}

	/* Save stack pointer to S */
	__asm__ volatile ("leas\t,u");

	/// /* Restore __x as task_current */
	/// __x = task_current;

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

	__asm__ volatile ("ldx	%0" :: "m" (__x->x));

	__asm__ volatile ("rts");
}


#pragma naked
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


task_t *task_create_gid (task_gid_t gid, task_function_t fn)
{
	register task_function_t fn_x asm ("x") = fn;
	register task_t *tp asm ("x");

	__asm__ volatile ("jsr\t_task_create" : "=r" (tp) : "0" (fn_x) : "d");
	tp->gid = gid;
	tp->arg = 0;
	return (tp);
}

#if 00000
struct task_create_gid_args
{
	task_gid_t gid;
	task_function_t fn;
};
task_t *task_create_gid_const (uint8_t unused)
{
	typedef struct task_create_gid_args *argptr;
	volatile argptr *args = (argptr *)(&unused - 2);
	argptr arg = *args;
	task_t *tp = task_create_gid (arg->gid, arg->fn);
	*args = arg + 3;
	return tp;
}
#endif


task_t *task_create_gid1 (task_gid_t gid, task_function_t fn)
{
	task_t *tp = task_find_gid (gid);
	if (tp) 
		return (tp);
	return task_create_gid (gid, fn);
}


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

#if 00000
	{
		dbprintf ("task %p awake (saved %d), pc =", 
			task_current, task_current->stack_word_count);
		__asm__ volatile ("ldd\t3,s\n\tjsr\t_db_put4x\n");
		db_putc ('\n');
	}
#endif
}


void task_sleep_sec (int8_t secs)
{
	while (--secs >= 0)
		task_sleep (TIME_1S);
}


#pragma naked
__noreturn__ void task_exit (void)
{
	if (task_current == 0)
		fatal (ERR_IDLE_CANNOT_EXIT);

	task_current->state = TASK_FREE;
	task_current = 0;
	task_dispatcher ();
}

task_t *task_find_gid (task_gid_t gid)
{
	register short t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if ((tp->state != TASK_FREE) && (tp->gid == gid))
			return (tp);
	return (NULL);
}

void task_kill_pid (task_t *tp)
{
	if (tp == task_current)
		fatal (ERR_TASK_KILL_CURRENT);
	tp->state = TASK_FREE;
	tp->gid = 0;
}

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


uint16_t task_get_arg (void)
{
	return task_current->arg;
}


void task_set_arg (task_t *tp, uint16_t arg)
{
	tp->arg = arg;
}

#pragma naked
void __attribute__((noreturn)) task_dispatcher (void)
{
	extern uint8_t tick_count;
	register task_t *tp asm ("x");

	for (tp++;; tp++)
	{
		/* Increment counter for number of times we run
		 * the dispatcher code. */
		task_dispatching_ok = TRUE;

		/* If at the end of the list, execute some special
		 * system code before starting at the top again. */
		if (tp == &task_buffer[NUM_TASKS])
		{
			extern void nvram_idle_task (void);

			/* Execute idle tasks on system stack */
			set_stack_pointer (STACK_BASE);

			/* Call idle tasks */
			switch_idle_task ();
			ac_idle_task ();
			nvram_idle_task ();

			tp = &task_buffer[0];
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


void task_init (void)
{
	/* Clean the memory for all task blocks */
	memset (task_buffer, 0, sizeof (task_buffer));

	/* No dispatching lockups so far */
	task_dispatching_ok = TRUE;

	/* Allocate a task for the first (current) thread of execution.
	 * The calling routine can then sleep and/or create new tasks
	 * after this point. */
	task_current = task_allocate ();
	task_current->state = TASK_USED;
	task_current->arg = 0;
	task_current->gid = GID_FIRST_TASK;
	task_current->delay = 0;
}


