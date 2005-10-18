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
typedef unsigned char bool;
typedef unsigned char bcd_t;
typedef char int8_t, I8, S8;
typedef unsigned char uint8_t, U8;
typedef int int16_t, I16, S16;
typedef unsigned int uint16_t, U16;

/*
 * Define TRUE and FALSE to suitable values.
 */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0


/*
 * Define the size of the process stack (currently here
 * because the assembler code needs it.
 *
 * This value + 21 should be a nice round number.
 * Original : 43 ==> Total size of 64
 */
/// #define TASK_STACK_SIZE		43   /* sometimes fails with deffs */
#define TASK_STACK_SIZE		75

/* Include the standard header files that are needed
 * by most modules */
#include <asm-6809.h>
#include <wpc.h>
#include <version.h>
#include <sys/bitarray.h>
#include <sys/errno.h>
#include <sys/task.h>
#include <sys/font.h>
#include <sys/sol.h>
#include <sys/lamp.h>
#include <sys/switch.h>
#include <sys/flip.h>
#include <sys/sound.h>
#include <sys/dmd.h>
#include <sys/triac.h>

#include <sys/deff.h>
#include <sys/debug.h>
#include <sys/device.h>
#include <score.h>
#include <coin.h>
#include <game.h>
#include <stdadj.h>
#include <printf.h>
#include <amode.h>
#include <mach/config.h>
#include <mach/switch.h>
#include <mach/coil.h>
#include <mach/sound.h>
#include <mach/lamp.h>

/* From here on are any headers that are dependent on machine
 * specific features being defined */
#include <hook.h>

#endif /* _FREEWPC_H */
