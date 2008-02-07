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

/**
 * \file
 * \brief Lamp management functions
 */

#include <freewpc.h>
#include <coin.h>

__fastram__ U8 lamp_matrix[NUM_LAMP_COLS];

U8 lamp_flash_matrix[NUM_LAMP_COLS];

__fastram__ U8 lamp_flash_matrix_now[NUM_LAMP_COLS];

__fastram__ U8 lamp_leff1_matrix[NUM_LAMP_COLS];

__fastram__ U8 lamp_leff1_allocated[NUM_LAMP_COLS];

__fastram__ U8 lamp_leff2_matrix[NUM_LAMP_COLS];

__fastram__ U8 lamp_leff2_allocated[NUM_LAMP_COLS];

U8 bit_matrix[NUM_LAMP_COLS];

U8 global_bits[NUM_LAMP_COLS];

__fastram__ U8 lamp_strobe_mask;

__fastram__ U8 lamp_strobe_column;

#define NUM_LAMP_RTTS 4


/** Initialize the lamp subsystem at startup. */
void lamp_init (void)
{
	/* Clear all lamps/flags */
	matrix_all_off (lamp_matrix);
	matrix_all_off (lamp_flash_matrix);
	matrix_all_off (lamp_flash_matrix_now);
	matrix_all_off (lamp_leff1_matrix);
	matrix_all_off (lamp_leff2_matrix);
	matrix_all_off (bit_matrix);
	matrix_all_off (global_bits);

	/* Lamp effect allocation matrices are "backwards",
	 * in the sense that a '1' means free, and '0' means
	 * allocated. */
	lamp_leff1_free_all ();
	lamp_leff2_free_all ();

	lamp_strobe_mask = 0x1;
	lamp_strobe_column = 0;
}


/** Runs periodically to invert any lamps in the flashing state */
void lamp_flash_rtt (void)
{
	U16 *lamp_matrix_words = (U16 *)lamp_flash_matrix_now;
	U16 *lamp_flash_matrix_words = (U16 *)lamp_flash_matrix;

	lamp_matrix_words[0] ^= lamp_flash_matrix_words[0];
	lamp_matrix_words[1] ^= lamp_flash_matrix_words[1];
	lamp_matrix_words[2] ^= lamp_flash_matrix_words[2];
	lamp_matrix_words[3] ^= lamp_flash_matrix_words[3];
}


/** Runs periodically to update the physical lamp state.
 * MODE ranges from 0 .. NUM_LAMP_RTTS-1, and says which version
 * of the routine is needed, because of loop unrolling.
 * For efficiency, not all iterations need to do everything.
 */
extern inline void lamp_rtt_common (const U8 mode)
{
	U8 bits;
	/* TODO : implement lamp power saver level.  For some number N
	 * iterations, just clear the lamp outputs and be done.
	 * But only do this outside of a game. */

	/* Setup the strobe */
	wpc_asic_write (WPC_LAMP_ROW_OUTPUT, 0);
	wpc_asic_write (WPC_LAMP_COL_STROBE, lamp_strobe_mask);

	/* Advance the strobe value for the next iteration.
	Keep this together with the above so that lamp_strobe_mask
	is already in a register. */
	lamp_strobe_mask <<= 1;
	if (mode == NUM_LAMP_RTTS-1 && lamp_strobe_mask == 0)
	{
		/* All columns strobed : reset strobe */
		lamp_strobe_mask = 0x1;
	}

	/* Grab the default lamp values */
	bits = lamp_matrix[lamp_strobe_column];

	/* OR in the flashing lamp values.  These are guaranteed to be
	 * zero for any lamps where the flash is turned off.
	 * Otherwise, these bits are periodically inverted by the
	 * (slower) flash rtt function above.
	 * This means that for the flash to work, the default bit
	 * must be OFF when the flash bit is ON.  (Use the tristate
	 * macros to ensure this.)
	 */
	bits |= lamp_flash_matrix_now[lamp_strobe_column];

	/* TODO : implement lamp strobing, like the newer Stern games
	do.  Implement like DMD page flipping, alternating between 2
	different lamp matrices rapidly to present 4 different
	intensities.  A background task, like the flash_rtt above,
	would toggle the intensities at a slower rate. */

	/* Override with the lamp effect lamps.
	 * Leff2 bits are low priority and used for long-running
	 * lamp effects.  Leff1 is higher priority and used
	 * for quick effects.  Therefore leff2 is applied first,
	 * and leff1 may override it.
	 */
	bits &= lamp_leff2_allocated[lamp_strobe_column];
	bits |= lamp_leff2_matrix[lamp_strobe_column];
	bits &= lamp_leff1_allocated[lamp_strobe_column];
	bits |= lamp_leff1_matrix[lamp_strobe_column];

	/* Write the result to the hardware */
	wpc_asic_write (WPC_LAMP_ROW_OUTPUT, bits);

	/* Advance strobe to next position for next iteration */
	lamp_strobe_column++;
	if (mode == NUM_LAMP_RTTS-1)
		lamp_strobe_column &= 7;
}


void lamp_rtt_0 (void)
{
	lamp_rtt_common (0);
}


void lamp_rtt_1 (void)
{
	lamp_rtt_common (1);
}


void lamp_rtt_2 (void)
{
	lamp_rtt_common (2);
}


void lamp_rtt_3 (void)
{
	lamp_rtt_common (3);
}


/* Basic bit manipulation routines.  These will
always use the WPC shift hardware and may not be
optimal.  The bitarray macros are preferred. */

