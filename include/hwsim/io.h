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

#ifndef _HWSIM_IO_H
#define _HWSIM_IO_H

#define MIN_IO_ADDR 0x3C00
#define MAX_IO_ADDR 0x4000
#define NUM_IO_ADDRS (MAX_IO_ADDR - MIN_IO_ADDR)

typedef U8 (*io_reader) (IOPTR addr, void *data);
typedef void (*io_writer) (IOPTR addr, U8 val, void *data);

struct io_region
{
	io_reader reader;
	io_writer writer;
	void *data;
};


U8 io_null_reader (IOPTR addr, void *data);
void io_null_writer (IOPTR addr, U8 val, void *data);
void io_write (IOPTR addr, U8 val);
U8 io_read (IOPTR addr);
void io_add (IOPTR addr, unsigned int len, io_reader reader, io_writer writer, void *data);
void io_init (void);

#endif /*  _HWSIM_IO_H */
