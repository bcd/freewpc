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

#ifndef _DEFAULTS_H
#define _DEFAULTS_H

/*
 * This file documents the machine-specific options that belong
 * in mach/config.h, and assigns proper defaults to all of them
 * in case the machine does not define them.
 * 
 * In some cases, there is no default, and failure to define
 * something will generate a compiler error.
 *
 * This file is included _after_ the machine-specific version.
 *
 * When creating a new machine, this file can be copied as a
 * reference for what things need to be defined.
 */


/** The name of the machine.  Be descriptive. */
#ifndef MACHINE_NAME
#define MACHINE_NAME "DEFAULT MACHINE"
#endif

/** The names of the ROM files which PinMAME will work for. */
#ifndef MACHINE_PINMAME_ZIP
#define MACHINE_PINMAME_ZIP "freewpc1.zip"
#endif
#ifndef MACHINE_PINMAME_ROM
#define MACHINE_PINMAME_ROM "freewpc1.rom"
#endif

/* Assume DMD by default, since FreeWPC doesn't support alphanumeric anymore. */
#ifndef MACHINE_DMD
#define MACHINE_DMD 1
#endif

/* The next three are required to denote the right generation of WPC */

/* Define as 1 if the machine has DCS sound, 0 if older WPC style */
#ifndef MACHINE_DCS
#error "MACHINE_DCS not defined"
#endif

/* Define as 1 if the game uses Fliptronic flippers */
#ifndef MACHINE_FLIPTRONIC
#error "MACHINE_FLIPTRONIC not defined"
#endif

/* Define as 1 if the machine uses the WPC95 hardware */
#ifndef MACHINE_WPC95
#error "MACHINE_WPC95 not defined"
#endif

/* Define if any upper flippers exist.  These are optional. */
// #define MACHINE_HAS_UPPER_LEFT_FLIPPER
// #define MACHINE_HAS_UPPER_RIGHT_FLIPPER

/* Define the number of digits to use for "standard" score values.
 * Note that for alphanumeric games, setting this to any value
 * higher than 8 will not work because of the way that the score
 * segments are drawn. */
#ifndef MACHINE_SCORE_DIGITS
#define MACHINE_SCORE_DIGITS 8
#endif
#if (MACHINE_SCORE_DIGITS > 8) && (MACHINE_DMD == 0)
#warning "Alphanumeric games cannot support more than 8 score digits"
#endif

/* Define the locations of standard switches within the switch matrix.
 * These switches _must_ be defined.  Use values from mach/switch.h.
 * The default values will look for standard names. */
#ifndef MACHINE_SHOOTER_SWITCH
#define MACHINE_SHOOTER_SWITCH	SW_SHOOTER
#endif
#ifndef MACHINE_TILT_SWITCH
#define MACHINE_TILT_SWITCH		SW_TILT
#endif
#ifndef MACHINE_SLAM_TILT_SWITCH
#define MACHINE_SLAM_TILT_SWITCH	SW_SLAM_TILT
#endif
#ifndef MACHINE_START_SWITCH
#define MACHINE_START_SWITCH		SW_START_BUTTON
#endif

/* Define the locations of optional switches.  These may not be
 * defined for all games. */
// #define MACHINE_BUYIN_SWITCH ...
// #define MACHINE_LAUNCH_SWITCH ...
// #define MACHINE_OUTHOLE_SWITCH ...

/* Define the number and location of the trough switches.
 * The order should proceed from the one nearest the outhole
 * to the plunger.  You can use the name of the switch as
 * defined in mach/switch.h */
#ifndef MACHINE_TROUGH_SIZE
#error "Trough size not defined"
#endif
#if (MACHINE_TROUGH_SIZE >= 1) && !defined(MACHINE_TROUGH1)
#error "Trough 1 switch not defined"
#endif
#if (MACHINE_TROUGH_SIZE >= 2) && !defined(MACHINE_TROUGH2)
#error "Trough 2 switch not defined"
#endif
#if (MACHINE_TROUGH_SIZE >= 3) && !defined(MACHINE_TROUGH3)
#error "Trough 3 switch not defined"
#endif

/* Define the standard solenoids */
#ifndef MACHINE_BALL_SERVE_SOLENOID
#define MACHINE_BALL_SERVE_SOLENOID	SOL_BALL_SERVE
#endif
#ifndef MACHINE_KNOCKER_SOLENOID
#define MACHINE_KNOCKER_SOLENOID	SOL_KNOCKER
#endif

