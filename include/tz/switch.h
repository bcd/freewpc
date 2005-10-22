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

#define SW_LEFT_RAMP_ENTER		MAKE_SW(5,3)
#define SW_LEFT_RAMP_EXIT		MAKE_SW(5,4)

#define SW_SLOT					MAKE_SW(5,8)

#define SW_SKILL_BOTTOM			MAKE_SW(6,1)
#define SW_SKILL_CENTER			MAKE_SW(6,2)
#define SW_SKILL_TOP				MAKE_SW(6,3)

#define SW_AUTOFIRE1				MAKE_SW(7,1)
#define SW_AUTOFIRE2				MAKE_SW(7,2)
#define SW_RIGHT_RAMP			MAKE_SW(7,3)

#define SW_LOWER_RIGHT_MAGNET	MAKE_SW(8,1)
#define SW_UPPER_RIGHT_MAGNET	MAKE_SW(8,2)
#define SW_LEFT_MAGNET			MAKE_SW(8,3)
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

#define MACHINE_SW11				sw_right_inlane
#define MACHINE_SW12				sw_right_outlane

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

#define MACHINE_SW31				sw_left_jet
#define MACHINE_SW32				sw_right_jet
#define MACHINE_SW33				sw_bottom_jet

#define MACHINE_SW34				sw_left_sling
#define MACHINE_SW35				sw_right_sling
#define MACHINE_SW36				sw_left_outlane
#define MACHINE_SW37				sw_left_inlane_1
#define MACHINE_SW38				sw_left_inlane_2

#define MACHINE_SW48				sw_greed

#define MACHINE_SW53				sw_left_ramp_enter
#define MACHINE_SW54				sw_left_ramp_exit
#define MACHINE_SW58				sw_slot

#define MACHINE_SW64				sw_greed
#define MACHINE_SW66				sw_greed
#define MACHINE_SW67				sw_greed
#define MACHINE_SW68				sw_greed

#define MACHINE_SW73				sw_right_ramp
#define MACHINE_SW77				sw_greed
#define MACHINE_SW78				sw_greed

#define MACHINE_SW84				sw_lock
#define MACHINE_SW85				sw_lock
#define MACHINE_SW88				sw_lock

#endif /* _MACH_SWITCH_H */
