
#include <freewpc.h>

/*
 * This module implements a round-robin, non-realtime, non-preemptive
 * task scheduler.
 *
 * A "task" is analogous to a process.  Tasks can be created, stopped,
 * killed, etc. as one would expect.  All tasks are "peers", as no
 * parent/child relationship exists among the tasks.
 *
 * Most of the module is written in portable C.  The task structure
 * and the register save/restore routines are written in lots of
 * assembler, though.
 *
 */

__fastram__ task_t *task_current;

__fastram__ uint8_t task_dispatch_tick;

__fastram__ uint16_t task_save_U, task_save_X;

task_t task_buffer[NUM_TASKS];

U8 task_idle_count;

#define DEBUG_TASKS

#ifdef DEBUG_TASKS
uint8_t task_count;
#endif


#pragma long_branch
void task_dump (void)
{
#ifdef DEBUGGER
	register short t;
	register task_t *tp;

	db_puts ("\nCurrent ");
	db_put4x ((uint16_t)task_current);
	db_puts ("\n----------------------\n");
	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
	{
		if (tp->state != TASK_FREE)
		{
			db_puts ("PID ");
			db_put4x ((uint16_t)tp);
			db_puts ("  State ");
			db_put2x (tp->state);
			db_puts ("  GID ");
			db_put2x (tp->gid);
			db_puts ("  PC ");
			db_put4x ((uint16_t)tp->pc);
			db_puts ("  S ");
			db_put4x ((uint16_t)tp->s);
			db_puts ("  Stack ");
			db_put4x ((uint16_t)tp->stack);
			db_puts ("-");
			db_put4x ((uint16_t)tp->stack + TASK_STACK_SIZE);
			db_puts ("  ARG ");
			db_put4x ((uint16_t)tp->arg);
			db_puts ("  SB ");
			db_put4x ((uint16_t)*(uint16_t *)tp->stack);
			db_putc ('\n');
		}
	}
	db_puts ("----------------------\n");
#endif
}
#pragma short_branch


task_t *task_allocate (void)
{
	register short t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if (tp->state == TASK_FREE)
		{
			tp->state = TASK_USED;
			tp->delay = 0;
			return tp;
		}
	fatal (ERR_NO_FREE_TASKS);
	return 0;
}

#pragma naked
void task_save (void)
{
	register uint16_t __u asm ("u");
	register task_t * __x asm ("x");

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

	/* Save current stack pointer */
	__asm__ volatile ("leau\t,s");
	__x->s = __u;

	/* TODO : add test for stack overflow */

	/* Jump to the task dispatcher */
	__asm__ volatile ("jmp _task_dispatcher");
}


#pragma naked
void task_restore (void)
{
	register task_t * __x asm ("x");
	register uint16_t __u asm ("u");

	task_current = __x;
	set_stack_pointer (__x->s);

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
	*(uint16_t *)&tp->stack = 0xEEEE;

	tp->s = (uint16_t)(tp->stack + TASK_STACK_SIZE - 1);

	/* TODO?? : push the address of task_exit onto the
	 * stack so that the task can simply return and it
	 * will exit automatically.  All tasks will need
	 * to do this and this will save the code space of
	 * a function call (though replaced by stack space)
	 */

	__asm__ volatile ("puls\td,u,y,pc");
}


void task_yield (void)
{
	/* TODO : this could just be a jmp, right? */
	__asm__ volatile ("jsr _task_save");
}


task_t *task_create_gid (task_gid_t gid, task_function_t fn)
{
	register task_function_t fn_x asm ("x") = fn;
	register task_t *tp asm ("x");

	__asm__ volatile ("jsr _task_create\n" : "=r" (tp) : "0" (fn_x) : "d");
	tp->gid = gid;
	tp->arg = 0;
#ifdef DEBUG_TASKS
	task_count++;
#endif
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

#if 0 /* doesn't work for the first task created */
	if (task_current->state != TASK_USED)
		fatal (ERR_TASK_STACK_OVERFLOW);
#endif

	if (task_current->state != TASK_USED)
	{
		db_puts ("*** Warning: task_current = ");
		db_put4x ((uint16_t)task_current);
		db_puts (" state = ");
		db_put2x (task_current->state);
		db_putc ('\n');
	}

	task_current->delay = ticks;
	task_current->asleep = tick_count;
	task_current->state = TASK_BLOCKED+TASK_USED; /* was |= */

	__asm__ volatile ("jsr _task_save");
}


void task_sleep_sec (int8_t secs)
{
	while (--secs >= 0)
		task_sleep (TIME_1S);
}


void task_exit (void) __noreturn__
{
	if (task_current == 0)
		fatal (ERR_IDLE_CANNOT_EXIT);

	task_current->state = TASK_FREE;
	task_current = 0;
#ifdef DEBUG_TASKS
	task_count--;
#endif

	__asm__ volatile ("jmp _task_dispatcher");
	for (;;);
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
#ifdef DEBUG_TASKS
	task_count--;
#endif
}

bool task_kill_gid (task_gid_t gid)
{
	register short t;
	register task_t *tp;
	bool rc = FALSE;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if ((tp->state != TASK_FREE) && (tp->gid == gid))
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
void task_dispatcher (void)
{
	extern __fastram__ uint8_t tick_count;
	register task_t *tp asm ("x");

	for (tp++;; tp++)
	{
		/* If at the end of the list, execute some special
		 * system code before starting at the top again. */
		if (tp == &task_buffer[NUM_TASKS])
		{
			/* Execute idle tasks on system stack */
			set_stack_pointer (STACK_BASE);

			/* Increment counter for number of times we run
			 * the idle code. */
			task_idle_count++;

			/* Call idle tasks */
			switch_idle_task ();

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
	extern uint8_t tick_count;

	memset (task_buffer, 0, sizeof (task_buffer));
	task_idle_count = 0;

#ifdef DEBUG_TASKS
	task_count = 0;
#endif

	task_current = task_allocate ();
	task_current->state = TASK_USED;
	task_current->arg = 0;
	task_current->gid = GID_FIRST_TASK;
	task_current->delay = 0;
	__asm__ volatile ("st%0 _task_dispatch_tick" :: "q" (tick_count));
}


