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

struct io_p2k
{
	U8 indata;
	U8 status;
};

struct io_p2k p2k_hw;


U8 io_p2k_read_data (struct io_p2k *obj, unsigned int addr)
{
}

void io_p2k_write_data (struct io_p2k *obj, unsigned int addr, U8 val)
{
}

U8 io_p2k_read_status (struct io_p2k *obj, unsigned int addr)
{
}

void io_p2k_write_control (struct io_p2k *obj, unsigned int addr, U8 val)
{
}


void io_p2k_init (void)
{
	io_add_rw (LPT_DATA, io_p2k_read_data, io_p2k_write_data, &p2k_hw);
	io_add_wo (LPT_CONTROL, io_p2k_write_control, &p2k_hw);
	io_add_ro (LPT_STATUS, io_p2k_read_status, &p2k_hw);
}


