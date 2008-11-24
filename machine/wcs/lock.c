
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

#ifdef BCD_WCS
		/* If the tail switch -- the next one that would
		 * become permanently closed should a ball enter
		 * the lock -- is not working, then consider this
		 * entry criteria for a virtual lock. */
		device_t *dev = device_entry (DEVNO_LOCK);
		if (device_empty_p (dev)) /* assume last switch broken */
		{
			device_add_virtual (dev);
		}
#endif
	}
}

CALLSET_ENTRY (lock_magnet, dev_lock_enter)
{
	lock_magnet_disable ();
}


CALLSET_ENTRY (lock_magnet, dev_lock_kick_attempt)
{
	free_timer_start (TIM_LOCK_KICKING, TIME_3S);
}


CALLSET_ENTRY (lock_magnet, sw_left_flipper_lane)
{
#ifdef BCD_WCS
	/* If the head switch -- the one that would open
	 * when a ball leaves the lock -- is not working
	 * and a ball was just ejected, then consider this
	 * a successful attempt.
	 */
	device_t *dev = device_entry (DEVNO_LOCK);
	if (free_timer_test (TIM_LOCK_KICKING) && (dev->actual_count == 1))
		/* assume last switch broken */
	{
		free_timer_stop (TIM_LOCK_KICKING);
		device_remove_virtual (dev);
	}
#endif
}


CALLSET_ENTRY (lock_magnet, start_game)
{
	lock_magnet_disable ();
}

CALLSET_ENTRY (lock_magnet, end_game)
{
	lock_magnet_disable ();
}
