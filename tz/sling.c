
#include <freewpc.h>
#include <rtsol.h>

U8 rtsol_lower_jet;

void slingshot_rtt (void)
{
	extern U8 *sol_state;

	rt_solenoid_update ((char *)&sol_state + (SOL_LOWER_JET / 8),
		1 << (SOL_LOWER_JET % 8),
		RTSOL_ACTIVE_HIGH,
		&switch_bits[0][0] + (SW_BOTTOM_JET / 8),
		1 << (SW_BOTTOM_JET % 8),
		RTSW_ACTIVE_HIGH,
		&rtsol_lower_jet,
		8,
		8);
}


DECLARE_SWITCH_DRIVER (sw_left_sling)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_SLINGSHOT,
};


DECLARE_SWITCH_DRIVER (sw_right_sling)
{
	.flags = SW_PLAYFIELD,
	.sound = SND_SLINGSHOT,
};

