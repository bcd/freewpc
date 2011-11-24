/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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


/** The contents of the 16 PIC serial data registers */
U8 pic_serial_data[16] = { 0, };

/** The value for the machine number to program into the PIC.
This defaults to the correct value as specified by MACHINE_NUMBER,
but it can be changed to test mismatches. */
#ifndef MACHINE_NUMBER
#define MACHINE_NUMBER 0
#endif
unsigned int pic_machine_number = MACHINE_NUMBER;

void simulation_pic_init (void)
{
	static U8 real_serial_number[] = { 0, 0, 0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 0, 0, 0 };
	U32 tmp;

	/* Add a knob to game the default game number */
	conf_add ("pic.gameno", &pic_machine_number);

	/* Initialize the PIC with the desired machine number. */
	real_serial_number[0] = pic_machine_number / 100;
	real_serial_number[1] = (pic_machine_number / 10) % 10;
	real_serial_number[2] = pic_machine_number % 10;

	/* TODO : Initialize the three byte switch matrix unlock code */

	/* Now encode the 17-byte serial number into the 16 PIC registers. */

	pic_serial_data[10] = 0x0;
	pic_serial_data[2] = 0x0;

	tmp = 100 * real_serial_number[1] + 10 * real_serial_number[7] +
		real_serial_number[4] + 5 * pic_serial_data[10];
	tmp = (tmp * 0x1BCD) + 0x1F3F0UL;
	pic_serial_data[1] = (tmp >> 16) & 0xFF;
	pic_serial_data[11] = (tmp >> 8) & 0xFF;
	pic_serial_data[9] = tmp  & 0xFF;

	tmp = 10000 * real_serial_number[2] + 1000 * real_serial_number[15] +
		100 * real_serial_number[0] + 10 * real_serial_number[8] +
		real_serial_number[6] + 2 * pic_serial_data[10] +
		pic_serial_data[2];
	tmp = (tmp * 0x107F) + 0x71E259UL;
	pic_serial_data[7] = (tmp >> 24) & 0xFF;
	pic_serial_data[12] = (tmp >> 16) & 0xFF;
	pic_serial_data[0] = (tmp >> 8) & 0xFF;
	pic_serial_data[8] = tmp  & 0xFF;

	tmp = 1000 * real_serial_number[16] + 100 * real_serial_number[3] +
		10 * real_serial_number[5] + real_serial_number[14] +
		pic_serial_data[2];
	tmp = (tmp * 0x245) + 0x3D74;
	pic_serial_data[3] = (tmp >> 16) & 0xFF;
	pic_serial_data[14] = (tmp >> 8) & 0xFF;
	pic_serial_data[6] = tmp  & 0xFF;

	tmp = 10000 * real_serial_number[13] + 1000 * real_serial_number[12] +
		100 * real_serial_number[11] + 10 * real_serial_number[10] +
		real_serial_number[9];
	tmp = 99999UL - tmp;
	pic_serial_data[15] = (tmp >> 8) & 0xFF;
	pic_serial_data[4] = tmp & 0xFF;
}


/** Access the simulated PIC.
 * writep is 1 if this is a write command, 0 on a read.
 * For writes, WRITE_VAL specifies the value to be written.
 *
 * For reads, it returns the read value, otherwise it returns
 * zero.
 */
U8 simulation_pic_access (int writep, U8 write_val)
{
	static U8 last_write = 0xFF;
	static int writes_until_unlock_needed = 1000;
	static int unlock_mode = 0;
	static U32 unlock_code;
	const U32 expected_unlock_code = 0;

	/* TODO : Break this into two different functions; use I/O module. */
	if (writep)
	{
		/* Handles writes to the PIC */
		if (last_write == 0xFF && write_val != WPC_PIC_RESET)
		{
			simlog (SLC_DEBUG, "PIC write %02X before reset.", write_val);
		}
		else if (unlock_mode > 0)
		{
			unlock_code = (unlock_code << 8) | write_val;
			if (++unlock_mode > 3)
			{
				if (unlock_code != expected_unlock_code)
				{
					static int already_warned = 0;
					if (!already_warned)
						simlog (SLC_DEBUG, "Invalid PIC unlock code %X (expected %X)\n",
							unlock_code, expected_unlock_code);
					already_warned = 1;
					unlock_mode = -1;
				}
				else
				{
					unlock_mode = 0;
				}
			}
		}
		else if (write_val == WPC_PIC_UNLOCK)
		{
			unlock_mode = 1;
			unlock_code = 0;
		}
		else
		{
			last_write = write_val;
			if (writes_until_unlock_needed > 0)
				writes_until_unlock_needed--;
		}
		return 0;
	}
	else
	{
		/* Handles reads to the PIC */
		switch (last_write)
		{
			case WPC_PIC_RESET:
			case WPC_PIC_UNLOCK:
				return 0;

			case WPC_PIC_COUNTER:
				return writes_until_unlock_needed;

			case WPC_PIC_COLUMN(0): case WPC_PIC_COLUMN(1):
			case WPC_PIC_COLUMN(2): case WPC_PIC_COLUMN(3):
			case WPC_PIC_COLUMN(4): case WPC_PIC_COLUMN(5):
			case WPC_PIC_COLUMN(6): case WPC_PIC_COLUMN(7):
			{
				unsigned int col;
				U8 val;

				if (unlock_mode > 0)
				{
					simlog (SLC_DEBUG, "Column read in unlock mode");
					return 0;
				}

				col = last_write - WPC_PIC_COLUMN(0);
				if (col >= 8)
				{
					simlog (SLC_DEBUG, "Invalid column %d", col);
					return 0;
				}

				val = sim_switch_matrix_get ()[col + 1];
				return val;
			}

			case WPC_PIC_SERIAL(0): case WPC_PIC_SERIAL(1):
			case WPC_PIC_SERIAL(2): case WPC_PIC_SERIAL(3):
			case WPC_PIC_SERIAL(4): case WPC_PIC_SERIAL(5):
			case WPC_PIC_SERIAL(6): case WPC_PIC_SERIAL(7):
			case WPC_PIC_SERIAL(8): case WPC_PIC_SERIAL(9):
			case WPC_PIC_SERIAL(10): case WPC_PIC_SERIAL(11):
			case WPC_PIC_SERIAL(12): case WPC_PIC_SERIAL(13):
			case WPC_PIC_SERIAL(14): case WPC_PIC_SERIAL(15):
				return pic_serial_data[last_write - WPC_PIC_SERIAL(0)];

			default:
				simlog (SLC_DEBUG, "Invalid PIC address read");
				return 0;
		}
	}
}


