/*
 * Copyright 2010-2012 by Brian Dominy <brian@oddchange.com>
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
#include "native/log.h"

U8 p2k_write_cache[0x20];

/**
 * readb/writeb talk to the parallel port driver.
 */
#ifndef CONFIG_SIM
void writeb (IOPTR addr, U8 val)
{
	print_log ("writeb(%X,%02X)\n", addr, val);
}

U8 readb (IOPTR addr)
{
	print_log ("readb(%X)\n", addr);
	return 0;
}
#endif


/**
 * Write to a P2K output register.
 */
void p2k_write (U8 reg, U8 val)
{
	print_log ("p2k_write: [%d] <- %02X\n", reg, val);
	p2k_write_cache[reg] = val;
	writeb (LPT_DATA, reg);
	writeb (LPT_CONTROL, LPT_REG_LATCH);
	writeb (LPT_CONTROL, 0);
	writeb (LPT_DATA, val);
	writeb (LPT_CONTROL, LPT_REG_OE);
	writeb (LPT_CONTROL, 0);
}


/**
 * Read from a P2K input register.
 */
U8 p2k_read (U8 reg)
{
	U8 val;
	writeb (LPT_DATA, reg);
	writeb (LPT_CONTROL, LPT_REG_LATCH);
	writeb (LPT_CONTROL, 0);
	writeb (LPT_CONTROL, 0x29); /* what are these bits? */
	val = readb (LPT_DATA);
	writeb (LPT_CONTROL, 0);
	print_log ("p2k_read: [%d] -> %02X\n", reg, val);
	return val;
}

/* RTT(name=switch_rtt freq=2) */
void switch_rtt (void)
{
}

/* RTT(name=lamp_rtt freq=20) */
void lamp_rtt (void)
{
}

/* RTT(name=sol_update_rtt_0 freq=1) */
void sol_update_rtt_0 (void)
{
}

void sol_update_rtt_1 (void)
{
}


void platform_init (void)
{
}


