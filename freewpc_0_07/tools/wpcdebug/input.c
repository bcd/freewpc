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


void
read_cycle_stat (int fd)
{
	char buf[16];
	int irq_cycle_count;
	uint8_t val;

	read (fd, &val, 1);

	irq_cycle_count = val * 16;
	sprintf (buf, "%d (%d)\n", 
		irq_cycle_count,
		(irq_cycle_count * 100UL) / 1952);
	write (1, buf, strlen(buf));
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

