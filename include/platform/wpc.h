/*
 * Copyright 2006-2012 by Brian Dominy <brian@oddchange.com>
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
 * \brief Definitions/macros specific to the WPC hardware
 */

#ifndef _WPC_H
#define _WPC_H


/***************************************************************
 * Peripheral timer
 ***************************************************************/

/** The FIRQ clear/peripheral timer register bits */
#define WPC_FIRQ_CLEAR_BIT 0x80


/***************************************************************
 * Memory usage
 ***************************************************************/

#ifdef __m6809__

/** AREA_DECL is used to expose a linker area name within the C
 * variable namespace.  It appears an external name.  The asm syntax
 * is needed so that the normal appending of an underscore does not
 * occur. */
#define ASM_DECL(name) name asm (#name)
#define AREA_DECL(name) extern U8 ASM_DECL (s_ ## name); extern U8 ASM_DECL (l_ ## name);

/** Return the base address of a linker area.  This has type (U8 *). */
#define AREA_BASE(name) (&s_ ## name)

/** Return the runtime size of a linker area.  This has type U16.
 * This is not the maximum allowable space for the area, but rather
 * reflects how many actual variables have been mapped there. */
#define AREA_SIZE(name) ((U16)(&l_ ## name))

/* Define externs for all of these areas.  AREA_BASE and AREA_SIZE can
 * only be called on these. */
AREA_DECL(direct)
AREA_DECL(ram)
AREA_DECL(local)
AREA_DECL(heap)
AREA_DECL(stack)
AREA_DECL(permanent)
AREA_DECL(nvram)

#else
#define AREA_SIZE(name) 0
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

/** The address of the current player's local variables. */
#define LOCAL_BASE		AREA_BASE(local)

/** The maximum alloted size of each player's local variables.
 * 5x this amount is allocated, one for the "live" variables and
 * 4 for the player save buffers. */
#define LOCAL_SIZE		0x30U

/** Returns the address of player P's save buffer. */
#define LOCAL_SAVE_BASE(p)	(LOCAL_BASE + (LOCAL_SIZE * (p)))


/**
 * New machine type flags
 *
 * The WPC_CAP_xxx flags indicate specific capabilities of the hardware.
 * The WPC_GEN_xxx values give the sets of capabilities that each
 * generation of the hardware supported.
 *
 * WPC_TYPE is set to the correct WPC_GEN_xxx value, when building for
 * a fixed system.  The MACHINE_WPC_GENERIC flag will build code
 * (theoretically!) for all generations of hardware, using runtime
 * checks to determine the actual system type.
 */
#define WPC_CAP_FLIPTRONIC      0x1
#define WPC_CAP_DMD             0x2
#define WPC_CAP_DCS             0x4
#define WPC_CAP_PIC             0x8
#define WPC_CAP_95              0x10
#define WPC_CAP_EXTIO           0x20
#define WPC_CAP_KNOWN           0

#define WPC_GEN_ALPHA           (WPC_CAP_KNOWN)
#define WPC_GEN_DMD             (WPC_GEN_ALPHA | WPC_CAP_DMD)
#define WPC_GEN_FLIPTRONIC      (WPC_GEN_DMD | WPC_CAP_FLIPTRONIC)
#define WPC_GEN_DCS             (WPC_GEN_FLIPTRONIC | WPC_CAP_DCS)
#define WPC_GEN_PIC             (WPC_GEN_DCS | WPC_CAP_PIC)
#define WPC_GEN_95              (WPC_GEN_PIC | WPC_CAP_95)

#if (MACHINE_WPC_GENERIC == 1)
#define WPC_TYPE 0
#elif (MACHINE_WPC95 == 1)
#define WPC_TYPE WPC_GEN_95
#elif (MACHINE_PIC == 1)
#define WPC_TYPE WPC_GEN_PIC
#elif (MACHINE_DCS == 1)
#define WPC_TYPE WPC_GEN_DCS
#elif (MACHINE_FLIPTRONIC == 1)
#define WPC_TYPE WPC_GEN_FLIPTRONIC
#elif (MACHINE_DMD == 1)
#define WPC_TYPE WPC_GEN_DMD
#else
#define WPC_TYPE WPC_GEN_ALPHA
#endif

