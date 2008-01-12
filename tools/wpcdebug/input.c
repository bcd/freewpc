/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/* Ring buffer for storing cycle counts.
 * This tracks the history of the last 256 IRQs.
 *
 * Each entry in the array says what percentage of time
 * between IRQs was actually spent in the IRQ handler.
 * Low values are good, meaning the IRQ is efficient.
 * The most each entry should be is 100, which means
 * that there was no time between IRQs for anything else.
 * A value over 100 is very bad, and means we didn't finish
 * the IRQ before the next one should have fired; this
 * leads to loss of timing information.
 */
static int irq_percent_array[256] = { 0, };
static int irq_percent_count[256] = { 0, };
int cycle_offset = 0;


/*
 * Called when the WPC program wants to tell us
 * that it has finished executing an IRQ handler.
 */
static void
read_cycle_stat (int fd)
{
	int irq_cycle_count, irq_percent;
	uint8_t val;

	/* Read the timer snapshot.  val is the number of 16-cycle
	increments that elapsed since the timer was reset.
	This tells us how long the IRQ took, since the timer
	is zeroed on entry. */
	read (fd, &val, 1);

	/* Convert to a true cycle count.  Subtract 24 cycles
	since that is how many extra cycles it takes the
	IRQ handler to read/write the debug registers to
	pass this information back.  In other words, the
	values reported are the times if profiling was turned off.
	The actual timing for this run therefore is a little more.
	The overhead may change slightly if the IRQ profiling code
	is ever modified. */
	irq_cycle_count = (val * 16) - 24;

	/* Convert this to a percentage of time spent in the IRQ,
	since the last one.  There are 1952 cycles per IRQ.
	irq_percent should range between 0 and 100. */
	irq_percent = (irq_cycle_count * 100UL) / 1952;

	/* Log the measurement */
	irq_percent_array[cycle_offset] += irq_percent;
	irq_percent_count[cycle_offset]++;
	cycle_offset++;
	if (cycle_offset == 256)
	{
		/* Every 256 measurements, generate output to the console
		so you can see what's going on.

		You should see roughly 5 different values, in various
		frequencies, which correspond to the 1ms, 8ms, 32ms,
		and 128ms realtime tasks.

		The 1ms task alternates between switches and lamps, so
		those are the first two values.
		*/
		int i;
		for (i=0; i < 256; i++)
		{
			printf ("%d ", irq_percent_array[i] / irq_percent_count[i]);
			if ((i % 16) == 15)
				putchar ('\n');
		}
		putchar ('\n');
		cycle_offset = 0;
	}
}


/* Process a character from the WPC program.  Anytime it writes
to the debugger output register, this function is called to
take action.  For most values, this is just ASCII text that
gets printed.  Some values are interpreted to do special things,
like profiling and debugging. */
void
process_input (int fd, uint8_t val)
{
	switch (val)
	{
		case 0xDD:
			read_cycle_stat (fd);
			break;

		default:
			write (1, &val, 1);
			break;
	}
}

