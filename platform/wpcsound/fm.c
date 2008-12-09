/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>


/**
 * Writes to a FM-chip register.
 */
__attribute__((noinline)) void fm_write (U8 addr, U8 val)
{
	fm_write_inline (addr, val, 0);
}


/**
 * Reads from a FM-chip register.
 */
__attribute__((noinline)) U8 fm_read (U8 addr)
{
	return fm_read_inline (addr, 0);
}


void fm_init (void)
{
	U8 reg;

	/* Initialize the FM chip.
	 * Use the 'in_interrupt' version, since interrupts are now
	 * disabled. */
	for (reg=0; reg <= 0xFE; reg++)
		fm_write_inline (reg, 0, 1);
	fm_write_inline (0xFF, 0, 1);

	fm_write_inline (FM_ADDR_CLOCK_CTRL, FM_TIMER_FRESETA + FM_TIMER_FRESETB, 1);
	fm_write_inline (FM_ADDR_CLOCK_A1, 0xFD, 1);
	fm_write_inline (FM_ADDR_CLOCK_A2, 0x02, 1);
	fm_timer_restart (1);
}

void fm_test5500 (void)
{
	fm_timera_config (5500);
}

void fm_test11000 (void)
{
	fm_timera_config (11000);
}