/* The extra capabilities defined by the machine itself */
#ifndef WPC_CAP_MACHINE
#define WPC_CAP_MACHINE 0
#endif

/** Test whether a certain capability exists on the target
 * hardware. */
#define WPC_HAS_CAP(cap)  ((WPC_TYPE | WPC_CAP_MACHINE) & (WPC_CAP_KNOWN | (cap)))


/* For sanity testing */
#if (MACHINE_WPC95 == 1) && !WPC_HAS_CAP(WPC_CAP_95)
#error "WPC95 capability broken"
#endif
#if (MACHINE_PIC == 1) && !WPC_HAS_CAP(WPC_CAP_PIC)
#error "PIC capability broken"
#endif
#if (MACHINE_DCS == 1) && !WPC_HAS_CAP(WPC_CAP_DCS)
#error "DCS capability broken"
#endif
#if (MACHINE_FLIPTRONIC == 1) && !WPC_HAS_CAP(WPC_CAP_FLIPTRONIC)
#error "Fliptronic capability broken"
#endif
#if (MACHINE_DMD == 1) && !WPC_HAS_CAP(WPC_CAP_DMD)
#error "DMD capability broken"
#endif



/***************************************************************
 * ASIC memory map
 ***************************************************************/

#include <platform/wpc-mmap.h>


/***************************************************************
 * Dot matrix display
 ***************************************************************/

/* Define the dimensions of the display */
#define PINIO_DMD_WIDTH 128
#define PINIO_DMD_HEIGHT 32

/* Define the available WINDOWS.  Each window is a memory
	area within the CPU's address space which can be written
	directly, without using writeb(), that addresses a display
	page.

	At least two windows must be provided, named WINDOW_0
	and WINDOW_1; these are the primary windows used for
	all display effects.  Other windows may be used if
	available by platform-specific code. */
#if (MACHINE_WPC95 == 1)
#define PINIO_DMD_WINDOW_AUX0 0x0
#define PINIO_DMD_WINDOW_AUX1 0x1
#define PINIO_DMD_WINDOW_AUX2 0x2
#define PINIO_DMD_WINDOW_AUX3 0x3
#endif
#define PINIO_DMD_WINDOW_0 0x4
#define PINIO_DMD_WINDOW_1 0x5

/* Define the number of display PAGES available.  Each page
	represents a buffer large enough to hold the contents of
	an entire display frame.   Pages are numbered from 0 to
	PINIO_NUM_DMD_PAGES-1. */
#define PINIO_NUM_DMD_PAGES 16


/** The type of a page number */
typedef U8 dmd_pagenum_t;

/** The type of a page pair, which tracks the two pages
needed to display a 4-color image.  Normally, the
dark and bright page numbers are consecutive, but for
mono images, they could be the same value. */
typedef union
{
	struct {
		dmd_pagenum_t first;
		dmd_pagenum_t second;
	} u;
	U16 pair;
} dmd_pagepair_t;


extern dmd_pagepair_t dmd_mapped_pages;

/**
 * Set the page that is visible in the given window.
 */
extern inline void pinio_dmd_window_set (U8 window, U8 page)
{
	switch (window)
	{
		case PINIO_DMD_WINDOW_0:
			writeb (WPC_DMD_LOW_PAGE, dmd_mapped_pages.u.first = page);
			break;

		case PINIO_DMD_WINDOW_1:
			writeb (WPC_DMD_HIGH_PAGE, dmd_mapped_pages.u.second = page);
			break;
	}
}


/**
 * Get the page that is visible in the given window.
 */
extern inline U8 pinio_dmd_window_get (U8 window)
{
	switch (window)
	{
		case PINIO_DMD_WINDOW_0:
			return dmd_mapped_pages.u.first;

		case PINIO_DMD_WINDOW_1:
			return dmd_mapped_pages.u.second;

		default:
			return 0;
	}
}


/**
 * Flip the mapping of windows 0 and 1.
 * This is an optimized swap function using 6809 assembly.
 */
