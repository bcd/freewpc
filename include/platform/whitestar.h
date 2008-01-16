/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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
 * \brief Definitions/macros specific to the Whitestar hardware
 */

#ifndef _WHITESTAR_H
#define _WHITESTAR_H



/***************************************************************
 * Memory usage
 ***************************************************************/

#ifdef __m6809__

#define ASM_DECL(name) name asm (#name)

#define AREA_DECL(name) extern U8 ASM_DECL (s_ ## name); extern U8 ASM_DECL (l_ ## name);
#define AREA_BASE(name) (&s_ ## name)
#define AREA_SIZE(name) ((U16)(&l_ ## name))

AREA_DECL(direct)
AREA_DECL(ram)
AREA_DECL(local)
AREA_DECL(heap)
AREA_DECL(stack)
AREA_DECL(nvram)

#else
/* TODO */
#endif /* __m6809__ */


/** The total size of RAM  -- 8K */
#define RAM_SIZE 			0x2000UL

/** The usable, nonprotected area of RAM -- the first 6K */
#define USER_RAM_SIZE	0x1800UL

/** The base address of the stack */
#define STACK_BASE 		(USER_RAM_SIZE - 0x8)
#define STACK_SIZE      0x200UL

/** The layout of the player local area.
 * There are 5 "copies" of the local area: the lowest address is active
 * for the current player up, and the next 4 are save areas to hold
 * values between players in a multi-player game. */
#define LOCAL_BASE		AREA_BASE(local)
#define LOCAL_SIZE		0x40U

#define LOCAL_SAVE_BASE(p)	(LOCAL_BASE + (LOCAL_SIZE * (p)))

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
#define TIME_1S 		(TIME_100MS * 10U) /* 2 * 3 * 10 = 60 ticks */
#define TIME_2S 		(TIME_1S * 2U)     /* 120 ticks */
#define TIME_3S 		(TIME_1S * 3UL)
#define TIME_4S 		(TIME_1S * 4UL)

/*
 * These time values can only be used for low-level timers,
 * because they use a 16-bit ticks field.
 */
#define TIME_5S 		(TIME_1S * 5UL)
#define TIME_6S 		(TIME_1S * 6UL)
#define TIME_7S 		(TIME_1S * 7UL)
#define TIME_8S 		(TIME_1S * 8UL)
#define TIME_9S 		(TIME_1S * 9UL)
#define TIME_10S 		(TIME_1S * 10UL)
#define TIME_15S 		(TIME_1S * 15UL)
#define TIME_30S 		(TIME_1S * 30UL)


/***************************************************************
 * ASIC memory map
 ***************************************************************/

#define WS_SOLA     0x2000
#define WS_SOLB     0x2001
#define WS_SOLC     0x2002
#define WS_FLASHERS 0x2003
#define WS_FLIP0    0x2004
#define WS_FLIP1    0x2005
#define WS_AUX0     0x2006
#define WS_AUX1     0x2007
#define WS_LAMP_COLUMN_STROBE 0x2008
#define WS_LAMP_ROW_OUTPUT 0x200A

#define WS_DEDICATED 0x3000
#define WS_DIP_SWITCHES 0x3100
#define WS_ROM_PAGE 0x3200
#define WS_SWITCH_COLUMN_STROBE 0x3300
#define WS_SWITCH_ROW_INPUT 0x3400


/********************************************/
/* LED                                      */
/********************************************/

/** Toggle the diagnostic LED. */
extern inline void wpc_led_toggle (void)
{
}


/********************************************/
/* Printer / Parallel Port                  */
/********************************************/


/** Writes a single byte to the parallel port.  The data
 * is first latched into the data register, then the
 * strobe line is brought low and then released. */
extern inline void wpc_parport_write (U8 data)
{
}

/********************************************/
/* RAM Protection Circuit                   */
/********************************************/

/** Write to the WPC's RAM protect register */
extern inline void wpc_set_ram_protect (U8 prot)
{
}


/** Write to the WPC's RAM protect size register */
extern inline void wpc_set_ram_protect_size (U8 sz)
{
}


/** Acquire write access to the NVRAM */
#define wpc_nvram_get()

/** Release write access to the NVRAM */
#define wpc_nvram_put()


/** Atomically increment a variable in NVRAM by N. */
#define wpc_nvram_add(var,n) \
	do { \
		volatile typeof(var) *pvar = &var; \
		wpc_nvram_get (); \
		*pvar += n; \
		wpc_nvram_put (); \
	} while (0)


/** Atomically decrement a variable in NVRAM by N. */
#define wpc_nvram_subtract(var,n) \
	do { \
		volatile typeof(var) *pvar = &var; \
		wpc_nvram_get (); \
		*pvar -= n; \
		wpc_nvram_put (); \
	} while (0)

/********************************************/
/* ROM Paging                               */
/********************************************/

extern inline U8 wpc_get_rom_page (void)
{
	return wpc_asic_read (WS_ROM_PAGE);
}

extern inline void wpc_set_rom_page (U8 page)
{
	wpc_asic_write (WS_ROM_PAGE, page);
}


/** The call_far, wpc_push_page, and wpc_pop_page
 * macros are only safe when calling from the system
 * page, so don't define them otherwise. */
#if (PAGE == SYS_PAGE) || !defined(HAVE_PAGING)

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

#endif /* PAGE == SYS_PAGE */

/********************************************/
/* RAM Paging                               */
/********************************************/


/********************************************/
/* Zero Crossing/IRQ Clear Register         */
/********************************************/


extern inline void wpc_write_misc_control (U8 val)
{
}

extern inline void wpc_int_enable (void)
{
}

extern inline void wpc_int_clear (void)
{
}


extern inline U8 wpc_read_ac_zerocross (void)
{
}


/***************************************************************
 * Flippers
 ***************************************************************/

extern inline U8 wpc_read_flippers (void)
{
}


extern inline U8 wpc_read_flipper_buttons (void)
{
}


extern inline U8 wpc_read_flipper_eos (void)
{
}


extern inline void wpc_write_flippers (U8 val)
{
}


/********************************************/
/* Jumpers                                  */
/********************************************/


extern inline U8 wpc_get_jumpers (void)
{
}

extern inline U8 wpc_read_locale (void)
{
}


extern inline U8 wpc_read_ticket (void)
{
}


extern inline void wpc_write_ticket (U8 val)
{
}


#endif /* _WHITESTAR_H */

