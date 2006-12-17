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

/**
 * \file
 * \brief Definitions/macros related to the WPC hardware
 */

#ifndef _WPC_H
#define _WPC_H


/***************************************************************
 * ASIC interface
 *
 * The intent of these functions is to encapsulate all I/O
 * reads and writes, so that they can be simulated in
 * environments where a direct memory map is not present.
 ***************************************************************/
extern inline void wpc_asic_write (U16 addr, U8 val)
{
#ifdef CONFIG_PLATFORM_LINUX
	extern void linux_asic_write (U16 addr, U8 val);
	linux_asic_write (addr, val);
#else
	*(volatile U8 *)addr = val;
#endif
}

extern inline U8 wpc_asic_read (U16 addr)
{
#ifdef CONFIG_PLATFORM_LINUX
	extern U8 linux_asic_read (U16 addr);
	return linux_asic_read (addr);
#else
	return *(volatile U8 *)addr;
#endif
}

extern inline void wpc_asic_xor (U16 addr, U8 val)
{
#ifdef CONFIG_PLATFORM_LINUX
	U8 reg = wpc_asic_read (addr);
	reg ^= val;
	wpc_asic_write (addr, val);
#else
	*(volatile U8 *)addr ^= val;
#endif
}

extern inline void wpc_asic_setbits (U16 addr, U8 val)
{
#ifdef CONFIG_PLATFORM_LINUX
	U8 reg = wpc_asic_read (addr);
	reg |= val;
	wpc_asic_write (addr, val);
#else
	*(volatile U8 *)addr |= val;
#endif
}

extern inline void wpc_asic_clearbits (U16 addr, U8 val)
{
#ifdef CONFIG_PLATFORM_LINUX
	U8 reg = wpc_asic_read (addr);
	reg &= ~val;
	wpc_asic_write (addr, val);
#else
	*(volatile U8 *)addr &= ~val;
#endif
}


/***************************************************************
 * Peripheral timer
 ***************************************************************/

/** The FIRQ clear/peripheral timer register bits */
#define FIRQ_CLEAR_BIT 0x80


/***************************************************************
 * Memory usage
 ***************************************************************/

/** The total size of RAM  -- 8K */
#define RAM_SIZE 			0x2000UL

/** The usable, nonprotected area of RAM -- the first 6K */
#define USER_RAM_SIZE	0x1800UL

/** The protected RAM size -- whatever is left */
#define NVRAM_SIZE	   (RAM_SIZE - USER_RAM_SIZE)

/** The base address of the (dynamic) heap */
#define HEAP_BASE 		0x800UL

/** The base address of the stack */
#define STACK_BASE 		(USER_RAM_SIZE - 0x8)
#define STACK_SIZE      0x200UL
#define STACK_MIN			(STACK_BASE - STACK_SIZE)

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
 * ASIC / DMD memory map
 ***************************************************************/

#ifdef CONFIG_PLATFORM_LINUX
extern U8 *linux_dmd_low_page;
extern U8 *linux_dmd_high_page;
#define DMD_LOW_BASE linux_dmd_low_page
#define DMD_HIGH_BASE linux_dmd_high_page
#else
#define DMD_LOW_BASE 					0x3800
#define DMD_HIGH_BASE 					0x3A00
#endif

#define WPC_DEBUG_DATA_PORT			0x3D60
#define WPC_DEBUG_CONTROL_PORT		0x3D61
#define WPC_PINMAME_CYCLE_COUNT		0x3D62
#define WPC_PINMAME_FUNC_ENTRY_HI	0x3D63
#define WPC_PINMAME_FUNC_ENTRY_LO	0x3D64
#define WPC_PINMAME_FUNC_EXIT_HI		0x3D65
#define WPC_PINMAME_FUNC_EXIT_LO		0x3D66

#define WPC_SERIAL_CONTROL_PORT 		0x3E66
#define WPC_SERIAL_DATA_PORT 			0x3E67

#define WPC_DMD_3200_PAGE				0x3FB8
#define WPC_DMD_3000_PAGE				0x3FB9
#define WPC_DMD_3600_PAGE				0x3FBA
#define WPC_DMD_3400_PAGE				0x3FBB
#define WPC_DMD_HIGH_PAGE 				0x3FBC
#define WPC_DMD_FIRQ_ROW_VALUE 		0x3FBD /* FIRQLINE */
#define WPC_DMD_LOW_PAGE 				0x3FBE
#define WPC_DMD_ACTIVE_PAGE 			0x3FBF /* VISIBLEPAGE */

