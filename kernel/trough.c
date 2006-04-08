
/**
 * \file
 * \brief Common trough logic
 */

#include <freewpc.h>
#include <mach/switch.h>
#include <mach/coil.h>


DECLARE_SWITCH_DRIVER (sw_trough)
{
	.devno = SW_DEVICE_DECL(DEV_TROUGH),
};


#ifdef MACHINE_OUTHOLE_SWITCH

void sw_outhole_handler (void)
{
	sol_on (SOL_OUTHOLE);
	task_sleep_sec (1);
	sol_off (SOL_OUTHOLE);
}

DECLARE_SWITCH_DRIVER (sw_outhole)
{
	.fn = sw_outhole_handler,
	.flags = SW_IN_TEST,
};

#endif

#if defined(MACHINE_LAUNCH_SWITCH) && defined(MACHINE_LAUNCH_SOLENOID) && defined(MACHINE_SHOOTER_SWITCH)

void sw_launch_handler (void)
{
	if (switch_poll (MACHINE_SHOOTER_SWITCH))
	{
		sol_pulse (MACHINE_LAUNCH_SOLENOID);
	}
}

DECLARE_SWITCH_DRIVER (sw_launch_button)
{
	.fn = sw_launch_handler,
	.flags = SW_IN_GAME,
};

#endif

void trough_enter (device_t *dev)
{
	device_remove_live ();
}


void trough_kick_attempt (device_t *dev)
{
	/* Wait for any conditions that should delay a trough
	 * kick.
	 *
	 * On autoplunging games, always wait for the plunger
	 * area to clear.
	 */
}


void trough_kick_success (device_t *dev)
{
	device_add_live ();
}

void trough_full (device_t *dev)
{
	db_puts ("Trough is full!\n");
}


device_ops_t trough_ops = {
	.enter = trough_enter,
	.kick_attempt = trough_kick_attempt,
	.kick_success = trough_kick_success,
	.full = trough_full,
};

device_properties_t trough_props = {
	.ops = &trough_ops,
	.name = "TROUGH",
	.sol = MACHINE_BALL_SERVE_SOLENOID,
	.sw_count = MACHINE_TROUGH_SIZE,
	.init_max_count = MACHINE_TROUGH_SIZE,
	.sw = {
#ifdef MACHINE_TROUGH1
		MACHINE_TROUGH1,
#endif
#ifdef MACHINE_TROUGH2
		MACHINE_TROUGH2,
#endif
#ifdef MACHINE_TROUGH3
		MACHINE_TROUGH3,
#endif
#ifdef MACHINE_TROUGH4
		MACHINE_TROUGH4,
#endif
#ifdef MACHINE_TROUGH5
		MACHINE_TROUGH5,
#endif
	}
};


void trough_init (void)
{
	device_register (DEV_TROUGH, &trough_props);
}

