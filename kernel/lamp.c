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

/** Lamps 40h through 7Fh are 'virtual' and don't really exist, but
 * they can be used to store boolean values.
 * These two bitsets are kept adjacent, so the same functions can
 * be used to bit-twiddle both of them.
 * Both of these bitsets are also saved from player to player.
 * Note the flash sets are NOT saved and must be recalculated from
 * ball to ball. */

/** Lamps 80h through BFh are the same as the regular lamps, but
 * control the flash state */

/** Lamps C0h through FFh are similarly used to control the
 * 'fast flash' state of the lamp.  Fast flash overrides slow
 * flash, which overrides the steady value. */

__fastram__ struct bit_matrix_table
{
	U8 solid_lamps[NUM_LAMP_COLS];
	U8 bits[NUM_VLAMP_COLS];
	U8 flashing_lamps[NUM_LAMP_COLS];
	U8 fast_flashing_lamps[NUM_LAMP_COLS];

	U8 flashing_lamps_now[NUM_LAMP_COLS];
} bit_matrix_array;

#define lamp_matrix					bit_matrix_array.solid_lamps
#define bit_matrix					bit_matrix_array.bits
#define lamp_flash_matrix			bit_matrix_array.flashing_lamps
#define lamp_fast_flash_matrix	bit_matrix_array.fast_flashing_lamps
#define lamp_flash_matrix_now		bit_matrix_array.flashing_lamps_now

/** Bitsets for doing temporary lamp effects, which hide the
 * normal state of the lamps */
