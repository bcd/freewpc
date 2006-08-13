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
#define SW_RIGHT_INLANE			MAKE_SW(1,1)
#define SW_RIGHT_OUTLANE		MAKE_SW(1,2)
#define SW_START_BUTTON			MAKE_SW(1,3)
#define SW_TILT					MAKE_SW(1,4)
#define SW_RIGHT_TROUGH			MAKE_SW(1,5)
#define SW_CENTER_TROUGH		MAKE_SW(1,6)
#define SW_LEFT_TROUGH			MAKE_SW(1,7)
#define SW_OUTHOLE				MAKE_SW(1,8)

#define SW_SLAM_TILT				MAKE_SW(2,1)
#define SW_COIN_DOOR_CLOSED	MAKE_SW(2,2)
#define SW_BUYIN_BUTTON			MAKE_SW(2,3)
/*** switch 24 reserved for system ***/
#define SW_FAR_LEFT_TROUGH		MAKE_SW(2,5)
#define SW_TROUGH_PROX			MAKE_SW(2,6)
#define SW_SHOOTER				MAKE_SW(2,7)
#define SW_ROCKET					MAKE_SW(2,8)

#define SW_LEFT_JET				MAKE_SW(3,1)
#define SW_RIGHT_JET				MAKE_SW(3,2)
#define SW_BOTTOM_JET			MAKE_SW(3,3)
#define SW_LEFT_SLING			MAKE_SW(3,4)
#define SW_RIGHT_SLING			MAKE_SW(3,5)
#define SW_LEFT_OUTLANE			MAKE_SW(3,6)
#define SW_LEFT_INLANE_1		MAKE_SW(3,7)
#define SW_LEFT_INLANE_2		MAKE_SW(3,8)

#define SW_DEAD_END				MAKE_SW(4,1)
#define SW_CAMERA					MAKE_SW(4,2)
#define SW_PIANO					MAKE_SW(4,3)
#define SW_MPF_ENTER				MAKE_SW(4,4)
#define SW_MPF_LEFT				MAKE_SW(4,5)
#define SW_MPF_RIGHT				MAKE_SW(4,6)
#define SW_CLOCK_TARGET			MAKE_SW(4,7)
#define SW_STANDUP1				MAKE_SW(4,8)

#define SW_GUMBALL_LANE			MAKE_SW(5,1)
#define SW_HITCHHIKER			MAKE_SW(5,2)
#define SW_LEFT_RAMP_ENTER		MAKE_SW(5,3)
#define SW_LEFT_RAMP_EXIT		MAKE_SW(5,4)
#define SW_GUMBALL_GENEVA		MAKE_SW(5,5)
#define SW_GUMBALL_EXIT			MAKE_SW(5,6)
#define SW_SLOT_PROXIMITY		MAKE_SW(5,7)
#define SW_SLOT					MAKE_SW(5,8)

#define SW_SKILL_BOTTOM			MAKE_SW(6,1)
#define SW_SKILL_CENTER			MAKE_SW(6,2)
#define SW_SKILL_TOP				MAKE_SW(6,3)
#define SW_STANDUP4				MAKE_SW(6,4)
#define SW_POWER_PAYOFF			MAKE_SW(6,5)
#define SW_STANDUP5				MAKE_SW(6,6)
#define SW_STANDUP6				MAKE_SW(6,7)
#define SW_STANDUP7				MAKE_SW(6,8)

#define SW_AUTOFIRE1				MAKE_SW(7,1)
#define SW_AUTOFIRE2				MAKE_SW(7,2)
#define SW_RIGHT_RAMP			MAKE_SW(7,3)
#define SW_GUMBALL_POPPER		MAKE_SW(7,4)
#define SW_MPF_TOP				MAKE_SW(7,5)
#define SW_MPF_EXIT				MAKE_SW(7,6)
#define SW_STANDUP2				MAKE_SW(7,7)
#define SW_STANDUP3				MAKE_SW(7,8)

#define SW_LOWER_RIGHT_MAGNET	MAKE_SW(8,1)
#define SW_UPPER_RIGHT_MAGNET	MAKE_SW(8,2)
#define SW_LEFT_MAGNET			MAKE_SW(8,3)
#define SW_LOCK_CENTER			MAKE_SW(8,4)
#define SW_LOCK_UPPER			MAKE_SW(8,5)
#define SW_CLOCK_PASSAGE		MAKE_SW(8,6)
#define SW_GUMBALL_ENTER		MAKE_SW(8,7)
#define SW_LOCK_LOWER			MAKE_SW(8,8)

/*
 * Declare switch drivers.  Each macro defines the name of a driver object
 * which contains all of the properties of that switch.
 */
//#define MACHINE_SW01
//#define MACHINE_SW02
//#define MACHINE_SW03
//#define MACHINE_SW04
//#define MACHINE_SW05
//#define MACHINE_SW06
//#define MACHINE_SW07
//#define MACHINE_SW08
//
//#define MACHINE_SW11
//#define MACHINE_SW12
//
//#define MACHINE_SW13
//#define MACHINE_SW14
//#define MACHINE_SW15
//#define MACHINE_SW16
//#define MACHINE_SW17
//#define MACHINE_SW18
//
//#define MACHINE_SW21
////#define MACHINE_SW22
//#define MACHINE_SW23
//#define MACHINE_SW28
//
//#define MACHINE_SW31
//#define MACHINE_SW32
//#define MACHINE_SW33
//
//#define MACHINE_SW34
//#define MACHINE_SW35
//#define MACHINE_SW36
//#define MACHINE_SW37
//#define MACHINE_SW38
//
//#define MACHINE_SW41
//#define MACHINE_SW42
//#define MACHINE_SW43
//#define MACHINE_SW48
//
//#define MACHINE_SW51
//#define MACHINE_SW53
//#define MACHINE_SW54
//#define MACHINE_SW55
//#define MACHINE_SW56
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
//#define MACHINE_SW73
//#define MACHINE_SW74
//#define MACHINE_SW77
//#define MACHINE_SW78
//
//#define MACHINE_SW81
//#define MACHINE_SW83
//#define MACHINE_SW84
//#define MACHINE_SW85
//#define MACHINE_SW87
//#define MACHINE_SW88

#endif /* _MACH_SWITCH_H */
