
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

#define MACHINE_NAME "TWILIGHT ZONE"

#define MACHINE_MAJOR_VERSION 0
#define MACHINE_MINOR_VERSION 1

#define MACHINE_DATE "07/03/2005"


#define MACHINE_PINMAME_ZIP "tz_92.zip"
#define MACHINE_PINMAME_ROM "tzone9_2.rom"


/* Define as 1 if the machine has a DMD, 0 if it is alphanumeric */
#define MACHINE_DMD 1

/* Define as 1 if the machine has DCS sound, 0 if older WPC style */
#define MACHINE_DCS 0

/* Define as 1 if the game uses Fliptronic flippers */
#define MACHINE_FLIPTRONIC 1

/* Define as 1 if the machine uses the WPC95 hardware */
#define MACHINE_WPC95 0

/* Define the number of digits to use for "standard" score values.
 * Note that for alphanumeric games, setting this to any value
 * higher than 8 will not work because of the way that the score
 * segments are drawn. */
#define MACHINE_SCORE_DIGITS 8

/* Define the locations of standard switches within the switch matrix.
 * These switches _must_ be defined */
#define MACHINE_SHOOTER_SWITCH
#define MACHINE_TILT_SWITCH
#define MACHINE_SLAM_TILT_SWITCH
#define MACHINE_START_SWITCH

/* Define the locations of optional switches.  These may not be
 * defined for all games. */
#define MACHINE_BUYIN_SWITCH
#define MACHINE_LAUNCH_SWITCH

/* Define the standard solenoids */
#define MACHINE_BALL_SERVE_SOLENOID

/* Define optional solenoids */
#define MACHINE_LAUNCH_SOLENOID

/* Define the standard lamps */
#define MACHINE_START_LAMP		LM_START_BUTTON

/* Define optional lamps */
#define MACHINE_BUYIN_LAMP

/* Declare the array of opto bits, defined elsewhere */
extern const uint8_t mach_opto_mask[];

/* Declare the array of edge switches, defined elsewhere */
extern const uint8_t mach_edge_switches[];

/* Declare the global machine hooks */
#define MACHINE_HOOKS tz_hooks

void amode_left_flipper (void);
void amode_right_flipper (void);


#endif /* MACH_CONFIG_H */