#ifdef __m6809__
#define HAVE_PINIO_DMD_WINDOW_FLIP
extern inline void pinio_dmd_window_flip (void)
{
	dmd_mapped_pages.pair = __bswap16 (dmd_mapped_pages.pair);
	asm ("sta\t%0" :: "m"(*(U8 *)WPC_DMD_LOW_PAGE));
	asm ("stb\t%0" :: "m"(*(U8 *)WPC_DMD_HIGH_PAGE));
}
#endif


/**
 * Get a pointer to the first byte of the named display window.
 */
#ifdef CONFIG_NATIVE
/* In native mode, the DMD is emulated using ordinary character
   buffers. */
extern U8 *pinio_dmd_low_page;
extern U8 *pinio_dmd_high_page;
#define pinio_dmd_window_ptr(w) \
	((w == PINIO_DMD_WINDOW_0) ? pinio_dmd_low_page : pinio_dmd_high_page)
#else
/* WPC can map up to 2 of the DMD pages into address space at
 * 0x3800 and 0x3A00.  Additionally, on WPC-95, 4 more pages
 * can be mapped at 0x3000, 0x3200, 0x3400, and 0x3600.
 * We refer to these areas as map 0 through map 5.
 *
 * FreeWPC only uses maps 4 and 5, as they work on all platforms.
 * We call these "low" and "high".
 */
#define pinio_dmd_window_ptr(w) ((U8 *)0x3000 + ((w) * 0x200))
#endif


/**
 * Set the given page as visible.  It may or may not be
 * mapped to one of the windows.
 */
extern inline void pinio_dmd_set_visible (U8 page)
{
	writeb (WPC_DMD_ACTIVE_PAGE, page);
}


/**
 * Request an interrupt when the display is updated.
 */
extern inline void pinio_dmd_request_interrupt (void)
{
	writeb (WPC_DMD_FIRQ_ROW_VALUE, 30);
}



/********************************************/
/* Diagnostic LED                           */
/********************************************/

#define MACHINE_DIAG_LED 0

#define WPC_LED_DIAGNOSTIC		0x80

/** Toggle the diagnostic LED. */
extern inline void pinio_active_led_toggle (void)
{
	extern U8 wpc_led_reg;
	wpc_led_reg ^= WPC_LED_DIAGNOSTIC;
	writeb (WPC_LEDS, wpc_led_reg);
}


/********************************************/
/* Printer / Parallel Port                  */
/********************************************/

#define HAVE_PARALLEL_PORT

/** Writes a single byte to the parallel port.  The data
 * is first latched into the data register, then the
 * strobe line is brought low and then released. */
extern inline void pinio_parport_write (U8 data)
{
	writeb (WPC_PARALLEL_DATA_PORT, data);
	writeb (WPC_PARALLEL_STROBE_PORT, 0x0);
	noop ();
	noop ();
	noop ();
	noop ();
	noop ();
	noop ();
	noop ();
	noop ();
	writeb (WPC_PARALLEL_STROBE_PORT, 0x1);
}

/********************************************/
/* RAM Protection Circuit                   */
/********************************************/

/** When the lock register(s) contain these values, the upper
 * N bytes of the RAM are write protected. */
#define RAM_LOCK_4K        0x0 /* up to 0x1000 is unprotected */
#define RAM_LOCK_2K        0x1 /* up to 0x1800 is unprotected */
#define RAM_LOCK_1K        0x3 /* up to 0x1C00 is unprotected */
#define RAM_LOCK_512       0x7 /* up to 0x1E00 is unprotected */
#define RAM_LOCK_256       0xF /* up to 0x1F00 is unprotected */
#define RAM_LOCK_NONE      0x10 /* up to 0x2000 is unprotected */

/** Another set of defines for the same purpose, but expressed
in terms of the base address of the protected memory area. */
#define PROT_BASE_0x1000   RAM_LOCK_4K
#define PROT_BASE_0x1800   RAM_LOCK_2K
#define PROT_BASE_0x1C00   RAM_LOCK_1K
#define PROT_BASE_0x1E00   RAM_LOCK_512
#define PROT_BASE_0x1F00   RAM_LOCK_256
#define PROT_BASE_NONE     RAM_LOCK_NONE