/* Define optional solenoids */
// #define MACHINE_LAUNCH_SOLENOID

/* Define the standard lamps */
#ifndef MACHINE_START_LAMP
#define MACHINE_START_LAMP	LM_START_BUTTON
#endif

/* Define optional lamps */
// #define MACHINE_BUYIN_LAMP	...

/* Define sounds/music for standard system functions */
// #define MACHINE_ADD_COIN_SOUND ...
// #define MACHINE_ADD_CREDIT_SOUND ...
// #define MACHINE_VOLUME_CHANGE_MUSIC ...
// #define MACHINE_START_GAME_SOUND ...
// #define MACHINE_START_BALL_MUSIC ...
// #define MACHINE_BALL_IN_PLAY_MUSIC ...
// #define MACHINE_END_GAME_MUSIC ...
// #define MACHINE_TILT_WARNING_SOUND ...
// #define MACHINE_TILT_SOUND ...

/* Define the names of functions to handle flipper buttons
 * in attract mode.  Leave undefined if not needed.
 * The functions should not take any arguments. */
#define MACHINE_AMODE_LEFT_FLIPPER_HANDLER amode_left_flipper
#define MACHINE_AMODE_RIGHT_FLIPPER_HANDLER amode_right_flipper

/* Define if the game provides its own attract mode functions.
 * If so, they must be named amode_start() and amode_stop().
 * If undefined, the system will use a default function.
 */
#define MACHINE_CUSTOM_AMODE

/* Define additional test menu items.  This is intended to be used
 * for devices unique to each machine.
 * This should be a comma separated list of pointers to test items.
 * There should be a comma after the last item, too. */
// #define MACHINE_TEST_MENU_ITEMS ...
	&tz_clock_test_item, \
	&tz_gumball_test_item,

/* Define additional feature adjustments.  You must define both a struct,
 * named feature_adj_t, that contains the storage for the adjustments,
 * as well as test mode descriptions to be included, named
 * MACHINE_FEATURE_ADJUSTMENTS. */
typedef struct
{
	U8 ball_saves;
	U8 ball_save_time;
	U8 installed_balls;
	U8 disable_clock;
	U8 disable_gumball;
	U8 powerball_missing;
	U8 third_magnet;
} feature_adj_t;

#define MACHINE_FEATURE_ADJUSTMENTS \
	{ "BALL SAVES", &integer_value, 1, &feature_config.ball_saves }, \
	{ "BALL SAVE TIME", &integer_value, 7, &feature_config.ball_save_time }, \
	{ "INSTALLED BALLS", &integer_value, 6, &feature_config.installed_balls }, \
	{ "DISABLE CLOCK", &yes_no_value, NO, &feature_config.disable_clock }, \
	{ "DISABLE GUMBALL", &yes_no_value, NO, &feature_config.disable_gumball }, \
	{ "POWERBALL MISSING", &yes_no_value, NO, &feature_config.powerball_missing }, \
	{ "HAVE 3RD MAGNET", &yes_no_value, NO, &feature_config.third_magnet },

/* Declare the array of opto bits, defined elsewhere */
extern const uint8_t mach_opto_mask[];

/* Declare the array of edge switches, defined elsewhere */
extern const uint8_t mach_edge_switches[];

/* Declare the display effects that this machine implements.
 * This should be a comma separate list of DECL_DEFF() declarations, with no
 * trailing comma.  Each DECL_DEFF looks like this:
 *
 * DECL_DEFF(name, flags, priority, function)
 *
 * Names should always begin with DEFF_ for proper gendefine detection.
 * Flags are either D_NORMAL or D_RUNNING.
 * See include/priority.h for a list of priority values.
 */
// #define MACHINE_DISPLAY_EFFECTS ...

/* Declare the lamp effects that this machine implements.
 * The format is similar to the above.  Use a series of DECL_LEFF() declarations:
 *
 * DECL_LEFF(name, flags, priority, lamps, gi, function)
 *
 * Names should always begin with LEFF_.
 * Flags should be either L_NORMAL, L_RUNNING, or L_SHARED.
 * Lamps should be a LAMPSET_ value (see below).
 * GI should be a GI bitmap.
 */
// #define MACHINE_LAMP_EFFECTS ...

