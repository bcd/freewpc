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
#define SW_LEFT_INLANE			MAKE_SW(1,2)
#define SW_START_BUTTON			MAKE_SW(1,3)
#define SW_TILT					MAKE_SW(1,4)
#define SW_RIGHT_TROUGH			MAKE_SW(1,5)
#define SW_CENTER_TROUGH		MAKE_SW(1,6)
#define SW_LEFT_TROUGH			MAKE_SW(1,7)
#define SW_OUTHOLE				MAKE_SW(1,8)

#define SW_SLAM_TILT				MAKE_SW(2,1)
#define SW_COIN_DOOR_CLOSED	MAKE_SW(2,2)
#define SW_BUYIN_BUTTON			MAKE_SW(2,3)
#define SW_FAR_LEFT_TROUGH		MAKE_SW(2,5)
#define SW_TROUGH_PROX			MAKE_SW(2,6)
#define SW_SHOOTER				MAKE_SW(2,7)
#define SW_ROCKET					MAKE_SW(2,8)

#define SW_SLOT					MAKE_SW(5,8)

#define SW_AUTOFIRE1				MAKE_SW(7,1)
#define SW_AUTOFIRE2				MAKE_SW(7,2)

#define SW_LOCK_CENTER			MAKE_SW(8,4)
#define SW_LOCK_UPPER			MAKE_SW(8,5)
#define SW_LOCK_LOWER			MAKE_SW(8,8)

/*
 * Declare switch drivers.  Each macro defines the name of a driver object
 * which contains all of the properties of that switch.
 */
#define MACHINE_SW01				sw_left_coin
#define MACHINE_SW02				sw_center_coin
#define MACHINE_SW03				sw_right_coin
#define MACHINE_SW04				sw_fourth_coin
#define MACHINE_SW05				sw_escape_button
#define MACHINE_SW06				sw_down_button
#define MACHINE_SW07				sw_up_button
#define MACHINE_SW08				sw_enter_button

#define MACHINE_SW13				sw_start_button
#define MACHINE_SW14				sw_tilt
#define MACHINE_SW15				sw_trough
#define MACHINE_SW16				sw_trough
#define MACHINE_SW17				sw_trough
#define MACHINE_SW18				sw_outhole

#define MACHINE_SW21				sw_slam_tilt
//#define MACHINE_SW22				sw_coin_door_closed
#define MACHINE_SW23				sw_buyin_button
#define MACHINE_SW28				sw_rocket

#define MACHINE_SW58				sw_slot

#define MACHINE_SW84				sw_lock
#define MACHINE_SW85				sw_lock
#define MACHINE_SW88				sw_lock

#endif /* _MACH_SWITCH_H */