/** When the lock register contains this value, the memory
 * protection circuit is enabled, and the protected
 * region cannot be written. */
#define RAM_LOCKED			0x0

/** When the lock register contains this value, the memory
 * protection circuit is disabled */
#define RAM_UNLOCKED			0xB4


/** Write to the WPC's RAM protect register */
extern inline void wpc_set_ram_protect (U8 prot)
{
	writeb (WPC_RAM_LOCK, prot);
}


/** Write to the WPC's RAM protect size register */
extern inline void wpc_set_ram_protect_size (U8 sz)
{
	writeb (WPC_RAM_LOCKSIZE, sz);
}


/** Acquire write access to the protected memory */
#define pinio_nvram_unlock() wpc_set_ram_protect(RAM_UNLOCKED)

/** Release write access to the protected memory */
#define pinio_nvram_lock() wpc_set_ram_protect(RAM_LOCKED)


/********************************************/
/* Bank Switching                           */
/********************************************/

#define PINIO_BANK_ROM 0
#define PINIO_BANK_RAM 1

extern __fastram__ U8 wpc_rom_bank;

extern inline void pinio_set_bank (U8 bankno, U8 val)
{
	switch (bankno)
	{
		case PINIO_BANK_ROM:
			wpc_rom_bank = val;
			writeb (WPC_ROM_BANK, val);
			break;
		case PINIO_BANK_RAM:
			writeb (WPC_RAM_BANK, val);
			break;
		default:
			break;
	}
}

extern inline U8 pinio_get_bank (U8 bankno)
{
	switch (bankno)
	{
		case PINIO_BANK_ROM:
			return wpc_rom_bank;
		case PINIO_BANK_RAM:
			return readb (WPC_RAM_BANK);
		default:
			return 0;
	}
}

/********************************************/
/* Zero Crossing/IRQ Clear Register         */
/********************************************/

/* 0x4 | 0x2 are always set when writing this register.
 * One of these is probably to reset the blanking circuit.
 * 0x80 and 0x10 are also set when clearing the IRQ from the
 * IRQ handler.  These are probably interrupt enable/status
 * lines.
 */

#define WPC_CTRL_BLANK_RESET    0x2
#define WPC_CTRL_WATCHDOG_RESET 0x4
#define WPC_CTRL_IRQ_ENABLE     0x10
#define WPC_CTRL_IRQ_CLEAR      0x80

extern inline void wpc_write_misc_control (U8 val)
{
	writeb (WPC_ZEROCROSS_IRQ_CLEAR,
		WPC_CTRL_BLANK_RESET | WPC_CTRL_WATCHDOG_RESET | val);
}

extern inline void pinio_watchdog_reset (void)
{
	wpc_write_misc_control (0);
}

extern inline void pinio_clear_periodic (void)
{
	wpc_write_misc_control (WPC_CTRL_IRQ_ENABLE | WPC_CTRL_IRQ_CLEAR);
}


#define WPC_ZC_CLEAR 0x0
#define WPC_ZC_SET   0x80

extern inline U8 pinio_read_ac_zerocross (void)
{
#ifdef CONFIG_NO_ZEROCROSS
	return WPC_ZC_CLEAR;
#else
	U8 val = readb (WPC_ZEROCROSS_IRQ_CLEAR);
	return (val & WPC_ZC_SET);
#endif
}


/***************************************************************
 * Power driver board controls
 ***************************************************************/

/* U1 = Triacs, coin interlock, and flipper relay */

#define U1_TRIAC0  0x1
#define U1_TRIAC1  0x2
#define U1_TRIAC2  0x4
#if (MACHINE_WPC95 == 0)
#define U1_TRIAC3  0x8
#define U1_TRIAC4  0x10
#endif
#define U1_COIN_LOCK 0x20
#define U1_FLIPPER_RELAY 0x80

#define TRIAC_GI_STRING(n)			(1 << (n))
#define PINIO_GI_STRINGS \
	(TRIAC_GI_STRING(0) | TRIAC_GI_STRING(1) | TRIAC_GI_STRING(2) | \
	TRIAC_GI_STRING(3) | TRIAC_GI_STRING(4))

