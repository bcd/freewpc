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
 * \brief Definitions/macros specific to the WPC hardware
 */

#ifndef _WPC_H
#define _WPC_H


/***************************************************************
 * Peripheral timer
 ***************************************************************/

/** The FIRQ clear/peripheral timer register bits */
#define FIRQ_CLEAR_BIT 0x80


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

/** The address of the current player's local variables. */
#define LOCAL_BASE		AREA_BASE(local)

/** The maximum alloted size of each player's local variables.
 * 5x this amount is allocated, one for the "live" variables and
 * 4 for the player save buffers. */
#define LOCAL_SIZE		0x40U

/** Returns the address of player P's save buffer. */
#define LOCAL_SAVE_BASE(p)	(LOCAL_BASE + (LOCAL_SIZE * (p)))

/***************************************************************
 * ASIC / DMD memory map
 ***************************************************************/

#ifdef CONFIG_NATIVE

/* In native mode, the DMD is emulated using ordinary character
buffers. */
extern U8 *linux_dmd_low_page;
extern U8 *linux_dmd_high_page;
#define DMD_LOW_BASE linux_dmd_low_page
#define DMD_HIGH_BASE linux_dmd_high_page

#else

/* WPC can map up to 2 of the DMD pages into address space at
 * 0x3800 and 0x3A00.  Additionally, on WPC-95, 4 more pages
 * can be mapped at 0x3000, 0x3200, 0x3400, and 0x3600.
 * We refer to these areas as map 0 through map 5.
 *
 * FreeWPC only uses maps 4 and 5, as they work on all platforms.
 * We call these "low" and "high".
 */

#define DMD_MAPPED(n) ((U8 *)0x3000 + ((n) * 0x200))

/* Define addresses for the two page buffer locations we
 * call low and high. */

#define DMD_LOW_BASE 					DMD_MAPPED(4)
#define DMD_HIGH_BASE 					DMD_MAPPED(5)

#endif /* CONFIG_NATIVE */

#define WPC_DEBUG_DATA_PORT			0x3D60
#define WPC_DEBUG_CONTROL_PORT		0x3D61

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

#define WPC_PARALLEL_STATUS_PORT 	0x3FC0
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

#if (MACHINE_PIC == 1)
#define WPCS_PIC_READ 					0x3FE9
#define WPCS_PIC_WRITE 					0x3FEA
#else
#define WPC_SW_ROW_INPUT 				0x3FE9
#define WPC_SW_COL_STROBE 				0x3FEA
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
/* Diagnostic LED                           */
/********************************************/

#define WPC_LED_DIAGNOSTIC		0x80

/** Toggle the diagnostic LED. */
extern inline void wpc_led_toggle (void)
{
	wpc_asic_xor (WPC_LEDS, WPC_LED_DIAGNOSTIC);
}


/********************************************/
/* Printer / Parallel Port                  */
/********************************************/


/** Writes a single byte to the parallel port.  The data
 * is first latched into the data register, then the
 * strobe line is brought low and then released. */
extern inline void wpc_parport_write (U8 data)
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
#define wpc_nvram_get() wpc_set_ram_protect(RAM_UNLOCKED)

/** Release write access to the protected memory */
#define wpc_nvram_put() wpc_set_ram_protect(RAM_LOCKED)


/** Atomically increment a variable in protected memory by N. */
#define wpc_nvram_add(var,n) \
	do { \
		volatile typeof(var) *pvar = &var; \
		wpc_nvram_get (); \
		*pvar += n; \
		wpc_nvram_put (); \
	} while (0)


/** Atomically decrement a variable in protected memory by N. */
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
	return readb (WPC_ROM_BANK);
}

