
#include <freewpc.h>



__fastram__ uint8_t lamp_matrix[NUM_LAMP_COLS];
__fastram__ uint8_t lamp_flash_select[NUM_LAMP_COLS];

uint8_t lamp_flash_max;
uint8_t lamp_flash_count;
uint8_t lamp_apply_delay;

void lamp_init (void)
{
	memset (lamp_matrix, 0, NUM_LAMP_COLS);
	memset (lamp_flash_select, 0, NUM_LAMP_COLS);
	lamp_flash_max = lamp_flash_count = LAMP_DEFAULT_FLASH_RATE;
	lamp_apply_delay = 0;
}

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
	register bitset p = lamp_flash_select;
	register uint8_t v = lamp;
	__setbit(p, v);
}


void lamp_noflash (lampnum_t lamp)
{
	register bitset p = lamp_flash_select;
	register uint8_t v = lamp;
	__clearbit(p, v);
}
