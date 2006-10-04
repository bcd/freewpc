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
 * \brief Simulation functions for use in a Linux environment
 */

#include <freewpc.h>


U8 linux_dmd_low_buffer[DMD_PAGE_SIZE];

U8 linux_dmd_high_buffer[DMD_PAGE_SIZE];


void linux_asic_write (U16 addr, U8 val)
{
	switch (addr)
	{
		case WPC_DEBUG_DATA_PORT:
			putchar (val);
			break;
		default:
			printf ("Error: invalid I/O address 0x%04X, val=0x%02X\n", addr, val);
	}
}


U8 linux_asic_read (U16 addr)
{
}


void linux_init (void)
{
}


int main (int argc, char *argv[])
{
	extern __noreturn__ void do_reset (void);
	do_reset ();
	return 0;
}


