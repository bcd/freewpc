/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

U8 in_test;

void proc_debug_write (U8 c)
{
	putchar (c);
}

/**
 * Post a switch transition.
 */
void proc_post_switch_transition (switchnum_t swno)
{
	extern __fastram__ switch_bits_t sw_stable;
	bit_toggle (sw_stable, swno);
}


void platform_main_loop (void)
{
	int n;
	extern __fastram__ U16 sys_time;
	for (;;)
	{
		task_sleep (TIME_16MS);
		for (n = 0; n < 16; n++)
		{
			sys_time++;
			if (n & 1)
				VOIDCALL (lamp_rtt);
			tick_driver ();
		}
		db_periodic ();
		if (likely (periodic_ok))
		{
			do_periodic ();
		}
	}
}

void writeb (IOPTR addr, U8 val)
{
}

U8 readb (IOPTR addr)
{
	return 0;
}


void platform_init (void)
{
	in_test = 0;
	remote_dmd_init ();
}


void linux_init (void)
{
	printf ("P-ROC: Initializing\n");
}


void linux_shutdown (U8 error_code)
{
	printf ("P-ROC: Shutdown\n");
	exit (error_code);
}


CALLSET_ENTRY (proc, diagnostic_check)
{
}

CALLSET_ENTRY (proc, init_complete)
{
}

CALLSET_ENTRY (proc, idle)
{
	/* Check for input events from the hardware */
}


int main (int argc, char *argv[])
{
	freewpc_init ();
	return 0;
}

