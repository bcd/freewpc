
/*
 * WPC platform definitions
 *
 * This file contains definitions that are common to the WPC platform.
 *
 */

#ifndef _WPC_H
#define _WPC_H

#ifndef __SASM__

typedef unsigned char bool;
typedef unsigned char bcd_t;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef int int16_t;
typedef unsigned int uint16_t;

#include <stdlib.h>
#include <string.h>
#include <env.h>
/* Including sys/types.h is almost always wrong! */

#endif

#include <sys/errno.h>

#define DEBUG

#define BLKB	blkb
#define BLKW	blkw

#define S(x)	#x
#define STR(x)	S(x)

/***************************************************************
 * WPC generations
 ***************************************************************/

#define WPC_ALPHA 		0
#define WPC_FLIPTRONIC 	1
#define WPC95  			2
#define WPC_DMD 			3

#ifndef WPC_FAMILY
#define WPC_FAMILY 		WPC_DMD
#endif

/***************************************************************
 * Properties of the the 6809 CPU
 ***************************************************************/

#define BITS_PER_BYTE 8
#define BITS_PER_WORD 16

#define CC_CARRY 		0x1
#define CC_OVERFLOW 	0x2
#define CC_ZERO 		0x4
#define CC_NEGATIVE 	0x8
#define CC_IRQ 		0x10
#define CC_HALF 		0x20
#define CC_FIRQ 		0x40
#define CC_E 			0x80

/* The FIRQ clear/peripheral timer register bits */
#define FIRQ_CLEAR_BIT 0x80


/***************************************************************
 * Paging
 ***************************************************************/

/* Offset of the offset and page parts of a far pointer */
#define FP_OFFSET			0
#define FP_PAGE			2

/***************************************************************
 * Memory usage
 ***************************************************************/

/* The base address of the (dynamic) heap */
#define HEAP_BASE 		0x800

/* The base address of the stack */
#define STACK_BASE 		0x1800

/* The total size of RAM */
#define RAM_SIZE 			0x2000

/* The usable, nonprotected area of RAM */
#define USER_RAM_SIZE	0x1000


/***************************************************************
 * System timing
 ***************************************************************/

/* A "tick" is defined as the minimum unit of time that is
 * tracked by the kernel.  A tick represents some multiple of
 * the IRQ frequency, since the IRQ happens more often than
 * we need for timing granularity.
 *
 * We define this to be 8 now, which means that every 8 IRQs,
 * we increment our "tick counter" by 1.  The tick then
 * represents about 8.33ms of actual time.
 */
#define IRQS_PER_TICK 8

/* Primitive time constants */
#define TIME_16MS 	2
#define TIME_33MS 	4
#define TIME_66MS 	(TIME_33MS * 2)
#define TIME_100MS 	(TIME_33MS * 3)
#define TIME_1S 		(TIME_100MS * 10)

/***************************************************************
 * RAM Protection Circuit
 ***************************************************************/

/* When the lock register(s) contain these values, the upper
 * N bytes of the RAM are write protected. */
#define RAM_LOCK_512			0x1

/* When the lock register contains this value, the memory
 * protection circuit is disabled */
#define RAM_UNLOCKED			0xB4

/***************************************************************
 * ASIC / DMD memory map
 ***************************************************************/

#define DMD_LOW_BASE 					0x3800
#define DMD_HIGH_BASE 					0x3A00

#define WPC_SERIAL_CONTROL_PORT 		0x3E66
#define WPC_SERIAL_DATA_PORT 			0x3E67
#define WPC_DMD_HIGH_PAGE 				0x3FBC
#define WPC_DMD_FIRQ_ROW_VALUE 		0x3FBD
#define WPC_DMD_LOW_PAGE 				0x3FBE
#define WPC_DMD_ACTIVE_PAGE 			0x3FBF
#define WPC_SERIAL_STATUS_PORT 		0x3FC0
#define WPC_PARALLEL_DATA_PORT 		0x3FC1
#define WPC_PARALLEL_STROBE_PORT 	0x3FC2
#define WPC_SERIAL_DATA_OUTPUT 		0x3FC3
#define WPC_SERIAL_CONTROL_OUTPUT	0x3FC4
#define WPC_SERIAL_BAUD_SELECT 		0x3FC5
#define WPC_TICKET_DISPENSE 			0x3FC6

