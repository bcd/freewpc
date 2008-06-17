
#ifndef _LOG_H
#define _LOG_H

struct log_event
{
	/* The number of 100ms ticks since the last event was logged.
	This can be as large as 250, for 25 seconds.  A value of 0xFF
	means that more than 25 seconds elapsed. */
	U8 timestamp;

	/* The module ID (upper 4-bits) and event ID (lower 4-bits) */
	U16 module_event;

	/* An event-specific 8-bit argument */
	U8 arg;
};

#define MAX_LOG_EVENTS 128

extern void log_event1(U16 module_event, U8 arg);

#define log_event(severity, module, event, arg) \
	log_event1((((module) << 8UL) | event), arg)

#define log_set_min_severity(severity)

#define log_set_wrapping(wrap_flag)

#define SEV_ERROR 0
#define SEV_WARN 1
#define SEV_INFO 2
#define SEV_DEBUG 3

#define MIN_SEVERITY SEV_DEBUG


#define MOD_DEFF 0 /* done */
	#define EV_DEFF_START 0
	#define EV_DEFF_STOP 1
	#define EV_DEFF_RESTART 2
	#define EV_DEFF_EXIT 3
	#define EV_DEFF_TIMEOUT 4

#define MOD_LAMP 1
	#define EV_LEFF_START 0 /* done */
	#define EV_LEFF_STOP 1 /* done */
	#define EV_LEFF_EXIT 3 /* done */
	#define EV_LAMP_ON 4
	#define EV_LAMP_OFF 5
	#define EV_LAMP_FLASH 6
	#define EV_BIT_ON 7 /* done */
	#define EV_BIT_OFF 8 /* done */
	#define EV_BIT_TOGGLE 9 /* done */
	#define EV_GLOBAL_BIT_ON 10
	#define EV_GLOBAL_BIT_OFF 11

#define MOD_SOUND 2
	#define EV_SOUND_START_MUSIC 0
	#define EV_SOUND_STOP_MUSIC 1
	#define EV_SOUND_RESULT 2

#define MOD_TASK 3
	#define EV_TASK_START 0 /* done */
	#define EV_TASK_KILL 1 /* done */
	#define EV_TASK_RESTART 2
	#define EV_TASK_START1 3
	#define EV_TASK_EXIT 4 /* done */
	#define EV_TASK_100MS 5

#define MOD_SWITCH 4
	#define EV_SW_SCHEDULE 0 /* done */
	#define EV_SW_BLIP 1
	#define EV_SW_SHORT 2

#define MOD_TRIAC 5 /* done */
	#define EV_TRIAC_ON 0
	#define EV_TRIAC_OFF 1

#define MOD_GAME 6
	#define EV_GAME_START 0
	#define EV_PLAYER_ADD 1
	#define EV_BALL_START 2
	#define EV_BALL_VALID 3
	#define EV_BALL_END 4
	#define EV_PLAYER_END 5
	#define EV_GAME_END 6
	#define EV_TILT 7
	#define EV_SLAM_TILT 8

#define MOD_SYSTEM 7
	#define EV_SYSTEM_INIT 0 /* done */
	#define EV_SYSTEM_NONFATAL 1 /* done */
	#define EV_SYSTEM_FATAL 2 /* done */

#define MOD_PRICING 8
	#define EV_PRICING_ADD_CREDIT 0

#define MOD_SOL 9
	#define EV_SOL_START 0 /* done */
	#define EV_SOL_STOP 1 /* done */
	#define EV_DEV_ENTER 2
	#define EV_DEV_KICK 3
	#define EV_DEV_LOCK 4
	#define EV_DEV_UNLOCK 5

#endif /* _LOG_H */
