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

#ifndef _HWSIM_MATRIX_H
#define _HWSIM_MATRIX_H

/*****
 *****	Generic I/O matrix
 *****/

struct io_matrix
{
	unsigned char *rowptr;
	unsigned char rowlatch;
	unsigned char rowdata[0];
};

struct io_switch_matrix
{
	struct io_matrix header;
	unsigned char data[SWITCH_BITS_SIZE+1];
};

struct io_lamp_matrix
{
	struct io_matrix header;
	unsigned char data[NUM_LAMP_COLS];
};


#endif /* _HWSIM_MATRIX_H */
