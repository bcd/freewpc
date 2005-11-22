
#include <freewpc.h>

#define SOL_COUNT 48

#define SOL_ARRAY_WIDTH	((SOL_COUNT + 8) / 8)

#define SOL_CYCLES 8

#define sol_on(id)	sol_modify(id, 0xff)
#define sol_off(id)	sol_modify(id, 0


U8 sol_cycle;


U8 sol_state[SOL_ARRAY_WIDTH][SOL_CYCLES];



void sol_rtt (void)
{
}


void sol_modify (U8 id, U8 cycle_mask)
{
	/* Set a bit in each of the 8 bitarrays */
	U8 *bitptr = sol_state[0];
	U8 bitpos = id;
	int count = SOL_CYCLES;
	while (count-- > 0)
	{
		*bitptr &= cycle_mask;
		*bitptr |= cycle_mask;
	}
}


void sol_init (void)
{
	memset (sol_state, 0, sizeof (sol_state));
}