extern inline void wpc_set_rom_page (U8 page)
{
	writeb (WPC_ROM_BANK, page);
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

extern inline U8 wpc_get_ram_page (void)
{
	return readb (WPC_RAM_BANK);
}

extern inline void wpc_set_ram_page (U8 page)
{
	writeb (WPC_RAM_BANK, page);
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

extern inline void wpc_watchdog_reset (void)
{
	wpc_write_misc_control (0);
}

extern inline void wpc_int_clear (void)
{
#ifdef CONFIG_DEBUG_ADJUSTMENTS
	extern U8 irq_ack_value;
	writeb (WPC_ZEROCROSS_IRQ_CLEAR, irq_ack_value);
#else
	wpc_write_misc_control (WPC_CTRL_IRQ_ENABLE | WPC_CTRL_IRQ_CLEAR);
#endif
}


#define WPC_ZC_CLEAR 0x0
#define WPC_ZC_SET   0x80

extern inline U8 wpc_read_ac_zerocross (void)
{
	U8 val = readb (WPC_ZEROCROSS_IRQ_CLEAR);
	return (val & 0x80);
}


/***************************************************************
 * Flippers
 ***************************************************************/

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
#if (MACHINE_WPC95 == 1)
	return readb (WPC95_FLIPPER_SWITCH_INPUT);
#else
	return readb (WPC_FLIPTRONIC_PORT_A);
#endif
}


extern inline U8 wpc_read_flipper_buttons (void)
{
	return wpc_read_flippers () & 0xAA;
}


extern inline U8 wpc_read_flipper_eos (void)
{
	return wpc_read_flippers () & 0x55;
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
#ifndef CONFIG_NO_SOL
#if (MACHINE_WPC95 == 1)
	writeb (WPC95_FLIPPER_COIL_OUTPUT, val);
#else
	writeb (WPC_FLIPTRONIC_PORT_A, ~val);
#endif
#endif
}


/********************************************/
/* Jumpers                                  */
/********************************************/

#define WPC_JUMPER_USA_CANADA 0
#define WPC_JUMPER_FRANCE 1
#define WPC_JUMPER_GERMANY 2
#define WPC_JUMPER_FRANCE2 3
#define WPC_JUMPER_EXPORT_ENGLISH 8
#define WPC_JUMPER_FRANCE3 9
#define WPC_JUMPER_EXPORT 10
#define WPC_JUMPER_FRANCE4 11
#define WPC_JUMPER_UK 12
#define WPC_JUMPER_EUROPE 13
#define WPC_JUMPER_SPAIN 14
#define WPC_JUMPER_USA_CANADA2 15


extern inline U8 wpc_get_jumpers (void)
{
	return readb (WPC_SW_JUMPER_INPUT);
}

extern inline U8 wpc_read_locale (void)
{
	return (wpc_get_jumpers () & 0x3C) >> 2;
}


extern inline U8 wpc_read_ticket (void)
{
	return readb (WPC_TICKET_DISPENSE);
}


extern inline void wpc_write_ticket (U8 val)
{
#ifndef CONFIG_NO_SOL
	writeb (WPC_TICKET_DISPENSE, val);
#endif
}


#if (MACHINE_PIC == 1)
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

#endif

/********************************************/
/* Lamps                                    */
/********************************************/

extern inline void pinio_write_lamp_strobe (U8 val)
{
	writeb (WPC_LAMP_COL_STROBE, val);
}

extern inline void pinio_write_lamp_data (U8 val)
{
	writeb (WPC_LAMP_ROW_OUTPUT, val);
}

/********************************************/
/* Solenoids                                */
/********************************************/

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
		writeb (WPC_SOL_FLASH1_OUTPUT, val);
		break;
	case 3:
		writeb (WPC_SOL_FLASH2_OUTPUT, val);
		break;
	case 4:
		wpc_write_flippers (val);
		break;
	case 5:
#ifdef WPC_EXTBOARD1
		writeb (WPC_EXTBOARD1, val);
#endif
		break;
	}
}

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
	writeb (WPCS_CONTROL_STATUS, 0);
}

extern inline void pinio_write_sound (U8 val)
{
	writeb (WPCS_DATA, val);
}

extern inline bool pinio_sound_ready_p (void)
{
	return readb (WPCS_CONTROL_STATUS) & WPCS_READ_READY;
}

extern inline U8 pinio_read_sound (void)
{
	return readb (WPCS_DATA);
}

#define SW_VOLUME_UP SW_UP
#define SW_VOLUME_DOWN SW_DOWN

/********************************************/
/* Switches                                 */
/********************************************/

extern inline void pinio_write_switch_column (U8 val)
{
#if (MACHINE_PIC == 1)
		wpc_write_pic (WPC_PIC_COLUMN (val));
#else
		writeb (WPC_SW_COL_STROBE, 1 << val);
#endif
}

extern inline U8 pinio_read_switch_rows (void)
{
#if (MACHINE_PIC == 1)
	return wpc_read_pic ();
#else
	return readb (WPC_SW_ROW_INPUT);
#endif
}

extern inline U8 pinio_read_dedicated_switches (void)
{
	return readb (WPC_SW_CABINET_INPUT);
}

/********************************************/
/* Triacs                                   */
/********************************************/

extern inline void pinio_write_triac (U8 val)
{
	writeb (WPC_GI_TRIAC, val);
}


#endif /* _WPC_H */

