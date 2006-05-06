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

/**
 * \file
 * \brief Lamp management functions
 */

#include <freewpc.h>

/** Lamps 00h through 3Fh correspond to the physical lamp locations */
__fastram__ U8 lamp_matrix[NUM_LAMP_COLS];

/** Lamps 40h through 7Fh are 'virtual' and don't really exist, but
 * they can be used to store boolean values.
 * These two bitsets are kept adjacent, so the same functions can
 * be used to bit-twiddle both of them.
 * Both of these bitsets are also saved from player to player.
 * Note the flash sets are NOT saved and must be recalculated from
 * ball to ball. */
__fastram__ U8 bit_matrix[NUM_VLAMP_COLS];

/** Lamps 80h through BFh are the same as the regular lamps, but
 * control the flash state */
__fastram__ U8 lamp_flash_matrix[NUM_LAMP_COLS];

/** Lamps C0h through FFh are similarly used to control the
 * 'fast flash' state of the lamp.  Fast flash overrides slow
 * flash, which overrides the steady value. */
__fastram__ U8 lamp_fast_flash_matrix[NUM_LAMP_COLS];

/** Bitsets for doing temporary lamp effects, which hide the
 * normal state of the lamps */
__fastram__ U8 lamp_leff1_matrix[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff1_allocated[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff2_matrix[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff2_allocated[NUM_LAMP_COLS];


U8 lamp_flash_max;
U8 lamp_flash_count;

U8 lamp_apply_delay;

U8 lamp_strobe_mask;
U8 lamp_strobe_column;


void lamp_init (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS * 4);
	lamp_leff1_free_all ();

	lamp_flash_max = lamp_flash_count = LAMP_DEFAULT_FLASH_RATE;
	lamp_apply_delay = 0;

	lamp_strobe_mask = 0x1;
	lamp_strobe_column = 0;
}


/* Runs periodically to invert any lamps in the flashing state */
void lamp_flash_rtt (void)
{
	--lamp_flash_count;
	if (lamp_flash_count == 0)
	{
		U16 *lamp_matrix_words = (U16 *)lamp_matrix;
		U16 *lamp_flash_matrix_words = (U16 *)lamp_flash_matrix;

		lamp_matrix_words[0] ^= lamp_flash_matrix_words[0];
		lamp_matrix_words[1] ^= lamp_flash_matrix_words[1];
		lamp_matrix_words[2] ^= lamp_flash_matrix_words[2];
		lamp_matrix_words[3] ^= lamp_flash_matrix_words[3];
		lamp_flash_count = lamp_flash_max;
	}
}


void lamp_rtt (void)
{
	U8 bits;
	register U16 col;

	/* Setup the strobe */
	*(U8 *)WPC_LAMP_ROW_OUTPUT = 0;
	*(U8 *)WPC_LAMP_COL_STROBE = lamp_strobe_mask;

	/* Load the lamp column into U */
	col = lamp_strobe_column;

	/* Grab the default lamp values */
	bits = lamp_matrix[col];

#if 0
	/* Override with the fast flashing lamps */
	flashbits = lamp_fast_flash_matrix[col];
	bits &= ~flashbits;
	bits |= (flashbits & lamp_fast_flash_mask);
#endif

	/* Override with the lamp effect lamps */
	bits &= lamp_leff1_allocated[col];
	bits |= lamp_leff1_matrix[col];
#if 0
	bits &= lamp_leff2_allocated[col];
	bits |= lamp_leff2_matrix[col];
#endif

	/* Write the result to the hardware */
	*(U8 *)WPC_LAMP_ROW_OUTPUT = bits;

	/* Advance strobe to next position for next iteration */
	lamp_strobe_column++;
	lamp_strobe_column &= 7;
	lamp_strobe_mask <<= 1;
	if (lamp_strobe_mask == 0)
	{
		/* All columns strobed : reset strobe */
		lamp_strobe_mask = 0x1;
	}
}


/*
 *
 * Basic lamp manipulation routines
 *
 * These control the main lamp matrix and also the flash
 * matrix.
 *
 */

void lamp_on (lampnum_t lamp)
{
	register bitset p = lamp_matrix;
	register uint8_t v = lamp;
	__setbit(p, v);
}


void lamp_off (lampnum_t lamp)
{
	register bitset p = lamp_matrix;
	register uint8_t v = lamp;
	__clearbit(p, v);
}


void lamp_toggle (lampnum_t lamp)
{
	register bitset p = lamp_matrix;
	register uint8_t v = lamp;
	__togglebit(p, v);
}


int lamp_test (lampnum_t lamp)
{
	register bitset p = lamp_matrix;
	register uint8_t v = lamp;
	__testbit(p, v);
	return v;
}

void lamp_global_update ()
{
	call_hook (lamp_refresh);
	if (!in_test)
		lamp_start_update ();
}


void lamp_all_on (void)
{
	memset (lamp_matrix, 0xff, NUM_LAMP_COLS);
	memset (lamp_flash_matrix, 0, 2 * NUM_LAMP_COLS);
	lamp_global_update ();
}


void lamp_all_off (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS);
	memset (lamp_flash_matrix, 0, 2 * NUM_LAMP_COLS);
	lamp_global_update ();
}


void lamp_leff1_allocate_all (void)
{
	memset (lamp_leff1_allocated, 0, NUM_LAMP_COLS);
}

void lamp_leff1_erase (void)
{
	memset (lamp_leff1_matrix, 0, NUM_LAMP_COLS);
}

void lamp_leff1_free_all (void)
{	
	memset (lamp_leff1_allocated, 0xFF, NUM_LAMP_COLS);
}

void lamp_leff2_erase (void)
{
	memset (lamp_leff2_matrix, 0, 2 * NUM_LAMP_COLS);
}


void lamp_leff_allocate (lampnum_t lamp)
{
	register bitset p = lamp_leff1_allocated;
	register uint8_t v = lamp;
	__clearbit(p, v);
}


void lamp_leff_free (lampnum_t lamp)
{
	register bitset p = lamp_leff1_allocated;
	register uint8_t v = lamp;
	__setbit(p, v);
}


void leff_on (lampnum_t lamp)
{
	register bitset p = lamp_leff1_matrix;
	register uint8_t v = lamp;
	__setbit(p, v);
}


void leff_off (lampnum_t lamp)
{
	register bitset p = lamp_leff1_matrix;
	register uint8_t v = lamp;
	__clearbit(p, v);
}


void leff_toggle (lampnum_t lamp)
{
	register bitset p = lamp_leff1_matrix;
	register uint8_t v = lamp;
	__togglebit(p, v);
}


int leff_test (lampnum_t lamp)
{
	register bitset p = lamp_leff1_matrix;
	register uint8_t v = lamp;
	__testbit(p, v);
	return v;
}