extern U8 triac_io_cache;

extern inline U8 wpc_u1_read (void)
{
	return triac_io_cache;
}

extern inline void wpc_u1_write (U8 val)
{
	triac_io_cache = val;
	writeb (WPC_GI_TRIAC, val);
}

extern inline void wpc_u1_set (U8 bits)
{
	wpc_u1_write (wpc_u1_read() | bits);
}

extern inline void wpc_u1_clear (U8 bits)
{
	wpc_u1_write (wpc_u1_read() & ~bits);
}


/***************************************************************
 * Fliptronic Flippers
 ***************************************************************/

#define WPC_LR_FLIP_EOS		0x1
#define WPC_LR_FLIP_SW		0x2
#define WPC_LL_FLIP_EOS		0x4
#define WPC_LL_FLIP_SW		0x8
#define WPC_UR_FLIP_EOS		0x10
#define WPC_UR_FLIP_SW		0x20
#define WPC_UL_FLIP_EOS		0x40
#define WPC_UL_FLIP_SW		0x80
#define WPC_FLIP_EOS \
	(WPC_LR_FLIP_EOS+WPC_LL_FLIP_EOS+WPC_UR_FLIP_EOS+WPC_UL_FLIP_EOS)
#define WPC_FLIP_SW \
	(WPC_LR_FLIP_SW+WPC_LL_FLIP_SW+WPC_UR_FLIP_SW+WPC_UL_FLIP_SW)

extern inline U8 wpc_read_flippers (void)
{
#if (MACHINE_WPC95 == 1)
	return readb (WPC95_FLIPPER_SWITCH_INPUT);
#else
	return readb (WPC_FLIPTRONIC_PORT_A);
#endif
}


#define WPC_LR_FLIP_POWER	0x1
#define WPC_LR_FLIP_HOLD	0x2
#define WPC_LL_FLIP_POWER	0x4
#define WPC_LL_FLIP_HOLD	0x8
#define WPC_UR_FLIP_POWER	0x10
#define WPC_UR_FLIP_HOLD	0x20
#define WPC_UL_FLIP_POWER	0x40
#define WPC_UL_FLIP_HOLD	0x80

extern inline void wpc_write_flippers (U8 val)
{
#if (MACHINE_WPC95 == 1)
	writeb (WPC95_FLIPPER_COIL_OUTPUT, val);
#else
	writeb (WPC_FLIPTRONIC_PORT_A, ~val);
#endif
}


/*
 * Pre-Fliptronic boards have a relay on the power
 * driver board that can enable/disable flipper power.
 * The relay is controlled by writing to the same latch
 * that controls the triacs.
 */

extern inline void pinio_enable_flippers (void)
{
#if (MACHINE_FLIPTRONIC == 0)
	wpc_u1_set (U1_FLIPPER_RELAY);
#endif
}

extern inline void pinio_disable_flippers (void)
{
#if (MACHINE_FLIPTRONIC == 0)
	wpc_u1_clear (U1_FLIPPER_RELAY);
#endif
}


/********************************************/
/* Locale                                   */
/********************************************/

extern inline U8 wpc_get_jumpers (void)
{
	return readb (WPC_SW_JUMPER_INPUT);
}

extern inline U8 pinio_read_locale (void)
{
	return (wpc_get_jumpers () & 0x3C) >> 2;
}


/* Read the current ticket switches. */
extern inline U8 pinio_read_ticket (void)
{
	/* Reading back a value of 0xFF indicates that
	 * the ticket board is not present.  Otherwise,
	 * it reads back a set of switch readings from
	 * the board. */
	return readb (WPC_TICKET_DISPENSE);
}


/* Write the ticket output drivers. */
extern inline void pinio_write_ticket (U8 val)
{
	writeb (WPC_TICKET_DISPENSE, val);
}



/********************************************/
/* WPC Security PIC Chip                    */
/********************************************/

