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
	how to simulate reading and writing from that address.

	Use one of the io_add variants to populate this table.  You can provide
	your own custom functions (of type io_reader and io_writer), or use some
	of the defaults in this file that provide common functionality.

	When you populate an I/O address handler, you can register an arbitrary
	void pointer that will be passed back to the handler at access time. */
struct io_region io_region_table[NUM_IO_ADDRS];


/* Default read/write handlers for invalid addresses, or addresses that were
not installed by the simulator */

U8 io_null_reader (void *data, unsigned int offset)
{
	return 0xFF;
}

void io_null_writer (void *data, unsigned int offset, U8 val)
{
}


/* Handle I/O write requests from the CPU */
void writeb (IOPTR addr, U8 val)
{
	if ((addr < MIN_IO_ADDR) || (addr >= MAX_IO_ADDR))
	{
		io_null_writer (NULL, addr, val);
		return;
	}
	int r = addr - MIN_IO_ADDR;
	void *data = io_region_table[r].data;
	unsigned int offset = io_region_table[r].offset;
#ifdef CONFIG_IO_DEBUG
	simlog (SLC_DEBUG, "writeb: %04X (%02X)", addr, val);
#endif
	io_region_table[r].writer (data, offset, val);
}


/* Handle I/O read requests from the CPU */
U8 readb (IOPTR addr)
{
	if ((addr < MIN_IO_ADDR) || (addr >= MAX_IO_ADDR))
		return io_null_reader (NULL, addr);
	int r = addr - MIN_IO_ADDR;
	void *data = io_region_table[r].data;
	unsigned int offset = io_region_table[r].offset;
	U8 val = io_region_table[r].reader (data, offset);
#ifdef CONFIG_IO_DEBUG
	simlog (SLC_DEBUG, "readb: %04X (%02X)", addr, val);
#endif
	return val;
}


/* Add read/write handlers for a particular I/O address region.
	This is the lowest level function that defines how to do I/O.  All
	other variants call this one ultimately. */
void io_add_1 (IOPTR addr, unsigned int len, io_reader reader, io_writer writer, void *data)
{
	if ((addr < MIN_IO_ADDR) || (addr+len > MAX_IO_ADDR))
		return;
	int r = addr - MIN_IO_ADDR;
	int offset = 0;
	while (len > 0)
	{
		io_region_table[r].reader = reader;
		io_region_table[r].writer = writer;
		io_region_table[r].data = data;
		io_region_table[r].offset = offset;
		len--;
		offset++;
		r++;
	}
}


/* Initialize the I/O table */
void io_init (void)
{
	int r;

	/* By default, I/O regions map to the null handlers */
	for (r=0; r < NUM_IO_ADDRS; r++)
	{
		io_region_table[r].reader = io_null_reader;
		io_region_table[r].writer = io_null_writer;
		io_region_table[r].data = NULL;
		io_region_table[r].offset = 0;
	}

	/* Call the platform-specific function to install whatever support
	is required. */
#ifdef CONFIG_PLATFORM_WPC
	io_wpc_init ();
#endif	
}

