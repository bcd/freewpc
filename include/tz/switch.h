/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

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
#define MACHINE_SW11				sw_right_inlane
#define MACHINE_SW12				sw_right_outlane
#define MACHINE_SW13				sw_start_button
#define MACHINE_SW14				sw_tilt
#define MACHINE_SW15				sw_trough
#define MACHINE_SW16				sw_trough
#define MACHINE_SW17				sw_trough
#define MACHINE_SW18				sw_outhole

#define MACHINE_SW21				sw_slam_tilt
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

#define MACHINE_SW41				sw_deadend
#define MACHINE_SW42				sw_camera
#define MACHINE_SW43				sw_piano
#define MACHINE_SW47          sw_clock_target
#define MACHINE_SW48				sw_greed

#define MACHINE_SW51				sw_gumball_lane
#define MACHINE_SW52          sw_hitch
#define MACHINE_SW53				sw_left_ramp_enter
#define MACHINE_SW54				sw_left_ramp_exit
#define MACHINE_SW55				sw_gumball_geneva
#define MACHINE_SW56				sw_gumball_exit
#define MACHINE_SW58				sw_slot

#define MACHINE_SW61				sw_lower_skill
#define MACHINE_SW62				sw_center_skill
#define MACHINE_SW63				sw_upper_skill
#define MACHINE_SW64				sw_greed
#define MACHINE_SW66				sw_greed
#define MACHINE_SW67				sw_greed
#define MACHINE_SW68				sw_greed

#define MACHINE_SW71				sw_autofire1
#define MACHINE_SW72				sw_autofire2
#define MACHINE_SW73				sw_right_ramp
#define MACHINE_SW74				sw_gumball_popper
#define MACHINE_SW77				sw_greed
#define MACHINE_SW78				sw_greed

#define MACHINE_SW81				sw_right_loop
#define MACHINE_SW83				sw_left_loop
#define MACHINE_SW84				sw_lock
#define MACHINE_SW85				sw_lock
#define MACHINE_SW87				sw_gumball_enter
#define MACHINE_SW88				sw_lock

#endif /* _MACH_SWITCH_H */