#define WPC_RESERVED1					0x3FC7
#define WPC_RESERVED2					0x3FC8
#define WPC_RESERVED3					0x3FC9
#define WPC_RESERVED4					0x3FCA
#define WPC_RESERVED5					0x3FCB
#define WPC_RESERVED6					0x3FCC
#define WPC_RESERVED7					0x3FCD
#define WPC_RESERVED8					0x3FCE
#define WPC_RESERVED9					0x3FCF

#define WPC_DCS_SOUND_DATA_OUT 		0x3FD0
#define WPC_DCS_SOUND_DATA_IN 		0x3FD1
#define WPC_DCS_SOUND_RESET 			0x3FD2
#define WPC_DCS_SOUND_DATA_READY 	0x3FD3
#define WPC_FLIPTRONIC_PORT_A 		0x3FD4
#define WPC_FLIPTRONIC_PORT_B 		0x3FD5
#define WPCS_DATA 						0x3FDC
#define WPCS_CONTROL_STATUS 			0x3FDD
#define WPC_SOL_FLASH2_OUTPUT 		0x3FE0
#define WPC_SOL_HIGHPOWER_OUTPUT 	0x3FE1
#define WPC_SOL_FLASH1_OUTPUT 		0x3FE2
#define WPC_SOL_LOWPOWER_OUTPUT 		0x3FE3
#define WPC_LAMP_ROW_OUTPUT 			0x3FE4
#define WPC_LAMP_COL_STROBE 			0x3FE5
#define WPC_GI_TRIAC 					0x3FE6
#define WPC_SW_JUMPER_INPUT 			0x3FE7
#define WPC_SW_CABINET_INPUT 			0x3FE8

#if (WPC_FAMILY == WPC95)
#define WPC95_SW_PIC_READ 				0x3FE9
#define WPC95_SW_PIC_WRITE 			0x3FEA
#else
#define WPC_SW_ROW_INPUT 				0x3FE9
#define WPC_SW_COL_STROBE 				0x3FEA
#endif

#if (WPC_FAMILY == WPC_ALPHA)
#define WPC_ALPHA_POS 					0x3FEB
#define WPC_ALPHA_ROW1 					0x3FEC
#else
#define WPC_EXTBOARD1 					0x3FEB
#define WPC_EXTBOARD2 					0x3FEC
#define WPC_EXTBOARD3 					0x3FED
#endif

#if (WPC_FAMILY == WPC95)
#else
#define WPC95_FLIPPER_COIL_OUTPUT 	0x3FEE
#define WPC95_FLIPPER_SWITCH_INPUT 	0x3FEF
#endif

#if (WPC_FAMILY == WPC_ALPHA)
#else
#define WPC_ALPHA_ROW2 					0x3FEE
#endif

#define WPC_LEDS 							0x3FF2
#define WPC_RAM_BANK 					0x3FF3
#define WPC_SHIFTADDR 					0x3FF4
#define WPC_SHIFTBIT 					0x3FF6
#define WPC_SHIFTBIT2 					0x3FF7
#define WPC_PERIPHERAL_TIMER_FIRQ_CLEAR 0x3FF8
#define WPC_ROM_LOCK 					0x3FF9
#define WPC_CLK_HOURS_DAYS 			0x3FFA
#define WPC_CLK_MINS 					0x3FFB
#define WPC_ROM_BANK 					0x3FFC
#define WPC_RAM_LOCK 					0x3FFD
#define WPC_RAM_LOCKSIZE 				0x3FFE
#define WPC_ZEROCROSS_IRQ_CLEAR 		0x3FFF


/***************************************************************
 * Task Groups
 ***************************************************************/

#define GID_NONE						0
#define GID_TEST_BANNER				1
#define GID_DEFF						2
#define GID_DEFF_POPUP				3
#define GID_TEST_LOOP				4
#define GID_DEFF_EXITING			5


/****************  Macros  ***************************/
#if defined(__SASM__)
#include "wpc.m4"
#endif


#endif /* _WPC_H */

