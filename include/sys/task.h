
#ifndef _SYS_TASK_H
#define _SYS_TASK_H

#include <env.h>
#include <sys/types.h>

#define TASK_FREE		0
#define TASK_USED		1
#define TASK_BLOCKED	2

#define NUM_TASKS 32



typedef uint8_t task_gid_t;

typedef uint8_t task_ticks_t;

typedef void (*task_function_t) (void);

typedef struct
{
	task_gid_t		gid;
	uint8_t        unused;
	uint16_t			pc;
	uint16_t			next;
	uint16_t			x;
	uint16_t			y;
	uint16_t			s;
	uint16_t			u;
	uint8_t			delay;
	uint8_t			asleep;
	uint8_t			state;
	uint8_t			a;
	uint8_t			b;
	uint16_t			arg;
	uint8_t			stack[TASK_STACK_SIZE];
} task_t;

typedef task_t *task_pid_t;



extern U8 task_idle_count;
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

/********************************/
/*     Function Prototypes      */
/********************************/

void task_dump (void);
void task_init (void);
void task_yield (void);
void task_create (void);
task_t *task_create_gid (task_gid_t, task_function_t fn);
task_t *task_create_gid1 (task_gid_t, task_function_t fn);
task_t *task_recreate_gid (task_gid_t, task_function_t fn);
task_t *task_getpid (void);
task_gid_t task_getgid (void);
void task_setgid (task_gid_t gid);
void task_sleep (task_ticks_t ticks);
void task_sleep_sec (int8_t secs);
void task_exit (void) __noreturn__;
task_t *task_find_gid (task_gid_t);
bool task_kill_gid (task_gid_t);
uint16_t task_get_arg (void);
void task_set_arg (task_t *tp, uint16_t arg);

#define task_create_child(fn)		task_create_gid (task_getgid (), fn)

#endif /* _SYS_TASK_H */
