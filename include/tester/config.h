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

#ifndef _MACH_CONFIG_H
#define _MACH_CONFIG_H

/*
 * Game specific configuration
 *
 * This file contains tons of definitions used by the main kernel
 * to implement machine-specific behavior.
 *
 * This file is always included _after_ all of the standard system
 * files, so you can reference types, macros, etc. from there just
 * fine.
 */

/* The tester is unique in that it can be built for any specific
 * hardware configuration.  Exactly one of the following should
 * be defined :
 *
 * WPC_TEST_DMD
 * WPC_TEST_DCS
 * WPC_TEST_SECURITY
 * WPC_TEST_95
 */

#if defined (WPC_TEST_ALPHA)
#define MACHINE_NAME        "WPC-ALPHA TESTER"
#define MACHINE_PINMAME_ZIP "wpcatest.zip"
#define MACHINE_PINMAME_ROM "wpcatest.rom"
#define MACHINE_DMD         0
#define MACHINE_FLIPTRONIC  0
#define MACHINE_DCS         0
#define MACHINE_PIC         0
#define MACHINE_WPC95       0

#elif defined (WPC_TEST_DMD)
#define MACHINE_NAME        "WPC-DMD TESTER"
#define MACHINE_PINMAME_ZIP "wpcdtest.zip"
#define MACHINE_PINMAME_ROM "wpcdtest.rom"
#define MACHINE_DMD         1
#define MACHINE_FLIPTRONIC  0
#define MACHINE_DCS         0
#define MACHINE_PIC         0
#define MACHINE_WPC95       0

#elif defined (WPC_TEST_FLIPTRONIC)
#define MACHINE_NAME        "WPC-F TESTER"
#define MACHINE_PINMAME_ZIP "wpcftest.zip"
#define MACHINE_PINMAME_ROM "wpcftest.rom"
#define MACHINE_DMD         1
#define MACHINE_FLIPTRONIC  1
#define MACHINE_DCS         0
#define MACHINE_PIC         0
#define MACHINE_WPC95       0

#elif defined(WPC_TEST_DCS)
#define MACHINE_NAME        "WPC-DCS TESTER"
#define MACHINE_PINMAME_ZIP "wpctest.zip"
#define MACHINE_PINMAME_ROM "wpctest.rom"
#define MACHINE_DMD         1
#define MACHINE_FLIPTRONIC  1
#define MACHINE_DCS         1
#define MACHINE_PIC         0
#define MACHINE_WPC95       0

#elif defined(WPC_TEST_SECURITY)
#define MACHINE_NAME        "WPC-S TESTER"
#define MACHINE_PINMAME_ZIP "wpcstest.zip"
#define MACHINE_PINMAME_ROM "wpcstest.rom"
#define MACHINE_DMD         1
#define MACHINE_FLIPTRONIC  1
#define MACHINE_DCS         1
#define MACHINE_PIC         1
#define MACHINE_WPC95       0

#elif defined(WPC_TEST_95)
#define MACHINE_NAME        "WPC-95 TESTER"
#define MACHINE_PINMAME_ZIP "wpc5test.zip"
#define MACHINE_PINMAME_ROM "wpc5test.rom"
#define MACHINE_DMD         1
#define MACHINE_FLIPTRONIC  1
#define MACHINE_DCS         1
#define MACHINE_PIC         1
#define MACHINE_WPC95       1

#endif

/* Only the test menu is enabled */
#define MACHINE_TEST_ONLY

/* Assume that upper flippers are present */
#define MACHINE_HAS_UPPER_LEFT_FLIPPER
#define MACHINE_HAS_UPPER_RIGHT_FLIPPER

/* Define the number of digits to use for "standard" score values.
 * Note that for alphanumeric games, setting this to any value
 * higher than 8 will not work because of the way that the score
 * segments are drawn. */
#define MACHINE_SCORE_DIGITS 8

/* Define the locations of standard switches within the switch matrix.
 * These switches _must_ be defined */
// #define MACHINE_SHOOTER_SWITCH ???
#define MACHINE_TILT_SWITCH		MAKE_SW(1,4)
#define MACHINE_SLAM_TILT_SWITCH	MAKE_SW(2,1)
#define MACHINE_START_SWITCH		MAKE_SW(1,3)

#define MACHINE_TROUGH_SIZE 6

#undef MACHINE_START_LAMP
#undef MACHINE_BUYIN_LAMP

/* Define additional test menu items.  This is intended to be used
 * for devices unique to each machine. */
// #define MACHINE_TEST_MENU_ITEMS

/* Declare the array of opto bits, defined elsewhere */
extern const uint8_t mach_opto_mask[];

/* Declare the array of edge switches, defined elsewhere */
extern const uint8_t mach_edge_switches[];

/* ??? not sure how this is being used */
#define MACHINE_SOL_EXTBOARD1

#define MACHINE_LACKS_ALL_INCLUDES

#define MACHINE_NEW_SWITCH_TYPES

#endif /* MACH_CONFIG_H */

