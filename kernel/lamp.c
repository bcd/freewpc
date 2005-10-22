
#include <freewpc.h>



__fastram__ uint8_t lamp_matrix[NUM_LAMP_COLS];
__fastram__ uint8_t lamp_flash_matrix[NUM_LAMP_COLS];
__fastram__ uint8_t lamp_leff_allocated[NUM_LAMP_COLS];
__fastram__ uint8_t lamp_leff_matrix[NUM_LAMP_COLS];

uint8_t lamp_flash_max;
uint8_t lamp_flash_count;
uint8_t lamp_apply_delay;
uint8_t lamp_flash_mask;

uint8_t lamp_strobe_mask;
uint8_t lamp_strobe_column;


void lamp_init (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS * 4);

	lamp_flash_max = lamp_flash_count = LAMP_DEFAULT_FLASH_RATE;
	lamp_apply_delay = 0;
	lamp_flash_mask = 0x00;

	lamp_strobe_mask = 0x1;
	lamp_strobe_column = 0;
}

void lamp_flash_rtt (void)
{
	--lamp_flash_count;
	if (lamp_flash_count == 0)
	{
		lamp_flash_mask = ~lamp_flash_mask;
		lamp_flash_count = lamp_flash_max;
	}
}


void lamp_rtt (void)
{
	uint8_t bits;
	uint8_t flashbits;

	/* Setup the strobe */
	*(uint8_t *)WPC_LAMP_ROW_OUTPUT = 0;
	*(uint8_t *)WPC_LAMP_COL_STROBE = lamp_strobe_mask;

	/* Grab the default lamp values */
	bits = lamp_matrix[lamp_strobe_column];

	/* Override with the flashing lamps */
	flashbits = lamp_flash_matrix[lamp_strobe_column];
	bits &= ~flashbits;
	bits |= (flashbits & lamp_flash_mask);

	/* Override with the lamp effect lamps */
	bits &= ~lamp_leff_allocated[lamp_strobe_column];
	bits |= lamp_leff_matrix[lamp_strobe_column];

	/* Write the result to the hardware */
	*(uint8_t *)WPC_LAMP_ROW_OUTPUT = bits;

	/* Advance strobe to next position for next iteration */
	lamp_strobe_column = (lamp_strobe_column+1) & 7;
	lamp_strobe_mask <<= 1;
	if (lamp_strobe_mask == 0)
		lamp_strobe_mask = 0x1;
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


void lamp_flash (lampnum_t lamp)
{
	register bitset p = lamp_flash_matrix;
	register uint8_t v = lamp;
	__setbit(p, v);
}


void lamp_noflash (lampnum_t lamp)
{
	register bitset p = lamp_flash_matrix;
	register uint8_t v = lamp;
	__clearbit(p, v);
}


void lamp_all_on (void)
{
	memset (lamp_matrix, 0xff, NUM_LAMP_COLS);
	memset (lamp_flash_matrix, 0, NUM_LAMP_COLS);
}


void lamp_all_off (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS);
	memset (lamp_flash_matrix, 0, NUM_LAMP_COLS);
}


