
#ifndef _SYS_TASK_H
#define _SYS_TASK_H

#include <env.h>
#include <sys/types.h>

typedef int task_gid_t;

typedef int task_ticks_t;

typedef void *task_t;

typedef void (*task_function_t) (uint16_t);

/********************************/
/*     Function Prototypes      */
/********************************/

void task_yield (void);
void task_create (task_function_t fn, uint16_t arg, task_gid_t gid);
void task_create_gid (task_gid_t, task_function_t fn, uint16_t arg);
void task_create_gid1 (task_gid_t, task_function_t fn, uint16_t arg);
void task_recreate_gid (task_gid_t, task_function_t fn, uint16_t arg);
task_gid_t task_getgid (void);
void task_sleep (task_ticks_t ticks);
void task_exit (void) __noreturn__;
task_t *task_find_gid (task_gid_t);
void task_kill_gid (task_gid_t);

#endif /* _SYS_TASK_H */
