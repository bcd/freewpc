
#ifndef _MACH_CONFIG_H
#define _MACH_CONFIG_H

/*
 * Game specific configuration
 *
 * This file contains tons of definitions used by the main kernel
 * to implement machine-specific behavior.
 *
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

/* Define the number of digits to use for "standard" score values. */
#define MACHINE_SCORE_DIGITS 8

#endif /* MACH_CONFIG_H */
