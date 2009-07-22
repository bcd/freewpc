/*
 * Copyright 2008, 2009 by Brian Dominy <brian@oddchange.com>
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


static const char copyright[] = "FreeWPC - WPC Sound System";


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
}


extern U8 bell_data[];
extern U8 *dac_data;
extern U8 dac_flag;


__noreturn__ void main (void)
{
	U16 count;

	wpcs_hardware_init ();
	VOIDCALL (host_init);
	VOIDCALL (volume_init);
	VOIDCALL (fm_init);

	dac_data = bell_data;
	dac_flag = 1;

	/* Wait for the host to be ready. */
	for (count = 0; count < 0xFFF0; count++)
	{
		noop ();
		noop ();
		noop ();
		noop ();
		noop ();
		noop ();
		noop ();
		noop ();
		noop ();
		noop ();
	}

	enable_interrupts ();

	for (;;)
	{
#if 0
		U8 val;
		for (val=0; val<0xff; val++)
		{
			writeb (WPCS_DAC, val);
			noop ();
			noop ();
			noop ();
			noop ();
		}
		for (val=0xff; val != 0; val--)
		{
			writeb (WPCS_DAC, val);
			noop ();
			noop ();
			noop ();
			noop ();
		}
#endif
	}
}

