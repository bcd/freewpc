
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

#define MACHINE_NAME "WORLD CUP"

#define MACHINE_MAJOR_VERSION 0
#define MACHINE_MINOR_VERSION 0

#define MACHINE_DATE "12/05/2005"



/* Define as 1 if the machine has a DMD, 0 if it is alphanumeric */
#define MACHINE_DMD 1

/* Define as 1 if the machine has DCS sound, 0 if older WPC style */
#define MACHINE_DCS 1

/* Define as 1 if the game uses Fliptronic flippers */
#define MACHINE_FLIPTRONIC 0

/* Define as 1 if the game uses a Security PIC chip */
#define MACHINE_PIC 1

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
#undef MACHINE_LAUNCH_SWITCH

/* Define the number and location of the trough switches.
 * The order should proceed from the one nearest the outhole
 * to the plunger.  You can use the name of the switch as
 * defined in mach/switch.h */
#define MACHINE_TROUGH_SIZE 5
#define MACHINE_TROUGH1		SW_TROUGH_5
#define MACHINE_TROUGH2		SW_TROUGH_4
#define MACHINE_TROUGH3		SW_TROUGH_3
#define MACHINE_TROUGH4		SW_TROUGH_2
#define MACHINE_TROUGH5		SW_TROUGH_1

/* Define the standard solenoids */
#define MACHINE_BALL_SERVE_SOLENOID SOL_TROUGH

/* Define optional solenoids */
#undef MACHINE_LAUNCH_SOLENOID

/* Define the standard lamps */
#define MACHINE_START_LAMP		LM_START_BUTTON

/* Define optional lamps */
#define MACHINE_BUYIN_LAMP

/* Define the names of functions to handle flipper buttons
 * in attract mode.  Leave undefined if not needed.
 * The functions should not take any arguments. */
#undef MACHINE_AMODE_LEFT_FLIPPER_HANDLER
#undef MACHINE_AMODE_RIGHT_FLIPPER_HANDLER

/* Define if the game provides its own attract mode functions.
 * If so, they must be named amode_start() and amode_stop().
 * If undefined, the system will use a default function.
 */
#undef MACHINE_CUSTOM_AMODE

/* Declare the array of opto bits, defined elsewhere */
extern const uint8_t mach_opto_mask[];

/* Declare the array of edge switches, defined elsewhere */
extern const uint8_t mach_edge_switches[];

/* Declare the display effects that this machine implements */


/* Declare the lamp effects that this machine implements */
#undef MACHINE_LAMP_EFFECTS

void piano_ball_start (void);
void slot_ball_start (void);
/* Declare the global machine hooks */
#define MACHINE_HOOKS wcs_hooks


#endif /* MACH_CONFIG_H */
