/**************************************************************************
 *
 * FreeWPC - an open source operating system for the Williams Pinball
 *           Controller (WPC) platform
 *
 **************************************************************************/

#ifndef _MACH_SWITCH_H
#define _MACH_SWITCH_H

/*
 * Declare switch numbering.  This provides named constants for the
 * various switch locations used by this machine.
 */
#define SW_MAGNA_GOALIE_BUTTON	MAKE_SW(1,2)
#define SW_START_BUTTON			MAKE_SW(1,3)
#define SW_TILT					MAKE_SW(1,4)
#define SW_LEFT_INLANE			MAKE_SW(1,5)
#define SW_STRIKER_HIGH			MAKE_SW(1,6)
#define SW_RIGHT_INLANE			MAKE_SW(1,7)
#define SW_RIGHT_OUTLANE		MAKE_SW(1,8)

#define SW_SLAM_TILT				MAKE_SW(2,1)
#define SW_COIN_DOOR_CLOSED	MAKE_SW(2,2)
#define SW_BUYIN_BUTTON			MAKE_SW(2,3)
/*** switch 24 reserved for system ***/
#define SW_FREE_KICK				MAKE_SW(2,5)
#define SW_KICKBACK_UPPER		MAKE_SW(2,6)
#define SW_SPINNER				MAKE_SW(2,7)
#define SW_LIGHT_KICKBACK		MAKE_SW(2,8)

#define SW_TROUGH_1				MAKE_SW(3,1)
#define SW_TROUGH_2				MAKE_SW(3,2)
#define SW_TROUGH_3				MAKE_SW(3,3)
#define SW_TROUGH_4				MAKE_SW(3,4)
#define SW_TROUGH_5				MAKE_SW(3,5)
#define SW_TROUGH_STACK			MAKE_SW(3,6)
#define SW_LIGHT_MAGNA_GOALIE	MAKE_SW(3,7)
#define SW_SHOOTER				MAKE_SW(3,8)

#define SW_GOAL_TROUGH			MAKE_SW(4,1)
#define SW_GOAL_POPPER			MAKE_SW(4,2)
#define SW_GOALIE_LEFT			MAKE_SW(4,3)
#define SW_GOALIE_RIGHT			MAKE_SW(4,4)
#define SW_TV_POPPER				MAKE_SW(4,5)
#define SW_TRAVEL_LANE			MAKE_SW(4,7)
#define SW_GOALIE_TARGET		MAKE_SW(4,8)

#define SW_SKILL_FRONT			MAKE_SW(5,1)
#define SW_SKILL_CENTER			MAKE_SW(5,2)
#define SW_SKILL_REAR			MAKE_SW(5,3)
#define SW_RIGHT_EJECT			MAKE_SW(5,4)
#define SW_UPPER_EJECT			MAKE_SW(5,5)
#define SW_LEFT_EJECT			MAKE_SW(5,6)
#define SW_FAR_RIGHT_HIGH		MAKE_SW(5,7)
#define SW_FAR_RIGHT_LOW		MAKE_SW(5,8)

#define SW_ROLLOVER_1			MAKE_SW(6,1)
#define SW_ROLLOVER_2			MAKE_SW(6,2)
#define SW_ROLLOVER_3			MAKE_SW(6,3)
#define SW_ROLLOVER_4			MAKE_SW(6,4)
#define SW_TACKLE					MAKE_SW(6,5)
#define SW_STRIKER_LEFT			MAKE_SW(6,6)
#define SW_STRIKER_CENTER		MAKE_SW(6,7)

#define SW_LEFT_RAMP_DIVERTED	MAKE_SW(7,1)
#define SW_LEFT_RAMP_ENTER		MAKE_SW(7,2)
#define SW_LEFT_RAMP_EXIT		MAKE_SW(7,4)
#define SW_RIGHT_RAMP_ENTER	MAKE_SW(7,5)
#define SW_LOCK_LOW				MAKE_SW(7,6)
#define SW_LOCK_HIGH				MAKE_SW(7,7)
#define SW_RIGHT_RAMP_EXIT		MAKE_SW(7,8)

#define SW_LEFT_JET				MAKE_SW(8,1)
#define SW_UPPER_JET				MAKE_SW(8,2)
#define SW_LOWER_JET				MAKE_SW(8,3)
#define SW_LEFT_SLING			MAKE_SW(8,4)
#define SW_RIGHT_SLING			MAKE_SW(8,5)
#define SW_KICKBACK				MAKE_SW(8,6)
#define SW_UPPER_LEFT_LANE		MAKE_SW(8,7)
#define SW_UPPER_RIGHT_LANE	MAKE_SW(8,8)

/*
 * Declare switch drivers.  Each macro defines the name of a driver object
 * which contains all of the properties of that switch.
 */
//#define MACHINE_SW11
//#define MACHINE_SW12
#define MACHINE_SW13			sw_start_button
//#define MACHINE_SW14
//#define MACHINE_SW15
//#define MACHINE_SW16
//#define MACHINE_SW17
//#define MACHINE_SW18
//
#define MACHINE_SW21			sw_slam_tilt
//#define MACHINE_SW23
//#define MACHINE_SW28
//
#define MACHINE_SW31			sw_trough
#define MACHINE_SW32			sw_trough
#define MACHINE_SW33			sw_trough
#define MACHINE_SW34			sw_trough
#define MACHINE_SW35			sw_trough
//#define MACHINE_SW36
//#define MACHINE_SW37
//#define MACHINE_SW38			sw_shooter
//
//#define MACHINE_SW41
//#define MACHINE_SW42
//#define MACHINE_SW43
//#define MACHINE_SW44
//#define MACHINE_SW45
//#define MACHINE_SW46
//#define MACHINE_SW47
//#define MACHINE_SW48
//
//#define MACHINE_SW51
//#define MACHINE_SW52
//#define MACHINE_SW53
//#define MACHINE_SW54
//#define MACHINE_SW55
//#define MACHINE_SW56
//#define MACHINE_SW57
//#define MACHINE_SW58
//
//#define MACHINE_SW61
//#define MACHINE_SW62
//#define MACHINE_SW63
//#define MACHINE_SW64
//#define MACHINE_SW66
//#define MACHINE_SW67
//#define MACHINE_SW68
//
//#define MACHINE_SW71
//#define MACHINE_SW72
//#define MACHINE_SW73
//#define MACHINE_SW74
//#define MACHINE_SW75
//#define MACHINE_SW76
//#define MACHINE_SW77
//#define MACHINE_SW78
//
//#define MACHINE_SW81
//#define MACHINE_SW82
//#define MACHINE_SW83
//#define MACHINE_SW84
//#define MACHINE_SW85
//#define MACHINE_SW86
//#define MACHINE_SW87
//#define MACHINE_SW88

#endif /* _MACH_SWITCH_H */
