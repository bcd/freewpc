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
#define SW_LAUNCH_BUTTON		MAKE_SW(1,1)
#define SW_START_BUTTON			MAKE_SW(1,3)
#define SW_TILT					MAKE_SW(1,4)
#define SW_SHOOTER				MAKE_SW(1,8)

#define SW_SLAM_TILT				MAKE_SW(2,1)
#define SW_COIN_DOOR_CLOSED	MAKE_SW(2,2)

#define SW_TROUGH_1				MAKE_SW(3,1)
#define SW_TROUGH_2				MAKE_SW(3,2)
#define SW_TROUGH_3				MAKE_SW(3,3)
#define SW_TROUGH_4				MAKE_SW(3,4)
#define SW_TROUGH_5				MAKE_SW(3,5)
#define SW_LEFT_POPPER			MAKE_SW(3,6)
#define SW_RIGHT_POPPER			MAKE_SW(3,7)

/*
 * Declare switch drivers.  Each macro defines the name of a driver object
 * which contains all of the properties of that switch.
 */
#define MACHINE_SW11			sw_launch_button
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