__fastram__ U8 lamp_leff1_matrix[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff1_allocated[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff2_matrix[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff2_allocated[NUM_LAMP_COLS];


U8 lamp_flash_max;
U8 lamp_flash_count;
/// U8 lamp_apply_delay;

__fastram__ U8 lamp_strobe_mask;
__fastram__ U8 lamp_strobe_column;


/** Initialize the lamp subsystem at startup. */
void lamp_init (void)
{
	memset (&bit_matrix_array, 0, sizeof (bit_matrix_array));

	/* Lamp effect allocation matrices are "backwards",
	 * in the sense that a '1' means free, and '0' means
	 * allocated. */
	lamp_leff1_free_all ();
	lamp_leff2_free_all ();

	lamp_flash_max = lamp_flash_count = LAMP_DEFAULT_FLASH_RATE;
	//// lamp_apply_delay = 0;

	lamp_strobe_mask = 0x1;
	lamp_strobe_column = 0;
}


void lamp_update_all (void)
{
	callset_invoke (update_lamps);
}


/* Runs periodically to invert any lamps in the flashing state */
void lamp_flash_rtt (void)
{
	--lamp_flash_count;
	if (lamp_flash_count == 0)
	{
		U16 *lamp_matrix_words = (U16 *)lamp_flash_matrix_now;
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

	/* Setup the strobe */
#if 0
	wpc_asic_write (WPC_LAMP_ROW_OUTPUT, 0);
#endif
	wpc_asic_write (WPC_LAMP_COL_STROBE, lamp_strobe_mask);

	/* Grab the default lamp values */
	bits = lamp_matrix[lamp_strobe_column];

	/* OR in the flashing lamp values.  These are guaranteed to be
	 * zero for any lamps where the flash is turned off.
	 * Otherwise, these bits are periodically inverted by the
	 * (slower) flash rtt function above. */
	bits |= lamp_flash_matrix_now[lamp_strobe_column];

	/* Override with the lamp effect lamps.
	 * Leff2 bits are low priority and used for long-running
	 * lamp effects.  Leff1 is higher priority and used
	 * for quick effects.
	 */
	bits &= lamp_leff2_allocated[lamp_strobe_column];
	bits |= lamp_leff2_matrix[lamp_strobe_column];
	bits &= lamp_leff1_allocated[lamp_strobe_column];
	bits |= lamp_leff1_matrix[lamp_strobe_column];

	/* Write the result to the hardware */
	wpc_asic_write (WPC_LAMP_ROW_OUTPUT, bits);

	/* Advance strobe to next position for next iteration */
	lamp_strobe_column++;
	lamp_strobe_column &= 7;

	/* TODO - a rotate here would be much smarter */
#if 1
	lamp_strobe_mask <<= 1;
	if (lamp_strobe_mask == 0)
	{
		/* All columns strobed : reset strobe */
		lamp_strobe_mask = 0x1;
	}
#else
	asm __volatile__ ("rol\t%0" :: "g" (lamp_strobe_mask));
	asm __volatile__ ("rol\t%0" :: "g" (lamp_strobe_mask));
#endif
}


/*
 * Basic lamp manipulation routines
 *
 * These control the main lamp matrix and also the flash
 * matrix.  They can be modified at any time without the
 * need for allocation.
 *
 */

void lamp_on (lampnum_t lamp)
{
	register bitset p = lamp_matrix;
	register U8 v = lamp;
	__setbit(p, v);
}


void lamp_off (lampnum_t lamp)
{
	register bitset p = lamp_matrix;
	register U8 v = lamp;
	__clearbit(p, v);
}


void lamp_toggle (lampnum_t lamp)
{
	register bitset p = lamp_matrix;
	register U8 v = lamp;
	__togglebit(p, v);
}


int lamp_test (lampnum_t lamp)
{
	register bitset p = lamp_matrix;
	register U8 v = lamp;
	__testbit(p, v);
	return v;
}


void lamp_flash_on (lampnum_t lamp)
{
	register bitset p = lamp_flash_matrix;
	register U8 v = lamp;
	__setbit(p, v);
}


void lamp_flash_off (lampnum_t lamp)
{
	register bitset p = lamp_flash_matrix;
	register U8 v = lamp;
	__clearbit(p, v);

	p = lamp_flash_matrix_now;
	v = lamp;
	__clearbit(p, v);
}


int lamp_flash_test (lampnum_t lamp)
{
	register bitset p = lamp_flash_matrix;
	register U8 v = lamp;
	__testbit(p, v);
	return v;
}

void lamp_global_update ()
{
	callset_invoke (lamp_refresh);
	if (!in_test)
		lamp_start_update ();
}


/*
 * lamp_all_on / lamp_all_off are optimized and should be used
 * if all lamps are affected, rather than setting them one at
 * a time.
 */

void lamp_all_on (void)
{
	disable_interrupts ();
	memset (lamp_flash_matrix, 0, 3 * NUM_LAMP_COLS);
	memset (lamp_matrix, 0xff, NUM_LAMP_COLS);
	enable_interrupts ();
	lamp_global_update ();
}


void lamp_all_off (void)
{
	disable_interrupts ();
	memset (lamp_flash_matrix, 0, 3 * NUM_LAMP_COLS);
	memset (lamp_matrix, 0, NUM_LAMP_COLS);
	enable_interrupts ();
	lamp_global_update ();
}

/*
 * Lamp effect allocation/free functions.  These are called
 * during leff creation/shutdown time to override some of the
 * lamp bits for use by the lamp effect routine.
 *
 * They are not called from the context of the lamp effect
 * itself, but rather from the context in which it is
 * started/stopped.
 */

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
	memset (lamp_leff2_matrix, 0, NUM_LAMP_COLS);
}

void lamp_leff2_free_all (void)
{
	memset (lamp_leff2_allocated, 0xFF, NUM_LAMP_COLS);
}


void lamp_leff_allocate (lampnum_t lamp)
{
	register bitset p = lamp_leff1_allocated;
	register U8 v = lamp;
	__clearbit(p, v);
}


void lamp_leff_free (lampnum_t lamp)
{
	register bitset p = lamp_leff1_allocated;
	register U8 v = lamp;
	__setbit(p, v);
}


void lamp_leff2_allocate (lampnum_t lamp)
{
	register bitset p = lamp_leff2_allocated;
	register U8 v = lamp;
	__clearbit(p, v);
}


void lamp_leff2_free (lampnum_t lamp)
{
	register bitset p = lamp_leff2_allocated;
	register U8 v = lamp;
	__setbit(p, v);
}


/*
 * The leff_ functions below are used to set/clear/toggle/test
 * bits from a lamp effect function.  Otherwise they work
 * identically to the lamp_ versions.
 *
 * TODO  : these functions are hardcoding usage of the
 * leff1 matrix now.  These should be checking the leff
 * flags to see if it's a partial leff and use leff2 matrix
 * instead.
 */
void leff_on (lampnum_t lamp)
{
	register bitset p = (leff_running_flags & L_SHARED) ? 
		lamp_leff2_matrix : lamp_leff1_matrix;
	register U8 v = lamp;
	__setbit(p, v);
}


void leff_off (lampnum_t lamp)
{
	register bitset p = (leff_running_flags & L_SHARED) ? 
		lamp_leff2_matrix : lamp_leff1_matrix;
	register U8 v = lamp;
	__clearbit(p, v);
}


void leff_toggle (lampnum_t lamp)
{
	register bitset p = (leff_running_flags & L_SHARED) ? 
		lamp_leff2_matrix : lamp_leff1_matrix;
	register U8 v = lamp;
	__togglebit(p, v);
}


int leff_test (lampnum_t lamp)
{
	register bitset p = (leff_running_flags & L_SHARED) ? 
		lamp_leff2_matrix : lamp_leff1_matrix;
	register U8 v = lamp;
	__testbit(p, v);
	return v;
}


U8 *get_bit_base (void)
{
	return bit_matrix;
}


