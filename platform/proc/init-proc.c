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

#include <freewpc.h>
#include "native/log.h"

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


#ifndef CONFIG_SIM
void writeb (IOPTR addr, U8 val)
{
}

U8 readb (IOPTR addr)
{
	return 0;
}
#endif


/* RTT(name=switch_rtt freq=2) */
void switch_rtt (void)
{
}

/* RTT(name=lamp_rtt freq=16) */
void lamp_rtt (void)
{
	writeb (IO_LAMP, platform_lamp_compute (0));
}

/* RTT(name=sol_update_rtt_0 freq=1) */
void sol_update_rtt_0 (void)
{
	pinio_write_solenoid_set (0, *sol_get_read_reg (0));
}

void sol_update_rtt_1 (void)
{
}

void platform_init (void)
{
	//remote_dmd_init ();
}


