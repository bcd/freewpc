/*
 * Copyright 2007-2010 by Brian Dominy <brian@oddchange.com>
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


/** Called at the beginning of every IRQ */
extern inline void do_irq_begin (void)
{
	/* If using the RAM paging facility, ensure that page 0
	 * is visible for the IRQ */
#ifdef CONFIG_PAGED_RAM
	pinio_set_bank (PINIO_BANK_RAM, 0);
#endif

	/* On WPC, clear the source of the periodic interrupt, and reset
	the hardware watchdog */
#ifdef CONFIG_PLATFORM_WPC
	pinio_clear_periodic ();
#endif
}


/** Advance the system clock.  This is currently in 16ms units, because
it is scheduled every 16 IRQs. */
extern inline void advance_time_rtt (void)
{
	extern U16 sys_time;
	sys_time++;
}


/** Called at the end of every IRQ */
extern inline void do_irq_end (void)
{
}

