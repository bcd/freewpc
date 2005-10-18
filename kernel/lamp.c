
#include <freewpc.h>



__fastram__ uint8_t lamp_matrix[NUM_LAMP_COLS];
__fastram__ uint8_t lamp_flash_allocated[NUM_LAMP_COLS];
__fastram__ uint8_t lamp_flash_matrix[NUM_LAMP_COLS];
__fastram__ uint8_t lamp_leff_allocated[NUM_LAMP_COLS];
__fastram__ uint8_t lamp_leff_matrix[NUM_LAMP_COLS];

uint8_t lamp_flash_max;
uint8_t lamp_flash_count;
uint8_t lamp_apply_delay;


void lamp_task (void)
{
	for (;;)
	{
		memset (lamp_flash_matrix, 0, NUM_LAMP_COLS);
		task_sleep (TIME_100MS * 2);
		memset (lamp_flash_matrix, 0xFF, NUM_LAMP_COLS);
		task_sleep (TIME_100MS * 2);
	}
}


void lamp_init (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS * 5);

	lamp_flash_max = lamp_flash_count = LAMP_DEFAULT_FLASH_RATE;
	lamp_apply_delay = 0;

	task_create_gid (GID_LAMP_UPDATE, lamp_task);
}


void lamp_rtt (void)
{
	extern uint8_t irq_count, irq_shift_count;
	uint8_t bits;

	/* Load the lamp strobe register */
	*(uint8_t *)WPC_LAMP_COL_STROBE = irq_shift_count;

	/* Grab the default lamp values */
	bits = lamp_matrix[irq_count % 8];

	/* Override with the flashing lamps */
	bits &= ~lamp_flash_allocated[irq_count % 8];
	bits |= lamp_flash_matrix[irq_count % 8];

	/* Override with the lamp effect lamps */
	bits &= ~lamp_leff_allocated[irq_count % 8];
	bits |= lamp_leff_matrix[irq_count % 8];

	/* Write the result to the hardware */
	*(uint8_t *)WPC_LAMP_ROW_OUTPUT = bits;
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
	register bitset p = lamp_flash_allocated;
	register uint8_t v = lamp;
	__setbit(p, v);
}


void lamp_noflash (lampnum_t lamp)
{
	register bitset p = lamp_flash_allocated;
	register uint8_t v = lamp;
	__clearbit(p, v);
}


void lamp_all_on (void)
{
	memset (lamp_matrix, 0xff, NUM_LAMP_COLS);
}


void lamp_all_off (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS);
}


