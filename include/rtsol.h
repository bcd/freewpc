#ifndef _RTSOL_H
#define _RTSOL_H

/* Code for doing real-time solenoids.
 * These are handled directly by polling the switches and
 * then driving the appropriate solenoids.
 */

#define RTSOL_ACTIVE_HIGH	0xff
#define RTSOL_ACTIVE_LOW	0x0

#define RTSW_ACTIVE_HIGH	0xff
#define RTSW_ACTIVE_LOW		0x0


extern inline void rt_sol_off (U8 *sol_cache, U8 bitmask, U8 active_high)
{
	*sol_cache &= ~bitmask;
}

extern inline void rt_sol_on (U8 *sol_cache, U8 bitmask, U8 active_high)
{
	*sol_cache |= bitmask;
}

extern inline U8 rt_sol_active (U8 *sol_cache, U8 bitmask, U8 active_high)
{
	return (*sol_cache & bitmask);
}

/* Each rt solenoid is governed by a rt_sol_state object,
 * which tracks the current status of that solenoid.
 * The state is an 8-bit value divided into two parts:
 * 1-bit for whether the solenoid is in its on/off phase,
 * and 7-bits for the timer coutdown/countup.
 */
extern inline void rt_solenoid_update (
	U8 *sol_cache,
	const U8 sol_bitmask,
	const U8 sol_active_high,
	U8 *sw_cache,
	const U8 sw_bitmask,
	const U8 sw_active_high,
	S8 *rt_sol_state,
	const U8 sol_on_irqs,
	const U8 sol_off_irqs )
{
	if (*rt_sol_state < 0)
	{
		/* Solenoid is in its off-phase */
		rt_sol_off (sol_cache, sol_bitmask, sol_active_high);
		*rt_sol_state++;
	}
	else if (*rt_sol_state > 0)
	{
		/* Solenoid is in its on-phase */
		rt_sol_on (sol_cache, sol_bitmask, sol_active_high);
		*rt_sol_state--;
		if (*rt_sol_state == 0)
		{
			*rt_sol_state = -sol_off_irqs;
		}
	}
	else
	{
		/* Solenoid is idle */
		/* Only here are allowed to poll the switch */
		if (rt_sol_active (sw_cache, sw_bitmask, sw_active_high))
		{
			/* Yes, the switch is active, so the solenoid can
			 * be scheduled to activate now */
			*rt_sol_state = sol_on_irqs;
		}
	}
}

#endif /* _RTSOL_H */

/* vim: set ts=3: */
