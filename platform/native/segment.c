
#include <freewpc.h>
#include <simulation.h>

unsigned int sim_seg_column = 0;

U16 sim_seg_data[SEG_SECTIONS][SEG_SECTION_SIZE] = { 0, };


void sim_seg_set_column (unsigned int col)
{
	sim_seg_column = col & 0x0F;
}


void sim_seg_write (unsigned int section, unsigned int subword, unsigned int val)
{
	U16 mask;
	U16 prev;
	U16 *ptr = &sim_seg_data[section][sim_seg_column];
	prev = *ptr;

	if (subword == 0)
		mask = 0xFF00;
	else
	{
		mask = 0x00FF;
		val <<= 8;
	}

	val = (*ptr & mask) | val;

	if (val != prev)
	{
		ui_refresh_display (sim_seg_column, section, val);
		*ptr = val;
	}
}

