/*
 * Copyright 2009-2012 by Brian Dominy <brian@oddchange.com>
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
 * \brief Definitions/macros specific to the P-ROC hardware
 */

#ifndef __PLATFORM_PROC_H
#define __PLATFORM_PROC_H

/* A hack -- code should not be calling this unless CONFIG_WPC */
#define WPC_HAS_CAP(cap) 0

/* A 32-bit request word is divided into four parts:
 * Command (READ or WRITE)   [31]
 * Number of words           [30:20]
 * Module select             [19:16]
 * Address                   [15:0]
 */

#define PACKET_HEADER_LEN(header)         ((header >> 20) & 0x3FF)

#define PROC_CMD(rw, count, module, addr) \
	(((rw) << 31) | ((count) << 20) | ((module) << 16) | (addr))

/* TODO */
#define SWAP32(n) (n)

/* Module selects */
#define PROC_MOD_MANAGER    0
	#define PROC_MANAGER_CHIPID    0
	#define PROC_MANAGER_VERSION   1
	#define PROC_MANAGER_WATCHDOG  2
	#define PROC_MANAGER_DIPSW     3

#define PROC_MOD_BUSMASTER  1

#define PROC_MOD_SWITCH     2
	#define PROC_SW_BURSTCFG       0
	#define PROC_SW_DIRECT_PHYS    4
	#define PROC_SW_MATRIX_PHYS(n) (5 + n)
	#define PROC_SW_BURST_PHYS(n)  (0x9 + n)
	#define PROC_SW_DIRECT         0xB
	#define PROC_SW_MATRIX(n)      (0xC + n)
	#define PROC_SW_BURST(n)       (0x10 + n)

#define PROC_MOD_OUTPUT     3

#define PROC_MOD_CHANGE     4

#define PROC_MOD_DMD        5
	#define PROC_DMD_DRVCFG        0
	#define PROC_DMD_FRAMECFG(n)   (0x8 + (n))
	#define PROC_DMD_MEM           0x1000

#define PROC_MOD_UNASSOC    15

#define PROC_ADDR(module, offset)   (offset | (module << 16))


/***************************************************************
 * Memory usage
 ***************************************************************/

/** The total size of RAM  -- 8K */
#define RAM_SIZE 			0x2000UL

/** The usable, nonprotected area of RAM -- the first 6K */
#define USER_RAM_SIZE	0x1800UL

#define LOCAL_SIZE      0x40

/*
 * Pinball I/O (pinio) functions
 */

extern U8 *pinio_dmd_low_page, *pinio_dmd_high_page;

#define MACHINE_DIAG_LED 0

extern inline void pinio_active_led_toggle (void)
{
}

extern inline U8 wpc_read_ac_zerocross (void)
{
	return 0;
}

/********************************************/
/* Lamps                                    */
/********************************************/

extern inline void pinio_write_lamp_strobe (U8 val)
{
}

extern inline void pinio_write_lamp_data (U8 val)
{
}

/********************************************/
/* Solenoids                                */
/********************************************/

extern inline void pinio_write_solenoid_set (U8 set, U8 val)
{
}

/********************************************/
/* Switches                                 */
/********************************************/

extern inline void pinio_write_switch_column (U8 val)
{
}

extern inline U8 pinio_read_switch_rows (void)
{
	return 0;
}

extern inline U8 pinio_read_dedicated_switches (void)
{
	return 0;
}

/***************************************************************
 * Flippers
 ***************************************************************/

extern inline U8 wpc_read_flippers (void)
{
	return 0;
}

extern inline U8 wpc_read_flipper_buttons (void)
{
	return 0;
}

extern inline U8 wpc_read_flipper_eos (void)
{
	return 0;
}


extern inline void wpc_write_flippers (U8 val)
{
}

extern inline void pinio_enable_flippers (void)
{
}

extern inline void pinio_disable_flippers (void)
{
}

extern inline void pinio_watchdog_reset (void)
{
}


/********************************************/
/* Bank Switching                           */
/********************************************/

#define PINIO_BANK_ROM 0

extern inline void pinio_set_bank (U8 bankno, U8 val)
{
}

extern inline U8 pinio_get_bank (U8 bankno)
{
	return 0;
}

/********************************************/
/* Zero Crossing/IRQ Clear Register         */
/********************************************/


extern inline U8 wpc_get_jumpers (void)
{
	return 0;
}

extern inline U8 pinio_read_locale (void)
{
	return 0;
}


/********************************************/
/* Sound                                    */
/********************************************/

extern inline void pinio_reset_sound (void)
{
}

extern inline void pinio_write_sound (U8 val)
{
}

extern inline bool pinio_sound_ready_p (void)
{
	return FALSE;
}

extern inline U8 pinio_read_sound (void)
{
	return 0;
}


/********************************************/
/* Triacs                                   */
/********************************************/

extern inline void pinio_write_triac (U8 val)
{
}

#define pinio_nvram_unlock()
#define pinio_nvram_lock()

#endif /* __PLATFORM_PROC_H */