void bit_on (bitset matrix, U8 bit)
{
	bitarray_set (matrix, bit);
}

void bit_off (bitset matrix, U8 bit)
{
	bitarray_clear (matrix, bit);
}

void bit_toggle (bitset matrix, U8 bit)
{
	bitarray_toggle (matrix, bit);
}

bool bit_test (bitset matrix, U8 bit)
{
	return bitarray_test (matrix, bit);
}

bool bit_test_all_on (bitset matrix)
{
	return matrix[0] && matrix[1]
		&& matrix[2] && matrix[3]
		&& matrix[4] && matrix[5]
		&& matrix[6] && matrix[7];
}

bool bit_test_all_off (bitset matrix)
{
	return !matrix[0] && !matrix[1]
		&& !matrix[2] && !matrix[3]
		&& !matrix[4] && !matrix[5]
		&& !matrix[6] && !matrix[7];
}


__attribute__((pure)) U8 *matrix_lookup (lamp_matrix_id_t id)
{
	switch (id)
	{
		case LMX_DEFAULT:
			return lamp_matrix;
		case LMX_FLASH:
			return lamp_flash_matrix;
		case LMX_EFFECT1_ALLOC:
			return lamp_leff1_allocated;
		case LMX_EFFECT1_LAMPS:
			return lamp_leff1_matrix;
		case LMX_EFFECT2_ALLOC:
			return lamp_leff2_allocated;
		case LMX_EFFECT2_LAMPS:
			return lamp_leff2_matrix;
	}
	fatal (ERR_INVALID_MATRIX);
}

void matrix_all_on (bitset matrix)
{
	memset (matrix, 0xFF, NUM_LAMP_COLS);
}

void matrix_all_off (bitset matrix)
{
	memset (matrix, 0, NUM_LAMP_COLS);
}


/*
 * Lamp manipulation routines
 *
 * These control the main lamp matrix and also the flash
 * matrix.  They can be modified at any time without the
 * need for allocation.
 *
 */
void lamp_on (lampnum_t lamp)
{
	bit_on (lamp_matrix, lamp);
}

void lamp_off (lampnum_t lamp)
{
	bit_off (lamp_matrix, lamp);
}

void lamp_toggle (lampnum_t lamp)
{
	bit_toggle (lamp_matrix, lamp);
}

bool lamp_test (lampnum_t lamp)
{
	return bit_test (lamp_matrix, lamp);
}

bool lamp_test_off (lampnum_t lamp)
{ 
	return !bit_test (lamp_matrix, lamp);
}


void lamp_flash_on (lampnum_t lamp)
{
	bit_on (lamp_flash_matrix, lamp);
}

void lamp_flash_off (lampnum_t lamp)
{
	bit_off (lamp_flash_matrix, lamp); 
	bit_off (lamp_flash_matrix_now, lamp);
}

bool lamp_flash_test (lampnum_t lamp)
{
	return bit_test (lamp_flash_matrix, lamp);
}


void lamp_global_update (void)
{
	if (in_live_game)
		callset_invoke (lamp_update);
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
	matrix_all_off (lamp_flash_matrix);
	matrix_all_off (lamp_flash_matrix_now);
	enable_interrupts ();
	matrix_all_on (lamp_matrix);
	lamp_global_update ();
}


void lamp_all_off (void)
{
	disable_interrupts ();
	matrix_all_off (lamp_flash_matrix);
	matrix_all_off (lamp_flash_matrix_now);
	matrix_all_off (lamp_leff1_matrix);
	matrix_all_off (lamp_leff2_matrix);
	enable_interrupts ();
	matrix_all_off (lamp_matrix);
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
	bit_off (lamp_leff1_allocated, lamp);
}

void lamp_leff_free (lampnum_t lamp)
{ 
	bit_on (lamp_leff1_allocated, lamp);
}

void lamp_leff2_allocate (lampnum_t lamp)
{ 
	bit_off (lamp_leff2_matrix, lamp);
	bit_off (lamp_leff2_allocated, lamp);
}

bool lamp_leff2_test_allocated (lampnum_t lamp)
{	
	return bit_test (lamp_leff2_allocated, lamp);
}

void lamp_leff2_free (lampnum_t lamp)
{
	bit_on (lamp_leff2_allocated, lamp);
	bit_off (lamp_leff2_matrix, lamp);
}

/*
 * The leff_ functions below are used to set/clear/toggle/test
 * bits from a lamp effect function.  Otherwise they work
 * identically to the lamp_ versions.
 *
 * The functions manipulate either the leff1 or leff2 matrix,
 * depending on whether the leff is shared or not.  Quick leffs
 * used for light shows use leff1.  Shared leffs that run
 * longer and may overlap with other shared leffs use leff2.
 */
void leff_on (lampnum_t lamp)
{
	register bitset p = (leff_running_flags & L_SHARED) ? 
		lamp_leff2_matrix : lamp_leff1_matrix;
	bit_on (p, lamp);
}


void leff_off (lampnum_t lamp)
{
	register bitset p = (leff_running_flags & L_SHARED) ? 
		lamp_leff2_matrix : lamp_leff1_matrix;
	bit_off (p, lamp);
}


void leff_toggle (lampnum_t lamp)
{
	register bitset p = (leff_running_flags & L_SHARED) ? 
		lamp_leff2_matrix : lamp_leff1_matrix;
	bit_toggle (p, lamp);
}


bool leff_test (lampnum_t lamp)
{
	register bitset p = (leff_running_flags & L_SHARED) ? 
		lamp_leff2_matrix : lamp_leff1_matrix;
	return bit_test (p, lamp);
}