/* The PIC Security Chip can be read/written one byte at a time.
 * It works much like the sound board, in that the first byte
 * sent indicates a 'command', with optional data bytes to
 * follow.  After a command that is intended to return a value,
 * the result can be obtained by doing a PIC read. */

/** The command to reset the PIC */
#define WPC_PIC_RESET       0x0

/** The command to update the PIC counter.  When this counter
 * reaches zero, the switch matrix cannot be accessed until
 * an unlock sequence is successfully issued.   The counter
 * resets automatically after unlocking. */
#define WPC_PIC_COUNTER     0x0D

/** The command to read the xth switch column */
#define WPC_PIC_COLUMN(x)   (0x16 + (x))

/** The command to unlock the switch matrix.  A three-byte
 * sequence follows. */
#define WPC_PIC_UNLOCK      0x20

/** The command to read the xth byte of the serial number.
 * See pic.c for a full description of how this value is
 * encoded/decoded. */
#define WPC_PIC_SERIAL(x)   (0x70 + (x))

extern inline void wpc_write_pic (U8 val)
{
	writeb (WPCS_PIC_WRITE, val);
}

extern inline U8 wpc_read_pic (void)
{
	return readb (WPCS_PIC_READ);
}


/********************************************/
/* Lamps                                    */
/********************************************/

#define PINIO_NUM_LAMPS 64

extern inline void pinio_write_lamp_strobe (U8 val)
{
	writeb (WPC_LAMP_COL_STROBE, val);
}

extern inline void pinio_write_lamp_data (U8 val)
{
	writeb (WPC_LAMP_ROW_OUTPUT, val);
}

/********************************************/
/* General Illumination via Triacs          */
/********************************************/

extern inline U8 pinio_read_triac (void)
{
	return wpc_u1_read () & PINIO_GI_STRINGS;
}

extern inline void pinio_write_triac (U8 val)
{
	wpc_u1_write (val | (wpc_u1_read () & ~PINIO_GI_STRINGS));
}

extern inline void pinio_write_gi (U8 val)
{
	pinio_write_triac (val);
}

/********************************************/
/* Solenoids                                */
/********************************************/

#define PINIO_NUM_SOLS 48
#define SOL_BASE_HIGH 0
#define SOL_BASE_LOW 8
#define SOL_BASE_GENERAL 16
#define SOL_BASE_AUXILIARY 24
#define SOL_BASE_FLIPTRONIC 32
#define SOL_BASE_EXTENDED 40
#define SOL_MIN_FLASHER 16

extern inline void pinio_write_solenoid_set (U8 set, U8 val)
{
	switch (set)
	{
	case 0:
		writeb (WPC_SOL_HIGHPOWER_OUTPUT, val);
		break;
	case 1:
		writeb (WPC_SOL_LOWPOWER_OUTPUT, val);
		break;
	case 2:
		writeb (WPC_SOL_FLASHER_OUTPUT, val);
		break;
	case 3:
		writeb (WPC_SOL_GEN_OUTPUT, val);
		break;
	case 4:
		if (WPC_HAS_CAP (WPC_CAP_FLIPTRONIC))
			wpc_write_flippers (val);
		break;
#ifdef MACHINE_SOL_EXTBOARD1
#if (CONFIG_WPC95 == 1)
#error "WPC95 machines cannot use EXTBOARD1"
#endif
	case 5:
		writeb (WPC_EXTBOARD1, val);
#endif
		break;
	}
}


/** Return the hardware register that can be written
to enable/disable a coil driver. */
extern inline IOPTR sol_get_write_reg (U8 sol)
{
	switch (sol / 8)
	{
		case 0:
			return (IOPTR)WPC_SOL_HIGHPOWER_OUTPUT;
		case 1:
			return (IOPTR)WPC_SOL_LOWPOWER_OUTPUT;
		case 2:
			return (IOPTR)WPC_SOL_FLASHER_OUTPUT;
		case 3:
			return (IOPTR)WPC_SOL_GEN_OUTPUT;
#if (MACHINE_WPC95 == 1)
		case 4:
			return (IOPTR)WPC95_FLIPPER_COIL_OUTPUT;
#elif (MACHINE_FLIPTRONIC == 1)
		case 4:
			return (IOPTR)WPC_FLIPTRONIC_PORT_A;
#endif
#ifdef MACHINE_SOL_EXTBOARD1
		case 5:
			return (IOPTR)WPC_EXTBOARD1;
#endif
	}
	return 0;
}


