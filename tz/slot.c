
#include <freewpc.h>
#include <mach/switch.h>
#include <mach/coil.h>


void sw_slot_handler (void)
{
	// device_sw_handler (2);
}


DECLARE_SWITCH_DRIVER (sw_slot)
{
	.fn = sw_slot_handler,
	.devno = SW_DEVICE_DECL(2),
};

void slot_kick_sound (void)
{
	sound_send (SND_SLOT_KICKOUT_2);
	task_exit ();
}


void slot_enter (device_t *dev)
{
	task_kill_gid (GID_SKILL_SWITCH_TRIGGER);
	mark_ball_in_play ();

	if (task_kill_gid (GID_SLOT_DISABLED_BY_PIANO))
	{
		/* piano was recently hit, so ignore slot */
	}
	else
	{
		score_add_current_const (0x2500);
	}
}


void slot_kick_attempt (device_t *dev)
{
	db_puts ("Sending slot kick sound\n");
	if (in_game && !in_tilt)
	{
		sound_send (SND_SLOT_KICKOUT_1);
		flasher_pulse (FLASH_RAMP3_POWER_PAYOFF);
		task_sleep (TIME_100MS * 7);
		task_create_gid (0, slot_kick_sound);
	}
}


CALLSET_ENTRY(slot, start_ball)
{
}


device_ops_t slot_ops = {
	.enter = slot_enter,
	.kick_attempt = slot_kick_attempt,
};

device_properties_t slot_props = {
	.ops = &slot_ops,
	.name = "SLOT KICKOUT",
	.sol = SOL_SLOT,
	.sw_count = 1,
	.init_max_count = 0,
	.sw = { SW_SLOT },
};


CALLSET_ENTRY (slot, init)
{
	device_register (2, &slot_props);
}