/* Declares the lampsets that should be instantiated */
#define MACHINE_LAMPSETS \
	DECL_LAMPSET(LAMPSET_DOOR_PANELS, LMSET_DOOR_PANEL_AWARDS) \
	DECL_LAMPSET(LAMPSET_DOOR_PANELS_AND_HANDLE, LMSET_DOOR_PANELS_AND_HANDLE) \
	DECL_LAMPSET(LAMPSET_DOOR_LOCKS, LMSET_DOOR_LOCKS) \
	DECL_LAMPSET(LAMPSET_DOOR_GUMBALL, LMSET_DOOR_GUMBALL) \
	DECL_LAMPSET(LAMPSET_LEFT_RAMP_AWARDS, LMSET_LEFT_RAMP_AWARDS) \
	DECL_LAMPSET(LAMPSET_LOCK_AWARDS, LMSET_LOCK_AWARDS) \
	DECL_LAMPSET(LAMPSET_PIANO_AWARDS, LMSET_PIANO_AWARDS) \
	DECL_LAMPSET(LAMPSET_GREED_TARGETS, LMSET_GREED_TARGETS) \
	DECL_LAMPSET(LAMPSET_POWERFIELD_VALUES, LMSET_POWERFIELD_VALUES) \
	DECL_LAMPSET(LAMPSET_JETS, LMSET_JETS) \
	DECL_LAMPSET(LAMPSET_LOWER_LANES, LMSET_LOWER_LANES) \
	DECL_LAMPSET(LAMPSET_SPIRAL_AWARDS, LMSET_SPIRAL_AWARDS) \
	DECL_LAMPSET(LAMPSET_AMODE_ALL, LMSET_AMODE_ALL) \
	DECL_LAMPSET(LAMPSET_AMODE_RAND, LMSET_AMODE_RAND)

/* Declares which coil drives correspond to flashlamps.
 * This should be a boolean expression that returns 1 if the solenoid
 * value given is a flasher. */
#ifndef MACHINE_SOL_FLASHERP
#warning "MACHINE_SOL_FLASHERP() not defined"
#endif

/* Declares that EXTBOARD1 is used to drive some extra solenoids */
// #define MACHINE_SOL_EXTBOARD1

/* Declares which coil drives should not be fired during ball
 * search, for any reason.  Ball device solenoids are automatically
 * excluded and need not be specified here.  Only solenoids 1-32
 * will be checked; others are automatically skipped. */
// #define MACHINE_SOL_NOSEARCHP(sol) ...

/* Declare realtime functions in the machine layer.
 * These functions MUST be linked into the system page of the ROM,
 * and must be declared like this: void xyz (void) .
 * Use the VOIDCALL macro around the name of each function to
 * generate the proper calling sequence.
 *
 * Use the proper define based on how often the function needs to be
 * called.  You can safely leave any unneeded ones #undef'd.
 *
 * Realtime solenoids should generally use the 8ms rtt.  Processing
 * intensive stuff should be done less frequently, in 32ms or 128ms.
 * Only the fastest code belongs in 1ms (it runs every IRQ!)
 */
// #define MACHINE_1MS_RTTS...
// #define MACHINE_8MS_RTTS...
// #define MACHINE_32MS_RTTS...
// #define MACHINE_128MS_RTTS...

/* ??? */
// #define MACHINE_INCLUDE_FLAGS

/* Externs for any functions system needs to see */
void piano_ball_start (void);
void slot_ball_start (void);
void autofire_launch_proc (void);
void autofire_to_gumball_proc (void);
void release_gumball_proc (void);

extern struct menu tz_clock_test_item;
extern struct menu tz_gumball_test_item;

/* Timed game parameters */
// #define CONFIG_TIMED_GAME 60
// #define CONFIG_TIMED_GAME_MAX 120
// #define CONFIG_TIMED_GAME_OVER_SOUND SND_CLOCK_GONG
// #define CONFIG_TIMED_GAME_OVER_LEFF LEFF_GAME_TIMEOUT

/* Default high scores */
// #define MACHINE_GRAND_CHAMPION_INITIALS ...
// #define MACHINE_GRAND_CHAMPION_SCORE ...
// #define MACHINE_HIGH_SCORE_INITIALS ...
// #define MACHINE_HIGH_SCORES ...

#endif /* MACH_CONFIG_H */