/** Return nonzero if a solenoid's enable line is inverted; i.e.
 * writing a 0 turns it on and writing a 1 turns it off.
 */
#if (MACHINE_WPC95 == 0)
#define PINIO_SOL_INVERTED(sol) \
	(((sol) >= SOL_BASE_FLIPTRONIC) && ((sol) < (SOL_BASE_FLIPTRONIC+8)))
#endif


/********************************************/
/* Sound                                    */
/********************************************/

/** This bit is set in the sound status register when
 * there is a byte to be read by the CPU. */
#if (MACHINE_DCS == 0)
#define WPCS_READ_READY	0x01
#else
#define WPCS_READ_READY	0x80
#endif

extern inline void pinio_reset_sound (void)
{
#ifdef MACHINE_SYS11_SOUND
	writeb (WPCS11_RESET, 0);
#else
	writeb (WPCS_CONTROL_STATUS, 0);
#endif
}

extern inline void pinio_write_sound (U8 val)
{
#ifdef MACHINE_SYS11_SOUND
	writeb (WPCS11_DATA_OUT, val);
#else
	writeb (WPCS_DATA, val);
#endif
}

extern inline bool pinio_sound_ready_p (void)
{
#ifdef MACHINE_SYS11_SOUND
	return readb (WPCS11_READY_IN);
#else
	return readb (WPCS_CONTROL_STATUS) & WPCS_READ_READY;
#endif
}

extern inline U8 pinio_read_sound (void)
{
#ifdef MACHINE_SYS11_SOUND
	return readb (WPCS11_DATA_IN);
#else
	return readb (WPCS_DATA);
#endif
}

#define SW_VOLUME_UP SW_UP
#define SW_VOLUME_DOWN SW_DOWN

/********************************************/
/* Switches                                 */
/********************************************/

#if (MACHINE_FLIPTRONIC == 1)
#define PINIO_NUM_SWITCHES 80
#else
#define PINIO_NUM_SWITCHES 72
#endif

extern inline void pinio_write_switch_column (U8 val)
{
	if (WPC_HAS_CAP (WPC_CAP_PIC))
		wpc_write_pic (WPC_PIC_COLUMN (val));
	else
		writeb (WPC_SW_COL_STROBE, 1 << val);
}

extern inline U8 pinio_read_switch_rows (void)
{
	if (WPC_HAS_CAP (WPC_CAP_PIC))
		return wpc_read_pic ();
	else
		return readb (WPC_SW_ROW_INPUT);
}

extern inline U8 pinio_read_dedicated_switches (void)
{
	return readb (WPC_SW_CABINET_INPUT);
}


/********************************************/
/* Precision Timer                          */
/********************************************/

extern inline U8 pinio_read_timer (U8 timerno)
{
	return readb (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR);
}

extern inline void pinio_write_timer (U8 timerno, U8 val)
{
	writeb (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR, val);
}


/********************************************/
/* Debugging                                */
/********************************************/

#define PINIO_HAVE_DEBUG_PORT

#define WPC_DEBUG_WRITE_READY 0x1
#define WPC_DEBUG_READ_READY 0x2

extern inline U8 wpc_debug_get_status (void)
{
	return readb (WPC_DEBUG_CONTROL_PORT);
}

extern inline bool pinio_debug_write_ready (void)
{
	return wpc_debug_get_status () & WPC_DEBUG_WRITE_READY;
}

extern inline bool pinio_debug_read_ready (void)
{
	return wpc_debug_get_status () & WPC_DEBUG_READ_READY;
}

extern inline void pinio_debug_write (U8 data)
{
	writeb (WPC_DEBUG_DATA_PORT, data);
}

extern inline U8 pinio_debug_read (void)
{
	return readb (WPC_DEBUG_DATA_PORT);
}

#endif /* _WPC_H */

