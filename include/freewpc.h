#ifndef _FREEWPC_H
#define _FREEWPC_H

/*
 * This is the top-level include file for FreeWPC.  All files
 * should include this one first in order to get all of the
 * required definitions and a lot of the more commonly used
 * features.
 */


/*
 * Standard types
 */
#ifndef __SASM__
typedef unsigned char bool;
typedef unsigned char bcd_t;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef int int16_t;
typedef unsigned int uint16_t;
#endif

/*
 * Define TRUE and FALSE to suitable values.
 */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0


/* Include the standard header files that are needed
 * by most modules */
#include <asm-6809.h>
#include <wpc.h>
#include <version.h>
#ifndef __SASM__
#include <sys/bitarray.h>
#include <sys/errno.h>
#include <sys/task.h>
#endif
#include <sys/sol.h>
#include <sys/lamp.h>
#include <sys/switch.h>
#include <sys/sound.h>
#include <sys/dmd.h>
#include <sys/triac.h>

#include <sys/segment.h>
#ifndef __SASM__
#include <sys/deff.h>
#include <sys/debug.h>
#include <sys/device.h>
#include <score.h>
#include <coin.h>
#include <game.h>
#include <stdadj.h>

#include <mach/config.h>
#include <mach/switch.h>
#include <mach/coil.h>
#include <mach/sound.h>
#endif

#endif /* _FREEWPC_H */
