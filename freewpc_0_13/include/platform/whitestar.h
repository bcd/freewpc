/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Definitions/macros related to the Sega Whitestar hardware
 * Note: this is not yet implemented and only a stub.
 */

#ifndef _WHITESTAR_H
#define _WHITESTAR_H


/***************************************************************
 * Memory usage
 ***************************************************************/

/** The total size of RAM  -- 8K */
#define RAM_SIZE 			0x2000UL

/** The usable, nonprotected area of RAM -- the first 6K */
#define USER_RAM_SIZE	0x1800UL

/** The protected RAM size -- whatever is left */
#define NVRAM_SIZE	   (RAM_SIZE - USER_RAM_SIZE)

/** The base address of the stack */
#define STACK_BASE 		(USER_RAM_SIZE - 0x8)
#define STACK_SIZE      0x200UL

/** The layout of the player local area */
#define LOCAL_BASE		((U8 *)0x1200)
#define LOCAL_SIZE		0xA0U

#define LOCAL_SAVE_BASE(p)	(LOCAL_BASE + (LOCAL_SIZE * (p)))

/** The layout of the malloc area */
#define MALLOC_BASE		((U8 *)0x1400)
#define MALLOC_SIZE		0x200UL

/***************************************************************
 * System timing
 ***************************************************************/

/* A "tick" is defined as the minimum unit of time that is
 * tracked by the kernel.  A tick represents some multiple of
 * the IRQ frequency, since the IRQ happens more often than
 * we need for timing granularity.
 *
 * We define this to be 16 now, which means that every 16 IRQs,
 * we increment our "tick counter" by 1.  The tick then
 * represents about 16.66ms of actual time.
 *
 * The tick count is stored in a single byte field of the
 * task structure, so it can store up to about 4 seconds.
 * If you need to sleep longer than this, use 'task_sleep_sec'
 * instead of 'task_sleep'.
 */
#define IRQS_PER_TICK 16

/* Primitive time constants */
#define TIME_16MS 	1U
#define TIME_33MS 	2U
#define TIME_50MS 	3U
#define TIME_66MS 	(TIME_33MS * 2U)
#define TIME_100MS 	(TIME_33MS * 3U)
#define TIME_133MS 	(TIME_33MS * 4U)
#define TIME_166MS 	(TIME_33MS * 5U)
#define TIME_200MS	(TIME_100MS * 2U)
#define TIME_300MS	(TIME_100MS * 3U)
#define TIME_400MS	(TIME_100MS * 4U)
#define TIME_500MS	(TIME_100MS * 5U)
#define TIME_1S 		(TIME_100MS * 10U) /* 4 * 3 * 10 = 120 ticks */
#define TIME_2S 		(TIME_1S * 2U)     /* 240 ticks */

/*
 * These time values can only be used for timer functions.
 */
#define TIME_3S 		(TIME_1S * 3UL)
#define TIME_4S 		(TIME_1S * 4UL)
#define TIME_5S 		(TIME_1S * 5UL)
#define TIME_6S 		(TIME_1S * 6UL)
#define TIME_7S 		(TIME_1S * 7UL)
#define TIME_8S 		(TIME_1S * 8UL)
#define TIME_9S 		(TIME_1S * 9UL)
#define TIME_10S 		(TIME_1S * 10UL)
#define TIME_15S 		(TIME_1S * 15UL)
#define TIME_30S 		(TIME_1S * 30UL)

/***************************************************************
 * RAM Protection Circuit
 ***************************************************************/

/** When the lock register(s) contain these values, the upper
 * N bytes of the RAM are write protected. */
#define RAM_LOCK_4K			0x0
#define RAM_LOCK_2K			0x1
#define RAM_LOCK_1K			0x3
#define RAM_LOCK_512			0x7
#define RAM_LOCK_256			0xF

#define RAM_LOCKED			0x0

/** When the lock register contains this value, the memory
 * protection circuit is disabled */
#define RAM_UNLOCKED			0xB4

/***************************************************************
 * CPU I/O memory map
 ***************************************************************/

