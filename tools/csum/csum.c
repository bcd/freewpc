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

/* This program can read/write WPC ROM checksums.  It works for both
original WPC ROMS and also those running FreeWPC. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

/** The sector size.  The ROM file is read in chunks of this amount. */
#define SECTOR_SIZE   0x8000

/** The offset within the system page (uppermost 32KB) where the
16-bit checksum resides. */
#define CKSUM_OFFSET  0xFFEE - 0x8000

/** The offset within the system page (uppermost 32KB) where the
16-bit delta resides.  This location is used to 'correct' the
checksum so that it is valid. */
#define DELTA_OFFSET  0xFFEC - 0x8000

/** The value of the delta field that causes the software to ignore
the checksum.  This is not implemented on FreeWPC but most real WPC
ROMs do this. */
#define DISABLE_CKSUM_DELTA 0x00FF

/** Returns the high byte of a 16-bit word */
#define HI(x)   ((x) / 256)

/** Returns the low byte of a 16-bit word */
#define LO(x)   ((x) % 256)

/** Returns the version component of a 16-bit WPC checksum */
#define CKSUM_VER(ck)   LO(ck)


int
main (int argc, char *argv[])
{
	char szRomName[128];
	FILE *fpRom;
	unsigned char sector[SECTOR_SIZE];
	uint32_t cksum = 0, rom_cksum = 0, rom_delta = 0;
	uint32_t rc, i, test_cksum = 0, adj_cksum;
	uint32_t subst_found = 0;
	uint32_t update = 0, test_delta = 0, desired_ver = 0; 
   int c;
   const char *options = "f:uhs:v:";
   int screwup = 0;
   int disable_cksum = 0;
	
	/* Process command-line options. */
   while ((c = getopt (argc, argv, options)) != -1)
   {
      switch (c)
      {
         case 'f':
				/* -f sets the name of the input/output ROM file */
            sprintf (szRomName, "%s", optarg);
            break;

         case 'u':
				/* -u enables update mode, which causes the checksum
				to be recalculated and saved back */
            update = 1;
        		printf ("Autoupdate enabled\n");
            break;
                        
         case 'h':
            printf ("FreeWPC Checksum Utility\n");
				printf ("-f <filename>     Sets input filename\n");
				printf ("-v <version>      Sets desired version number\n");
				printf ("-u                Update mode (default: read-only)\n");
				printf ("-d                Disable existing checksum verification\n");
				printf ("-s <delta>        Screw up the ROM so checksum will fail\n");
            exit (0);
            break;
            
         case 's':
				/* -s causes the input file data to be distorted (in memory,
				not on disk).  This should cause checksum verification to
				fail. */
            printf ("screwup enabled\n");
            screwup = strtoul (optarg, NULL, 0);
            break;
            
         case 'v':
            desired_ver = strtoul (optarg, NULL, 0);
            break;
            
         case 'd':
            disable_cksum = 1;
            break;
      }
   }
  
  	/* Open the ROM file */
	fpRom = fopen (szRomName, "r+b");
	if (!fpRom)
	{
		fprintf (stderr, "Cannot open %s\n", szRomName);
		exit (1);
	}

	/* Read each sector of the ROM image into memory.
	 * Calculate the existing checksum, 'cksum'.
	 * If screwup is enabled, modify one byte per sector.
	 * In the final sector, grab the existing checksum
	 * 'rom_cksum' and existing fudge factor 'rom_delta'.
	 *
	 * When finished, 'sector' will contain the final page, which
	 * contains all of the modifiable parts of the ROM.  This is
	 * manipulated below and will then be written back to the file.
	 */
	for (;;)
	{
		rc = fread (sector, 1, SECTOR_SIZE, fpRom);
		if (feof (fpRom))
			break;

		if (rc < SECTOR_SIZE)
		{
		}

      if (screwup)
         sector[1] = (sector[1] + screwup) & 0xFF;
      
		for (i = 0; i < SECTOR_SIZE; i++)
			cksum += sector[i];
	}

	/* Grab checksums from the last sector read */
	rom_cksum = sector[CKSUM_OFFSET] * 256 + sector[CKSUM_OFFSET + 1];
	rom_delta = sector[DELTA_OFFSET] * 256 + sector[DELTA_OFFSET + 1];

	/* Make sure checksums are 16-bit values */
	cksum &= 0xFFFF;
	rom_cksum &= 0xFFFF;

	/* Display results */
	printf ("Calculated checksum: %04XH (version %02XH)\n",
		cksum, CKSUM_VER(cksum));
	printf ("Stored checksum: %04XH (version %02XH)\n",
		rom_cksum, CKSUM_VER(rom_cksum));
   printf ("Store delta: %04XH\n", rom_delta);

	/* Now what to do?
	 * If checksum should be disabled, but it's not currently disabled,
	 * then store the disable code (0x00FF) in the delta.
	 */
   if (disable_cksum && (rom_delta != DISABLE_CKSUM_DELTA))
   {
      printf ("Disable checksum requested, modification required.\n");
      if (update == 1)
      {
         sector[DELTA_OFFSET] = HI(DISABLE_CKSUM_DELTA);
         sector[DELTA_OFFSET + 1] = LO(DISABLE_CKSUM_DELTA);
         printf ("ROM modified to disable checksum.\n");
      }
   }

	/* If the checksum doesn't match, or the version component of the
	checksum is wrong, then correct it. */
	else if ((cksum != rom_cksum) 
		|| (desired_ver && (desired_ver != CKSUM_VER(rom_cksum))))
	{
      printf ("Figuring out how to correct checksum...\n");

		/* Clear the checksum and delta fields in the ROM file.
		 * Subtract these from the previously calculated checksum. */
		cksum -= (HI (rom_cksum) + LO (rom_cksum));
      cksum &= 0xFFFF;
		cksum -= (HI (rom_delta) + LO (rom_delta));
      cksum &= 0xFFFF;

		/* Try all possible 16-bit values for delta, until one is
		found that satisfies the constraints. */
		for (test_delta = 0; test_delta <= 0xFFFF; test_delta++)
		{
         if (test_delta == DISABLE_CKSUM_DELTA)
            continue;
            
			for (test_cksum = desired_ver; 
					test_cksum <= 0xFFFF; 
					test_cksum += 0x0100)
			{
				adj_cksum = (cksum + HI (test_cksum) + LO (test_cksum)
					+ HI(test_delta) + LO(test_delta)) & 0xFFFF;
				if (adj_cksum == test_cksum)
				{
					printf
						("Solution: use checksum = %04XH, delta = %04XH\n",
						 test_cksum, test_delta);
					subst_found = 1;

					/* Only update the file when in update mode (-u given). */
					if (update == 1)
					{
						sector[DELTA_OFFSET] = HI(test_delta);
						sector[DELTA_OFFSET + 1] = LO(test_delta);
						sector[CKSUM_OFFSET] = HI (test_cksum);
						sector[CKSUM_OFFSET + 1] = LO (test_cksum);
						fseek (fpRom, -SECTOR_SIZE, SEEK_CUR);

						printf ("Writing changes to ROM file...\n");
						fwrite (sector, 1, SECTOR_SIZE, fpRom);
						printf ("ROM file updated.\n");
					}
					break;
				}
			}
         if (subst_found) break;
		}

		if (subst_found == 0)
			printf ("Error: can't figure out how to update checksum\n");
	}

	else
	{
		printf ("Checksums match; no action required.\n");
	}


	fclose (fpRom);
	return (0);
}

