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

/** Normally we don't like to use 'int', but this code interfaces
 * with the standard library, so make absolutely sure we are using
 * the right types */
#undef int

__fastram__ U8 tick_count;


void fatal (U8 error_code)
{
	while (1);
}


void abort (void)
{
	fatal (0xff);
}


void exit (void)
{
	fatal (0);
}


void wpcs_hardware_init (void)
{
	writeb (WPCS_ROM_BANK, 0x7D);
	writeb (WPCS_HOST_OUTPUT, 0);
}


int main (void)
{
	wpcs_hardware_init ();
	VOIDCALL (host_init);
	VOIDCALL (volume_init);
	for (;;)
	{
		writeb (WPCS_DAC, 0x55);
	}
	return 0;
}

