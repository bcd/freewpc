
#include <freewpc.h>
#include <simulation.h>

int show_switch_levels = 0;

/** The simulated switch matrix inputs.  This acts as a buffer between the
 * simulation code and the actual product code, but it serves the same 
 * purpose.  Only one matrix is needed, however. 
 */
U8 linux_switch_matrix[SWITCH_BITS_SIZE+1];



U8 *sim_switch_matrix_get (void)
{
	return linux_switch_matrix;
}


void sim_switch_toggle (int sw)
{
	U8 level;

	if (sim_test_badness (SIM_BAD_NOSWITCHPOWER))
		return;

	/* Update the current state of the switch */
	linux_switch_matrix[sw / 8] ^= (1 << (sw % 8));

	/* Redraw the switch */
	level = linux_switch_matrix[sw/8] & (1 << (sw%8));
#ifdef CONFIG_UI
	if (show_switch_levels)
		ui_write_switch (sw, level);
	else
		ui_write_switch (sw, level ^ switch_is_opto (sw));
#endif

	/* Some switch closures require additional simulation... */
	if (level ^ switch_is_opto (sw))
		sim_switch_effects (sw);
}


int sim_switch_read (int sw)
{
	return linux_switch_matrix[sw/8] & (1 << (sw%8));
}


void sim_switch_init (void)
{
	memset (linux_switch_matrix, 0, SWITCH_BITS_SIZE);
	linux_switch_matrix[9] = 0xFF;
}

