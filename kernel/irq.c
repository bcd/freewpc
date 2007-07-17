/*
 * Copyright 2007 by Brian Dominy <brian@oddchange.com>
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
 * \brief The top-level IRQ handler
 */

#include <freewpc.h>

/** The number of IRQ loops executed.  A loop consists of 8 IRQs. */
__fastram__ U8 irq_loop_count;

/** The number of task ticks executed.  A tick equals 16 IRQs. */
__fastram__ U8 tick_count;

/** A pointer to the currently installed IRQ function handler. */
__fastram__ void (*irq_function) (void);


__interrupt__ void do_irq0 (void);
__interrupt__ void do_irq1 (void);
__interrupt__ void do_irq2 (void);
__interrupt__ void do_irq3 (void);
__interrupt__ void do_irq4 (void);
__interrupt__ void do_irq5 (void);
__interrupt__ void do_irq6 (void);
__interrupt__ void do_irq7 (void);


/** Initialize the IRQ subsystem.  This must be invoked before
enabling the IRQ for the first time. */
void irq_init (void)
{
	irq_loop_count = 0;
	irq_function = do_irq0;
}


/** Realtime function that is called every 1ms */
static inline void do_irq_begin (void)
{
	/* If using the RAM paging facility, ensure that page 0
	 * is visible for the IRQ */
#ifdef CONFIG_PAGED_RAM
	wpc_set_ram_page (0);
#endif

	/* Clear the source of the interrupt */
	wpc_write_irq_clear (0x96);

	/* When building a profiling program, count the
	 * number of IRQs by writing to the pseudoregister
	 * WPC_PINMAME_CYCLE_COUNT every time we take an IRQ.
	 * Pinmame has been modified to understand this and
	 * keep track of how long we're spending in the IRQ
	 * based on these writes.  This should never be defined
	 * for real hardware. */
#ifdef IRQPROFILE
	wpc_asic_write (WPC_PINMAME_CYCLE_COUNT, 0);
#endif

}


static inline void do_irq_1ms (void)
{
#ifdef MACHINE_1MS_RTTS
	MACHINE_1MS_RTTS
#endif
}


static inline void do_irq_end (void)
{
	/* Again, for profiling, we mark the end of an IRQ
	 * by writing these markers. */
#ifdef IRQPROFILE
	wpc_debug_write (0xDD);
	wpc_debug_write (wpc_asic_read (WPC_PINMAME_CYCLE_COUNT));
#endif
}

static inline void do_irq_2ms_a (void)
{
	lamp_rtt (); /* TODO : split into 2 rtts for strobing */
	flipper_rtt ();
	triac_rtt ();
}

static inline void do_irq_2ms_b (void)
{
	switch_rtt ();
	ac_rtt ();
}


/** Realtime function that is called every 8ms */
static inline void do_irq_8ms (void)
{
	/* Execute rtts every 8ms */
	sol_rtt ();
	flasher_rtt ();
#ifdef MACHINE_8MS_RTTS
	MACHINE_8MS_RTTS
#endif

	/* Increment total number of IRQ loops */
	irq_loop_count++;

	/* Every other 8ms (i.e. every 16ms), bump the tick count */
	if ((irq_loop_count & 1) == 0)
	{
		tick_count++;

		/* Every 4 loops, run the 32ms rtts */
		if ((irq_loop_count & 3) == 0)
		{
			wpc_led_toggle ();
			sound_rtt ();
			lamp_flash_rtt ();
#ifdef MACHINE_32MS_RTTS
			MACHINE_32MS_RTTS
#endif

			/* Every 16 loops, execute the 128ms rtts */
			if ((irq_loop_count & 15) == 0)
			{
#ifndef CONFIG_PLATFORM_LINUX
				lockup_check_rtt ();
#endif
				/* Execute machine-specific rtts */
#ifdef MACHINE_128MS_RTTS
				MACHINE_128MS_RTTS
#endif
			}
		}
	}
}


/* Eight different functions are defined for the IRQ, which are
cycled through in an 8ms period. */


__interrupt__ void do_irq0 (void)
{
	do_irq_begin ();
	do_irq_1ms ();
	do_irq_2ms_a ();
	irq_function = do_irq1;
	do_irq_end ();
}

__interrupt__ void do_irq1 (void)
{
	do_irq_begin ();
	do_irq_1ms ();
	do_irq_2ms_b ();
	irq_function = do_irq2;
	do_irq_end ();
}

__interrupt__ void do_irq2 (void)
{
	do_irq_begin ();
	do_irq_1ms ();
	do_irq_2ms_a ();
	irq_function = do_irq3;
	do_irq_end ();
}

__interrupt__ void do_irq3 (void)
{
	do_irq_begin ();
	do_irq_1ms ();
	do_irq_2ms_b ();
	irq_function = do_irq4;
	do_irq_end ();
}

__interrupt__ void do_irq4 (void)
{
	do_irq_begin ();
	do_irq_1ms ();
	do_irq_2ms_a ();
	irq_function = do_irq5;
	do_irq_end ();
}

__interrupt__ void do_irq5 (void)
{
	do_irq_begin ();
	do_irq_1ms ();
	do_irq_2ms_b ();
	irq_function = do_irq6;
	do_irq_end ();
}

__interrupt__ void do_irq6 (void)
{
	do_irq_begin ();
	do_irq_1ms ();
	do_irq_2ms_a ();
	irq_function = do_irq7;
	do_irq_end ();
}

__interrupt__ void do_irq7 (void)
{
	do_irq_begin ();
	do_irq_1ms ();
	do_irq_2ms_b ();
	do_irq_8ms ();
	irq_function = do_irq0;
	do_irq_end ();
}


/** The top-level IRQ function, that is installed into the IRQ
vector.  This is a small function that just calls the real handler
through a pointer. */
void do_irq (void)
{
	(*irq_function) ();
}


