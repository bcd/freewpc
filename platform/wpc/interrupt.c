/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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
 * do_firq is the entry point from the FIRQ vector.  This interrupt
 * is generated from the WPC ASIC on two different occasions: (1)
 * when the DMD controller has been programmed to generate an
 * interrupt after drawing a particular scan line, and (2) when the
 * WPC's peripheral timer register reaches zero.  The type of interrupt
 * can be determined by reading the peripheral timer register.
 *
 * The peripheral timer is currently unused.  Real WPC games only used
 * it on the alphanumeric machines, supposedly.  PinMAME doesn't handle
 * it correctly, so the check is kept.
 */
__interrupt__
void do_firq (void)
{
	/* The processor does not save/restore all registers on
	entry to an FIRQ, unlike IRQ.  So the handler is responsibly
	for doing this for all registers that might be used which are
	not ordinarily saved automatically.  The 6809 toolchain
	does not save X and D automatically, thus they need to be
	saved explicitly.  (It is possible that an application may
	know definitely that they are *not* used and thus this step
	can be skipped, but FreeWPC does not make any assumptions.) */
#ifdef __m6809__
	m6809_firq_save_regs ();
#endif

	/* Read the peripheral timer register.
	 * If bit 7 is set, it is a timer interrupt.  Otherwise,
	 * it is a DMD interrupt. */
	if (readb (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR) & WPC_FIRQ_CLEAR_BIT)
	{
		/* It is a timer interrupt.
		 * Clear the interrupt by writing back to the same register. */
		writeb (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR, 0);

		/* If we were using the timer, we would process the interrupt
		here... */
		interrupt_dbprintf ("Timer interrupt.\n");
	}
#ifdef CONFIG_DMD
	else if (WPC_HAS_CAP (WPC_CAP_DMD))
	{
		/* It is a DMD interrupt. */
		dmd_rtt ();
	}
#endif

#ifdef __m6809__
	m6809_firq_restore_regs ();
#endif
}


__interrupt__
void do_nmi (void)
{
	fatal (ERR_NMI);
}


__interrupt__
void do_swi (void)
{
	fatal (ERR_SWI);
}


__interrupt__
void do_swi2 (void)
{
	fatal (ERR_SWI2);
}


__interrupt__
void do_swi3 (void)
{
	fatal (ERR_SWI3);
}

