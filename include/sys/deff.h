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
} deff_t;

#define MAX_QUEUED_DEFFS 16

#define DEFF_NULL				0
#define DEFF_COIN_INSERT	1
#define DEFF_TEST_MENU		2
#define DEFF_PRINT_RTC		3
#define DEFF_CREDITS			4
#define DEFF_SCORES			5
#define DEFF_FONT_TEST		6
#define DEFF_AMODE			7
#define DEFF_VOLUME_CHANGE	8
#define DEFF_TILT				9
#define DEFF_TILT_WARNING	10
#define DEFF_SLAM_TILT		11

uint8_t deff_get_active (void);
void deff_start (deffnum_t dn);
void deff_stop (deffnum_t dn);
void deff_restart (deffnum_t dn);
void deff_start_highest_priority (void);
void deff_exit (void) __noreturn__;
void deff_delay_and_exit (task_ticks_t ticks);
void deff_swap_low_high (int8_t count, task_ticks_t delay);
void deff_init (void);

#endif /* _SYS_DEFF_H */

