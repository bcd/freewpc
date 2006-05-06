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

/* This file implements a simple S-record to binary converter. */

#include <stdio.h>
#include <string.h>


typedef unsigned char U8;

/** Global array that contains the binary data, up to 64KB */
U8 image[0x10000];

/** The fill byte to be used for sections that aren't covered
 * by the S-record input file. */
U8 fill_byte = 0x00;


/** Convert a string of 'len' hex digits into decimal */
unsigned int hexval (char *s, int len)
{
	unsigned int r = 0;
	while (--len >= 0)
	{
		r *= 16;
		r += ((*s >= 'A') ? (*s - 'A' + 10) : (*s - '0'));
		s++;
	}
	return r;
}

int main (int argc, char *argv[])
{
	FILE *ifp;
	FILE *ofp;
	char line[256];
	char *s;
	int len;
	int addr;
	int i;
	char *srec_file = "freewpc.s19";
	char *bin_file = "freewpc.bin";
	char *option;
	int start_offset = 0;
	int write_length = 0;
	int coco_bin_format = 0;
	char **argn = &argv[1];
	int min_addr = 0xFFFF, max_addr = 0;

	/* Process command-line options */
	while (*argn != NULL)
	{
		if (**argn == '-')
		{
			switch ((*argn++)[1])
			{
				case 'o':
					bin_file = *argn++;
					break;

				case 'f':
					fill_byte = strtoul (*argn++, NULL, 0);
					break;

				case 's':
					start_offset = strtoul (*argn++, NULL, 0);
					break;

				case 'l':
					write_length = strtoul (*argn++, NULL, 0);
					break;

				case 'C':
					coco_bin_format = 1;
					break;
			}
		}
		else
		{
			srec_file = *argn++;
			break;
		}
	}

	/* Initialize output to the fill byte */
	memset (image, fill_byte, sizeof (image));

	/* Open the S-record file */
	ifp = fopen (srec_file, "r");
	for (;;)
	{
		/* Read the next line of input */
		fgets (line, 255, ifp);
		if (feof (ifp))
			break;

		s = line;

		/* Skip over S */
		s++;

		/* Check S-record type */
		if (*s == '9')
			break;
		s++;

		/* Next two bytes are record length */
		len = hexval (s, 2) - 3;
		s += 2;

		/* Next four bytes are address in hex */
		addr = hexval (s, 4);
		s += 4;

		/* Next len bytes are data */
		for (i=0; i < len; i++)
		{
			image[addr+i] = hexval (s, 2);
			s += 2;
		}

		/* Update min/max addr */
		if (addr < min_addr)
			min_addr = addr;
		if (addr+len-1 > max_addr)
			max_addr = addr+len-1;

	}
	fclose (ifp);

	/* If the length to write is given as zero, then
	 * autocompute the length based on the min/max
	 * addresses seen in the input */
	if (write_length == 0)
		write_length = max_addr - min_addr + 1;

	/* Open the binary file for writing */
	ofp = fopen (bin_file, "wb");

	/* If the COCO option is turned on, then prepend
	 * the output with the 5-byte header used for
	 * executables. */
	if (coco_bin_format)
	{
		line[0] = 0;
		line[1] = (write_length & 0xFF00) >> 8;
		line[2] = write_length & 0xFF;
		line[3] = (start_offset & 0xFF00) >> 8;
		line[4] = start_offset & 0xFF;
		fwrite (line, sizeof (U8), 5, ofp);
	}

	/* Write the data */
	fwrite (image + start_offset, 
		sizeof (U8), 
		write_length, ofp);

	/* Likewise, write the 5-byte terminator on the Coco */
	if (coco_bin_format)
	{
		line[0] = 0xFF;
		line[1] = 0;
		line[2] = 0;
		line[3] = (start_offset & 0xFF00) >> 8;
		line[4] = start_offset & 0xFF;
		fwrite (line, sizeof (U8), 5, ofp);
	}

	fclose (ofp);

	exit (0);
}