#define WS_IO_SOLA               0x2000
#define WS_IO_SOLB               0x2001
#define WS_IO_SOLC               0x2002
#define WS_IO_FLMP               0x2003
#define WS_IO_FLIP0              0x2004
#define WS_IO_FLIP1              0x2005
#define WS_IO_AUX0               0x2006
#define WS_IO_AUX1               0x2007
#define WS_IO_LAMP_STROBE        0x2008
#define WS_IO_LAMP_ROW_OUT       0x200A
#define WS_IO_SW_COIN_DOOR       0x3000
#define WS_IO_SW_DIP             0x3100
#define WS_IO_ROM_PAGE           0x3200
#define WS_IO_SW_COL_STROBE      0x3300
#define WS_IO_SW_ROW_OUTPUT      0x3400
#define WS_IO_DMD_INPUT          0x3500
#define WS_IO_DMD_OUTPUT         0x3600
#define WS_IO_DMD_RESET          0x3601
#define WS_IO_DMD_STATUS_INPUT   0x3700
#define WS_IO_1800               0x3800

extern inline void wpc_led_toggle (void)
{
}

/********************************************/
/* RAM Protection Circuit                   */
/********************************************/

extern inline void wpc_set_ram_protect (U8 prot)
{
}

extern inline void wpc_set_ram_protect_size (U8 sz)
{
}

#define wpc_nvram_get()		wpc_set_ram_protect(RAM_UNLOCKED)
#define wpc_nvram_put()		wpc_set_ram_protect(RAM_LOCKED)

#define wpc_nvram_add(var,n) \
	do { \
		volatile typeof(var) *pvar = &var; \
		wpc_nvram_get (); \
		*pvar += n; \
		wpc_nvram_put (); \
	} while (0)

#define wpc_nvram_subtract(var,n) \
	do { \
		volatile typeof(var) *pvar = &var; \
		wpc_nvram_get (); \
		*pvar -= n; \
		wpc_nvram_put (); \
	} while (0)

/********************************************/
/* DIP Switches                             */
/********************************************/

/********************************************/
/* ROM Paging                               */
/********************************************/

extern inline U8 wpc_get_rom_page (void)
{
}

extern inline void wpc_set_rom_page (U8 page)
{
}

#define call_far(page, fncall) \
do { \
	U8 __saved_page = wpc_get_rom_page (); \
	wpc_set_rom_page (page); \
	fncall; \
	wpc_set_rom_page (__saved_page); \
} while (0)


#define wpc_push_page(page) \
{ \
	U8 __saved_page = wpc_get_rom_page (); \
	wpc_set_rom_page (page);


#define wpc_pop_page() \
	wpc_set_rom_page (__saved_page); \
}

/********************************************/
/* RAM Paging                               */
/********************************************/

extern inline U8 wpc_get_ram_page (void)
{
}

extern inline void wpc_set_ram_page (U8 page)
{
}

/********************************************/
/* LED Register                             */
/********************************************/

#define LED_DIAGNOSTIC		0x80


/********************************************/
/* Zero Crossing/IRQ Clear Register         */
/********************************************/

extern inline void wpc_write_irq_clear (U8 val)
{
}

extern inline U8 wpc_read_ac_zerocross (void)
{
}


/***************************************************************
 * Flippers
 ***************************************************************/

#define WPC_LR_FLIP_POWER	0x1
#define WPC_LR_FLIP_HOLD	0x2
#define WPC_LL_FLIP_POWER	0x4
#define WPC_LL_FLIP_HOLD	0x8
#define WPC_UR_FLIP_POWER	0x10
#define WPC_UR_FLIP_HOLD	0x20
#define WPC_UL_FLIP_POWER	0x40
#define WPC_UL_FLIP_HOLD	0x80

#define WPC_LR_FLIP_EOS		0x1
#define WPC_LR_FLIP_SW		0x2
#define WPC_LL_FLIP_EOS		0x4
#define WPC_LL_FLIP_SW		0x8
#define WPC_UR_FLIP_EOS		0x10
#define WPC_UR_FLIP_SW		0x20
#define WPC_UL_FLIP_EOS		0x40
#define WPC_UL_FLIP_SW		0x80

extern inline U8 wpc_read_flippers (void)
{
}


extern inline void wpc_write_flippers (U8 val)
{
}


extern inline U8 wpc_get_jumpers (void)
{
}


#endif /* _WHITESTAR_H */

