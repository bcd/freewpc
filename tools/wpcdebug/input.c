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

#include <stdlib.h>
#include <inttypes.h>


static int irq_percent_array[256] = { 0, };
static int irq_percent_count[256] = { 0, };
int cycle_offset = 0;

void
read_cycle_stat (int fd)
{
	int irq_cycle_count, irq_percent;
	uint8_t val;

	read (fd, &val, 1);

	irq_cycle_count = (val * 16) - 24;
	irq_percent = (irq_cycle_count * 100UL) / 1952;

	irq_percent_array[cycle_offset] += irq_percent;
	irq_percent_count[cycle_offset]++;
	cycle_offset++;
	if (cycle_offset == 256)
	{
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

