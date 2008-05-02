#ifndef _SYS_TIME_H
#define _SYS_TIME_H

/***************************************************************
 * System timing
 ***************************************************************/

/* A "tick" is defined as the minimum unit of time that is
 * tracked by the kernel.  A tick represents some multiple of
 * the IRQ frequency, since the IRQ happens more often than
 * we need for timing granularity.
 *
 * We define this to be 16 now, which means that every 16 IRQs,
 * we increment our "tick counter" by 1.  The tick then
 * represents about 16.66ms of actual time.
 *
 * The tick count is stored in a single byte field of the
 * task structure, so it can store up to about 4 seconds.
 * If you need to sleep longer than this, use 'task_sleep_sec'
 * instead of 'task_sleep'.
 */
#define IRQS_PER_TICK 16

/* Primitive time constants */
#define TIME_16MS 	1U
#define TIME_33MS 	2U
#define TIME_50MS 	3U
#define TIME_66MS 	(TIME_33MS * 2U)
#define TIME_100MS 	(TIME_33MS * 3U)
#define TIME_133MS 	(TIME_33MS * 4U)
#define TIME_166MS 	(TIME_33MS * 5U)
#define TIME_200MS	(TIME_100MS * 2U)
#define TIME_250MS	(TIME_200MS + TIME_50MS)
#define TIME_300MS	(TIME_100MS * 3U)
#define TIME_400MS	(TIME_100MS * 4U)
#define TIME_500MS	(TIME_100MS * 5U)
#define TIME_1S 		(TIME_100MS * 10U) /* 2 * 3 * 10 = 60 ticks */
#define TIME_2S 		(TIME_1S * 2U)     /* 120 ticks */
#define TIME_3S 		(TIME_1S * 3UL)
#define TIME_4S 		(TIME_1S * 4UL)

/*
 * These time values can only be used for low-level timers,
 * because they use a 16-bit ticks field.
 */
#define TIME_5S 		(TIME_1S * 5UL)
#define TIME_6S 		(TIME_1S * 6UL)
#define TIME_7S 		(TIME_1S * 7UL)
#define TIME_8S 		(TIME_1S * 8UL)
#define TIME_9S 		(TIME_1S * 9UL)
#define TIME_10S 		(TIME_1S * 10UL)
#define TIME_15S 		(TIME_1S * 15UL)
#define TIME_30S 		(TIME_1S * 30UL)


extern inline U16 get_sys_time (void)
{
	extern U16 sys_time;
	return sys_time;
}

#endif /* _SYS_TIME_H */
