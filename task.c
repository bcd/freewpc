
#include <wpc.h>
#include <sys/task.h>

__fastram__ task_t *task_current;

__fastram__ uint8_t task_dispatch_tick;

task_t task_buffer[NUM_TASKS];



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
}


void task_yield (void)
{
	__asm__ volatile ("jsr task_save");
}

void task_create (task_function_t fn, uint16_t arg)
{
}

void task_create_gid (task_gid_t gid, task_function_t fn, uint16_t arg)
{
	register task_function_t fn_x asm ("x") = fn;
	register task_t *tp asm ("x");

	__asm__ volatile ("jsr task_create\n" : "=r" (tp) : "0" (fn_x));
	tp->gid = gid;
	tp->arg = arg;
}

void task_create_gid1 (task_gid_t gid, task_function_t fn, uint16_t arg)
{
	if (!task_find_gid (gid))
		task_create_gid (gid, fn, arg);
}

void task_recreate_gid (task_gid_t gid, task_function_t fn, uint16_t arg)
{
	task_kill_gid (gid);
	task_create_gid (gid, fn, arg);
}

task_gid_t task_getgid (void)
{
	return task_current->gid;
}

void task_sleep (task_ticks_t ticks)
{
	extern uint8_t tick_count;

	if (task_current == 0)
		fatal (ERR_IDLE_CANNOT_SLEEP);

	task_current->delay = ticks;
	task_current->asleep = tick_count;
	task_current->state |= TASK_BLOCKED;

	__asm__ volatile ("jsr task_save");
}


void task_exit (void) __noreturn__
{
	if (task_current == 0)
		fatal (ERR_IDLE_CANNOT_EXIT);

	task_current->state = 0;
	task_current = 0;
	asm ("jmp task_dispatcher");
}

task_t *task_find_gid (task_gid_t gid)
{
	register task_t *tp asm ("x");

	__asm__ volatile ("tfr b,a\njsr task_find_gid\n" : "=r" (tp) : "q" (gid));
	return (tp);
}

void task_kill_pid (task_t *tp)
{
	if (tp == task_current)
		fatal (ERR_TASK_KILL_CURRENT);
	tp->state = 0;
}

void task_kill_gid (task_gid_t gid)
{
	task_t *tp = task_find_gid (gid);
	if (tp)
		task_kill_pid (tp);
}


void task_init (void)
{
	int t;
	extern uint8_t tick_count;

	for (t=0; t < NUM_TASKS; t++)
	{
		task_buffer[t].state = TASK_FREE;
	}

	task_current = task_allocate ();
	__asm__ volatile ("st%0 _task_dispatch_tick" :: "q" (tick_count));
}