#define WPC_SERIAL_STATUS_PORT 		0x3FC0
#define WPC_PARALLEL_DATA_PORT 		0x3FC1
#define WPC_PARALLEL_STROBE_PORT 	0x3FC2
#define WPC_SERIAL_DATA_OUTPUT 		0x3FC3
#define WPC_SERIAL_CONTROL_OUTPUT	0x3FC4
#define WPC_SERIAL_BAUD_SELECT 		0x3FC5
#define WPC_TICKET_DISPENSE 			0x3FC6

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

#define WPC_SW_ROW_INPUT 				0x3FE9
#define WPC_SW_COL_STROBE 				0x3FEA
#if (MACHINE_PIC == 1)
#define WPCS_PIC_READ 					0x3FE9
#define WPCS_PIC_WRITE 					0x3FEA
#endif

#if (MACHINE_DMD == 0)
#define WPC_ALPHA_POS 					0x3FEB
#define WPC_ALPHA_ROW1 					0x3FEC
#else
#define WPC_EXTBOARD1 					0x3FEB
#define WPC_EXTBOARD2 					0x3FEC
#define WPC_EXTBOARD3 					0x3FED
#endif

#if (MACHINE_WPC95 == 1)
#define WPC95_FLIPPER_COIL_OUTPUT 	0x3FEE
#define WPC95_FLIPPER_SWITCH_INPUT 	0x3FEF
#else
#endif

#if (MACHINE_DMD == 0)
#define WPC_ALPHA_ROW2 					0x3FEE
#else
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

/********************************************/
/* LED                                      */
/********************************************/

extern inline void wpc_led_toggle (void)
{
	wpc_asic_xor (WPC_LEDS, 0x80);
}


/********************************************/
/* Printer / Parallel Port                  */
/********************************************/

extern inline void wpc_parport_write (U8 data)
{
	wpc_asic_write (WPC_PARALLEL_DATA_PORT, data);
	wpc_asic_write (WPC_PARALLEL_STROBE_PORT, 0x0);
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	asm ("nop");
	wpc_asic_write (WPC_PARALLEL_STROBE_PORT, 0x1);
}

/********************************************/
/* RAM Protection Circuit                   */
/********************************************/

extern inline void wpc_set_ram_protect (U8 prot)
{
	wpc_asic_write (WPC_RAM_LOCK, prot);
}

extern inline void wpc_set_ram_protect_size (U8 sz)
{
	wpc_asic_write (WPC_RAM_LOCKSIZE, sz);
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
	return wpc_asic_read (WPC_ROM_BANK);
}

extern inline void wpc_set_rom_page (U8 page)
{
	wpc_asic_write (WPC_ROM_BANK, page);
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
	return *(volatile U8 *)WPC_RAM_BANK;
}

extern inline void wpc_set_ram_page (U8 page)
{
	*(volatile U8 *)WPC_RAM_BANK = page;
}

/********************************************/
/* LED Register                             */
/********************************************/

#define LED_DIAGNOSTIC		0x80

/* 0x4 | 0x2 are always set when writing this register.
 * One of these is probably to reset the hardware watchdog.
 * 0x80 and 0x10 are also set when clearing the IRQ from the
 * IRQ handler.  These are probably interrupt enable/status
 * lines.
 */

/********************************************/
/* Zero Crossing/IRQ Clear Register         */
/********************************************/

extern inline void wpc_write_irq_clear (U8 val)
{
	wpc_asic_write (WPC_ZEROCROSS_IRQ_CLEAR, val);
}

extern inline U8 wpc_read_ac_zerocross (void)
{
	U8 val = wpc_asic_read (WPC_ZEROCROSS_IRQ_CLEAR);
	return (val & 0x80);
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
	return wpc_asic_read (WPC_FLIPTRONIC_PORT_A);
}


extern inline void wpc_write_flippers (U8 val)
{
	wpc_asic_write (WPC_FLIPTRONIC_PORT_A, val);
}


extern inline U8 wpc_get_jumpers (void)
{
	return wpc_asic_read (WPC_SW_JUMPER_INPUT);
}


extern inline U8 wpc_read_ticket (void)
{
	return wpc_asic_read (WPC_TICKET_DISPENSE);
}


extern inline void wpc_write_ticket (U8 val)
{
	wpc_asic_write (WPC_TICKET_DISPENSE, val);
}


#endif /* _WPC_H */

