
#include <freewpc.h>
#include <window.h>
#include <test.h>

void switch_matrix_draw (void)
{
	U8 row, col;

	for (row=0; row < 8; row++)
	{
		for (col=0; col < 8; col++)
		{
			U8 sw = MAKE_SWITCH (col+1,row+1);
#if 0 /* whether or not it is an opto isn't important now */
			bool opto_p = switch_is_opto (sw);
#endif
			bool state_p = switch_poll (sw);
			register U8 *dmd = dmd_low_buffer +
				((U16)row << 6) + (col >> 1);
			U8 mask = (col & 1) ? 0x0E : 0xE0;

			/* TODO : use bitmap_draw for these */
			if (state_p)
			{
				dmd[0 * DMD_BYTE_WIDTH] |= mask;
				dmd[1 * DMD_BYTE_WIDTH] |= mask & ~0x44;
				dmd[2 * DMD_BYTE_WIDTH] |= mask;
			}
			else
			{
				dmd[0 * DMD_BYTE_WIDTH] &= ~mask;
				dmd[1 * DMD_BYTE_WIDTH] |= mask & 0x44;
				dmd[2 * DMD_BYTE_WIDTH] &= ~mask;
			}
		}
	}
}

void switch_edges_update (void)
{
	/* TODO : here's what needs to happen.
	We begin by drawing the switch matrix normally, then we
	take a snapshot of raw switches.  Every 16ms, we do
	a compare of the current raw switches vs. our snapshot.
	If the same, nothing to be done.  If different, save
	current as the new snapshot and redraw the switch matrix.
	Even better, we could only redraw the columns that changed.
	Also show the transition(s) that just occurred.
	(For switch levels, iterate through the active switches
	accounting for backwards optos continuously.) */
	switch_matrix_draw ();
}

void switch_levels_update (void)
{
	switch_matrix_draw ();
}

