/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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
#include <simulation.h>
#include <hwsim/io.h>

/* A table of I/O addresses.  Every valid IO addr has an entry here that says
how to read and write from that address. */
struct io_region io_region_table[NUM_IO_ADDRS];


/* Default read/write handlers for invalid addresses, or addresses that were
not installed by the simulator */

U8 io_null_reader (IOPTR addr, void *data)
{
	return 0xFF;
}

void io_null_writer (IOPTR addr, U8 val, void *data)
{
}


/* Handle I/O write requests from the CPU */
void io_write (IOPTR addr, U8 val)
{
	if ((addr < MIN_IO_ADDR) || (addr >= MAX_IO_ADDR))
	{
		io_null_writer (addr, val, NULL);
		return;
	}
	int r = addr - MIN_IO_ADDR;
	void *data = io_region_table[r].data;
	io_region_table[r].writer (addr, val, data);
}


/* Handle I/O read requests from the CPU */
U8 io_read (IOPTR addr)
{
	if ((addr < MIN_IO_ADDR) || (addr >= MAX_IO_ADDR))
		return io_null_reader (addr, NULL);
	int r = addr - MIN_IO_ADDR;
	void *data = io_region_table[r].data;
	return io_region_table[r].reader (addr, data);
}


/* Add read/write handlers for a particular I/O address region. */
void io_add (IOPTR addr, unsigned int len, io_reader reader, io_writer writer, void *data)
{
	if ((addr < MIN_IO_ADDR) || (addr+len >= MAX_IO_ADDR))
		return;
	int r = addr - MIN_IO_ADDR;
	while (len > 0)
	{
		io_region_table[r].reader = reader;
		io_region_table[r].writer = writer;
		io_region_table[r].data = data;
		len--;
	}
}

/* Initialize the I/O table */
void io_init (void)
{
	int r;
	for (r=0; r < NUM_IO_ADDRS; r++)
	{
		io_region_table[r].reader = io_null_reader;
		io_region_table[r].writer = io_null_writer;
		io_region_table[r].data = NULL;
	}
}

