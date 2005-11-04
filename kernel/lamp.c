
#include <freewpc.h>

/* Lamps 00h through 3Fh correspond to the physical lamp locations */
__fastram__ U8 lamp_matrix[NUM_LAMP_COLS];

/* Lamps 40h through 7Fh are 'virtual' and don't really exist, but
 * they can be used to store boolean values.
 * These two bitsets are kept adjacent, so the same functions can
 * be used to bit-twiddle both of them.
 * Both of these bitsets are also saved from player to player.
 * Note the flash sets are NOT saved and must be recalculated from
 * ball to ball. */
__fastram__ U8 bit_matrix[NUM_VLAMP_COLS];

/* Lamps 80h through BFh are the same as the regular lamps, but
 * control the flash state */
__fastram__ U8 lamp_flash_matrix[NUM_LAMP_COLS];

/* Lamps C0h through FFh are similarly used to control the
 * 'fast flash' state of the lamp.  Fast flash overrides slow
 * flash, which overrides the steady value. */
__fastram__ U8 lamp_fast_flash_matrix[NUM_LAMP_COLS];

/* Bitsets for doing temporary lamp effects, which hide the
 * normal state of the lamps */
__fastram__ U8 lamp_leff1_allocated[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff1_matrix[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff2_allocated[NUM_LAMP_COLS];
__fastram__ U8 lamp_leff2_matrix[NUM_LAMP_COLS];

U8 lamp_flash_max;
U8 lamp_flash_count;
U8 lamp_flash_mask;
U8 lamp_fast_flash_mask;

U8 lamp_apply_delay;

U8 lamp_strobe_mask;
U8 lamp_strobe_column;


void lamp_init (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS * 7);

	lamp_flash_max = lamp_flash_count = LAMP_DEFAULT_FLASH_RATE;
	lamp_apply_delay = 0;
	lamp_flash_mask = lamp_fast_flash_mask = 0x00;

	lamp_strobe_mask = 0x1;
	lamp_strobe_column = 0;
}

void lamp_flash_rtt (void)
{
	--lamp_flash_count;
	if (lamp_flash_count == 0)
	{
		lamp_fast_flash_mask = ~lamp_fast_flash_mask;
		if (lamp_fast_flash_mask == 0)
			lamp_flash_mask = ~lamp_flash_mask;
		lamp_flash_count = lamp_flash_max;
	}
}


void lamp_rtt (void)
{
	static U8 bits;
	static U8 flashbits;

	/* Setup the strobe */
	*(U8 *)WPC_LAMP_ROW_OUTPUT = 0;
	*(U8 *)WPC_LAMP_COL_STROBE = lamp_strobe_mask;

	/* Grab the default lamp values */
	bits = lamp_matrix[lamp_strobe_column];

	/* Override with the flashing lamps */
	flashbits = lamp_flash_matrix[lamp_strobe_column];
	bits &= ~flashbits;
	bits |= (flashbits & lamp_flash_mask);

	/* Override with the fast flashing lamps */
	flashbits = lamp_fast_flash_matrix[lamp_strobe_column];
	bits &= ~flashbits;
	bits |= (flashbits & lamp_fast_flash_mask);

	/* Override with the lamp effect lamps */
	bits &= ~lamp_leff1_allocated[lamp_strobe_column];
	bits |= lamp_leff1_matrix[lamp_strobe_column];
	bits &= ~lamp_leff2_allocated[lamp_strobe_column];
	bits |= lamp_leff2_matrix[lamp_strobe_column];

	/* Write the result to the hardware */
	*(U8 *)WPC_LAMP_ROW_OUTPUT = bits;

	/* Advance strobe to next position for next iteration */
	lamp_strobe_column = (lamp_strobe_column+1) & 7;
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


void lamp_all_on (void)
{
	memset (lamp_matrix, 0xff, NUM_LAMP_COLS);
	memset (lamp_flash_matrix, 0, 2 * NUM_LAMP_COLS);
}


void lamp_all_off (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS);
	memset (lamp_flash_matrix, 0, 2 * NUM_LAMP_COLS);
}


