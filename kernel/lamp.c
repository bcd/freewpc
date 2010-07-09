/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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
 *
 * Theory of operation:
 *
 * The WPC lamp circuit is setup as an 8x8 matrix.  A column strobe output
 * selects which group of 8 lamps can be on at any time.  A separate row
 * output register contains the on/off values for each of the 8 lamps in
 * that column.
 *
 * At IRQ time, the column strobe is repeatedly switched among the 8
 * columns, at a rate of every 2ms.  Thus, the effective time to redraw
 * the entire lamp matrix is 16ms.
 *
 * The lowest level function calculates the row output by overlaying several
 * different values.  There are 3 different states for each lamp: the
 * default state, the secondary state, and the lightshow state -- in that
 * order.  It is possible that there is no secondary/lightshow associated
 * with a lamp, in which case those values are not applied to the output.
 * So for each those, a lamp must be allocated/freed.
 *
 * In mathematical form, the row output is defined as:
 *
 * 	(((default & secondary_mask) | secondary) & lightshow_mask) | lightshow
 *
 * A mask bit of 0 discards the previous state of the lamp, and allows the
 * new state, either 0 or 1 to come through.
 *
 * On top of this, the default state of a lamp can be set to 'flashing'.
 * A separate bit matrix tracks all lamps that are in flashing mode.
 * A separate realtime function, which runs much more slowly, toggles the
 * default lamp states to create the flashing.
 *
 * There are many boolean functions defined for testing lamp states as well
 * as setting them.  In simple cases, the default lamps can be used as
 * boolean variables for indicating game state.
 *
 * Default lamp values are also tracked per-player.  They are guaranteed to be
 * all off at the start of a player's game, and they are saved/restored in
 * multiplayer games.  The secondary/lightshow values are transient and are not
 * saved this way.
 *
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

U8 bit_matrix[BITS_TO_BYTES (MAX_FLAGS)];

U8 global_bits[BITS_TO_BYTES (MAX_GLOBAL_FLAGS)];

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


/** Runs periodically to invert any lamps in the flashing state.
 * (This is hard realtime now; it could probably be dropped in
 * priority, though.) */
void lamp_flash_rtt (void)
{
	U16 *lamp_matrix_words = (U16 *)lamp_flash_matrix_now;
	U16 *lamp_flash_matrix_words = (U16 *)lamp_flash_matrix;

	lamp_matrix_words[0] ^= lamp_flash_matrix_words[0];
	lamp_matrix_words[1] ^= lamp_flash_matrix_words[1];
	lamp_matrix_words[2] ^= lamp_flash_matrix_words[2];
	lamp_matrix_words[3] ^= lamp_flash_matrix_words[3];
}


/** Runs periodically to update the physical lamp state. */
void lamp_rtt (void)
{
	U8 bits;
	/* TODO : implement lamp power saver level.  For some number N
	 * iterations, just clear the lamp outputs and be done.
	 * But only do this outside of a game. */

	/* Turn off the lamp circuits before recalculating.  But don't
	do this in native mode, because the simulator doesn't simulate
	well-enough. */
#ifndef CONFIG_NATIVE
#ifdef __m6809__
	/* On the 6809, avoid using the CLR instruction which is known to cause
	problems in the WPC ASIC.   Also, always write ROW first to avoid
	spurious lamps. */
	__asm__ volatile ("clrb");
	__asm__ volatile ("stb\t" C_STRING (WPC_LAMP_ROW_OUTPUT));
	__asm__ volatile ("stb\t" C_STRING (WPC_LAMP_COL_STROBE));
#else
	pinio_write_lamp_data (0);
	pinio_write_lamp_strobe (0);
#endif /* __m6809__ */
#endif /* CONFIG_NATIVE */

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
	pinio_write_lamp_data (bits);
	pinio_write_lamp_strobe (lamp_strobe_mask);

	/* Advance the strobe value for the next iteration.
	Keep this together with the above so that lamp_strobe_mask
	is already in a register. */
	lamp_strobe_mask <<= 1;
	if (lamp_strobe_mask == 0)
	{
		/* All columns strobed : reset strobe */
		lamp_strobe_mask++;
		lamp_strobe_column = 0;
	}
	else
	{
		/* Advance strobe to next position for next iteration */
		lamp_strobe_column++;
	}
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

bool bit_test (const_bitset matrix, U8 bit)
{
	return bitarray_test (matrix, bit);
}


/** Return nonzero if all bits in a matrix are set. */
bool bit_test_all_on (const_bitset matrix)
{
	U8 product = (matrix[0] & matrix[1]
		& matrix[2] & matrix[3]
		& matrix[4] & matrix[5]
		& matrix[6] & matrix[7]);
	return (product == 0xFF);
}


/** Return nonzero if all bits in a matrix are clear. */
bool bit_test_all_off (const_bitset matrix)
{
	U8 sum = (matrix[0] | matrix[1]
		| matrix[2] | matrix[3]
		| matrix[4] | matrix[5]
		| matrix[6] | matrix[7]);
	return (sum == 0);
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
	/* Note: gcc6809 generates inefficient code for this.
	This would be better:
	   if (rc) return (0) else return (rc);
	gcc is insisting on returning 0/1, when zero/nonzero is
	sufficient.
    */
#if 0
	bool t = bit_test (lamp_matrix, lamp);
	return t ? 0 : t;
#endif
	return !bit_test (lamp_matrix, lamp);
}


void lamp_flash_on (lampnum_t lamp)
{
	if (!bit_test (lamp_flash_matrix, lamp))
	{
		/* Enable flashing on this lamp */
		bit_on (lamp_flash_matrix, lamp);

		/* Set the initial flash state of the lamp to match that of all
		other lamps that are flashing.  If any of the flashing lamps
		are on now, then this one should be on, too.  Otherwise, leave
		it off. */
		disable_interrupts ();
		if (!bit_test_all_off (lamp_flash_matrix_now))
			bit_on (lamp_flash_matrix_now, lamp);
		else
			bit_off (lamp_flash_matrix_now, lamp);
		enable_interrupts ();
	}
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


/*
 * lamp_all_on / lamp_all_off are optimized and should be used
 * if all lamps are affected, rather than setting them one at
 * a time.
 */

void lamp_all_on (void)
{
	disable_interrupts ();
	matrix_all_off (lamp_flash_matrix);
	enable_interrupts ();
	matrix_all_on (lamp_matrix);
}


void lamp_all_off (void)
{
	disable_interrupts ();
	matrix_all_off (lamp_flash_matrix_now);
	matrix_all_off (lamp_flash_matrix);
	matrix_all_off (lamp_leff1_matrix);
	matrix_all_off (lamp_leff2_matrix);
	enable_interrupts ();
	matrix_all_off (lamp_matrix);
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
	matrix_all_off (lamp_leff1_allocated);
}

void lamp_leff1_erase (void)
{
	matrix_all_off (lamp_leff1_matrix);
}

void lamp_leff1_free_all (void)
{	
	matrix_all_on (lamp_leff1_allocated);
}

void lamp_leff2_erase (void)
{
	matrix_all_off (lamp_leff2_matrix);
}

void lamp_leff2_free_all (void)
{
	matrix_all_on (lamp_leff2_allocated);
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

bool lamp_leff2_test_and_allocate (lampnum_t lamp)
{
	bool ok;

	disable_interrupts ();
	ok = bit_test (lamp_leff2_allocated, lamp);
	if (ok)
		lamp_leff2_allocate (lamp);
	enable_interrupts ();
	return ok;
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

