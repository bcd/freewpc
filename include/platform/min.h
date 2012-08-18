/*
 * Copyright 2012 by Brian Dominy <brian@oddchange.com>
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

#ifndef __PLATFORM_MIN_H
#define __PLATFORM_MIN_H

/* This is an example of a 'minimal platform' which only provides the
   basic necessities for a pinball game. */

/* Mandatory PINIO defines */
#define PINIO_NUM_LAMPS 24
#define PINIO_NUM_SWITCHES 8
#define PINIO_NUM_SOLS 4
#define PINIO_GI_STRINGS 0x3

/* Internal names for I/O addresses.  These addresses are "virtual".
   Each represents an 8-bit input/output register. */
#define IO_SWITCH 0
#define IO_LAMP 1 /* 8 direct lamps (0-7) */
#define IO_SOL 2
#define IO_DIAG 3
#define IO_LAMP_MX_ROW 4 /* 4x4 lamp matrix (8-23) */
#define IO_LAMP_MX_COL 5

/* TODO : we shouldn't need to declare anything if there is no banking... */

#define PINIO_BANK_ROM 0
#define PINIO_BANK_RAM 1
extern inline void pinio_set_bank (U8 bankno, U8 val) { }
extern inline U8 pinio_get_bank (U8 bankno) { return 0; }

/* TODO : why do I have to say this? */
#define LOCAL_SIZE 64

/* The following two are very similar, and could be merged... */

extern inline void pinio_write_solenoid_set (U8 set, U8 val)
{
	if (set == 0)
		writeb (IO_SOL, val & 0xF);
}

extern inline IOPTR sol_get_write_reg (U8 sol)
{
	return IO_SOL;
}

/* Other stuff */

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

extern inline void pinio_nvram_lock (void)
{
}

extern inline void pinio_nvram_unlock (void)
{
}

extern inline void pinio_enable_flippers (void)
{
}

extern inline void pinio_disable_flippers (void)
{
}

extern inline void pinio_active_led_toggle (void)
{
}

extern inline U8 pinio_read_locale (void)
{
}

#endif /* __PLATFORM_MIN_H */
