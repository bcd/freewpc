
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
 * and the register save/restore routines are written in assembler.
 *
 */

__fastram__ task_t *task_current;

__fastram__ uint8_t task_dispatch_tick;

__fastram__ uint16_t task_save_U, task_save_X;

task_t task_buffer[NUM_TASKS];

#define DEBUG_TASKS

#ifdef DEBUG_TASKS
uint8_t task_count;
#endif


void task_dump (void)
{
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
}


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
#if 0
	asm __volatile__ ("stu	_task_save_U");
	asm __volatile__ ("stx	_task_save_X");
	asm __volatile__ ("puls	u");
	asm __volatile__ ("ldx	_task_current");
	asm __volatile__ ("stu	%0" :: "o" (task_current->pc));
	asm __volatile__ ("ldu	_task_save_U");
	asm __volatile__ ("stu	%0" :: "o" (task_current->u));
	asm __volatile__ ("ldu	_task_save_X");
	asm __volatile__ ("stu	%0" :: "o" (task_current->x));
	asm __volatile__ ("sta	%0" :: "o" (task_current->a));
	asm __volatile__ ("stb	%0" :: "o" (task_current->b));
	asm __volatile__ ("sty	%0" :: "o" (task_current->y));
	asm __volatile__ ("leau	,s");
	/* asm __volatile__ ("stu	TASK_OFF_S,x"); */
	__asm__ volatile ("jmp _task_dispatcher");
#endif
}


#pragma naked
void task_restore (void)
{
}

void task_yield (void)
{
	/* TODO : this could just be a jmp, right? */
	__asm__ volatile ("jsr task_save");
}


task_t *task_create_gid (task_gid_t gid, task_function_t fn)
{
	register task_function_t fn_x asm ("x") = fn;
	register task_t *tp asm ("x");

	__asm__ volatile ("jsr task_create\n" : "=r" (tp) : "0" (fn_x));
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

task_t *task_getpid (void)
{
	return task_current;
}

task_gid_t task_getgid (void)
{
	return task_current->gid;
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

	__asm__ volatile ("jsr task_save");
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

void task_kill_gid (task_gid_t gid)
{
	register short t;
	register task_t *tp;

	for (t=0, tp = task_buffer; t < NUM_TASKS; t++, tp++)
		if ((tp->state != TASK_FREE) && (tp->gid == gid))
			task_kill_pid (tp);
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
			asm volatile ("jmp task_restore");
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
				asm volatile ("jmp task_restore");
			}
		}
	}
}

void task_init (void)
{
	extern uint8_t tick_count;

	memset (task_buffer, 0, sizeof (task_buffer));

#ifdef DEBUG_TASKS
	task_count = 0;
#endif

	task_current = task_allocate ();
	task_current->state = TASK_USED;
	task_current->arg = 0;
	task_current->gid = GID_FIRST_TASK;
	__asm__ volatile ("st%0 _task_dispatch_tick" :: "q" (tick_count));
}


