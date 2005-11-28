#ifndef _SYS_DEFF_H
#define _SYS_DEFF_H

typedef uint8_t deffnum_t;

typedef void (*deff_function_t) (void) __taskentry__;

#define D_NORMAL	0x0

/** A running deff is long-lived and continues to be active
 * until it is explicitly stopped. */
#define D_RUNNING 0x1

typedef struct
{
	uint8_t flags;
	uint8_t prio;
	deff_function_t fn;
	U8 pad;
} deff_t;

#define MAX_QUEUED_DEFFS 16

uint8_t deff_get_active (void);
void deff_start (deffnum_t dn);
void deff_stop (deffnum_t dn);
void deff_restart (deffnum_t dn);
void deff_start_highest_priority (void);
__noreturn__ void deff_exit (void);
void deff_delay_and_exit (task_ticks_t ticks);
void deff_swap_low_high (int8_t count, task_ticks_t delay);
void deff_init (void);
void deff_stop_all (void);

#endif /* _SYS_DEFF_H */

