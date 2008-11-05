
#include <freewpc.h>

void lock_magnet_enable (void)
{
	flag_on (FLAG_LOCK_MAGNET_ENABLE);
}

void lock_magnet_disable (void)
{
	flag_off (FLAG_LOCK_MAGNET_ENABLE);
}

CALLSET_ENTRY (lock_magnet, sw_r_ramp_exit)
{
	if (in_live_game && flag_test (FLAG_LOCK_MAGNET_ENABLE))
	{
		/* start the magnet catch for X ms */
	}
}

CALLSET_ENTRY (lock_magnet, dev_lock_enter)
{
	lock_magnet_disable ();
}

CALLSET_ENTRY (lock_magnet, start_game)
{
	lock_magnet_disable ();
}

CALLSET_ENTRY (lock_magnet, end_game)
{
	lock_magnet_disable ();
}
