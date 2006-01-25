
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

/* Generic define used to indicate some TZ-specific function.
 * In the long run, all of these instances should be replaced with
 * something more meaningful, like a callback or well-named hook,
 * so that it can be applied to all machines.
 */
#define MACHINE_TZ

#define MACHINE_NAME "TWILIGHT ZONE"

#define MACHINE_MAJOR_VERSION 0
#define MACHINE_MINOR_VERSION 05


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
#undef MACHINE_LAUNCH_SWITCH
#define MACHINE_OUTHOLE_SWITCH	SW_OUTHOLE

/* Define the number and location of the trough switches.
 * The order should proceed from the one nearest the outhole
 * to the plunger.  You can use the name of the switch as
 * defined in mach/switch.h */
#define MACHINE_TROUGH_SIZE 3
#define MACHINE_TROUGH1		SW_LEFT_TROUGH
#define MACHINE_TROUGH2		SW_CENTER_TROUGH
#define MACHINE_TROUGH3		SW_RIGHT_TROUGH

/* Define the standard solenoids */
#define MACHINE_BALL_SERVE_SOLENOID	SOL_BALL_SERVE

/* Define optional solenoids */
#undef MACHINE_LAUNCH_SOLENOID

/* Define the standard lamps */
#define MACHINE_START_LAMP		LM_START_BUTTON

/* Define optional lamps */
#define MACHINE_BUYIN_LAMP

/* Define sound effects for standard system functions */
#define MACHINE_ADD_COIN_SOUND		SND_LIGHT_SLOT_TIMED
#define MACHINE_ADD_CREDIT_SOUND		SND_THUD
#define MACHINE_VOLUME_CHANGE_MUSIC MUS_SUPER_SLOT


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
 * for devices unique to each machine. */
#define MACHINE_TEST_MENU_ITEMS \
	{ "AUTOFIRE LAUNCH", &main_menu_items[4], 0, 0, TEST_ITEM(autofire_launch_proc) }, \
	{ "LOAD GUMBALL", &main_menu_items[4], 0, 0, TEST_ITEM(autofire_to_gumball_proc) }, \
	{ "RELEASE GUMBALL", &main_menu_items[4], 0, 0, TEST_ITEM(release_gumball_proc) },

/* Declare the array of opto bits, defined elsewhere */
extern const uint8_t mach_opto_mask[];

/* Declare the array of edge switches, defined elsewhere */
extern const uint8_t mach_edge_switches[];

/* Declare the global machine hooks */
#define MACHINE_HOOKS tz_hooks

/* Declare the display effects that this machine implements */
#define MACHINE_DISPLAY_EFFECTS \
	DECL_DEFF (DEFF_AMODE, D_RUNNING, 10, amode_deff) \
	DECL_DEFF (DEFF_BRIAN_IMAGE, D_NORMAL, 20, egg_brian_image_deff) \
	DECL_DEFF (DEFF_BONUS, D_RUNNING, 150, bonus_deff) \
	DECL_DEFF (DEFF_REPLAY, D_NORMAL, 202, replay_deff) \
	DECL_DEFF (DEFF_JACKPOT, D_NORMAL, 202, jackpot_deff) \
	DECL_DEFF (DEFF_SPECIAL, D_NORMAL, 202, special_deff) \
	DECL_DEFF (DEFF_EXTRA_BALL, D_NORMAL, 202, extra_ball_deff) \
	DECL_DEFF (DEFF_BALL_SAVE, D_NORMAL, 202, ballsave_deff) \
	DECL_DEFF (DEFF_DOOR_AWARD, D_NORMAL, 202, door_award_deff) \

/* Declare the lamp effects that this machine implements */
#define MACHINE_LAMP_EFFECTS \
DECL_LEFF (LEFF_AMODE, L_RUNNING, 10, LAMPSET_AMODE_ALL, L_NOGI, amode_leff) \
DECL_LEFF (LEFF_NO_GI, L_NORMAL, 20, L_NOLAMPS, L_NOGI, no_gi_leff) \
DECL_LEFF (LEFF_LEFT_RAMP, L_NORMAL, 50, L_NOLAMPS, L_NOGI, left_ramp_leff) \
DECL_LEFF (LEFF_FLASH_ALL, L_NORMAL, 100, LAMPSET_AMODE_ALL, L_NOGI, flash_all_leff) \
DECL_LEFF (LEFF_FLASHER_HAPPY, L_NORMAL, 140, L_NOLAMPS, L_NOGI, flasher_happy_leff) \
DECL_LEFF (LEFF_BONUS, L_RUNNING, 150, L_ALL_LAMPS, L_NOGI, bonus_leff) \
DECL_LEFF (LEFF_TILT_WARNING, L_RUNNING, 200, L_ALL_LAMPS, L_NOGI, no_lights_leff) \
DECL_LEFF (LEFF_TILT, L_RUNNING, 205, L_ALL_LAMPS, L_NOGI, no_lights_leff) \
DECL_LEFF (LEFF_SLOT_KICKOUT, L_NORMAL, 205, L_ALL_LAMPS, L_NOGI, slot_kickout_leff) \



/* Externs for any functions system needs to see */
void piano_ball_start (void);
void slot_ball_start (void);
void autofire_launch_proc (void);
void autofire_to_gumball_proc (void);
void release_gumball_proc (void);

#endif /* MACH_CONFIG_H */
