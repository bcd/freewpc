
#ifndef _TZ_DRIVER_H
#define _TZ_DRIVER_H

#include <rtsol.h>

extern inline void mpf_driver (void)
{
	extern U8 mpf_active;
	extern U8 rtsol_mpf_left;
	extern U8 rtsol_mpf_right;

	/* While the mini-playfield is 'active' during a game,
	pulse the magnets whenever the flipper buttons are
	pressed. */
	if (mpf_active && in_live_game)
	{
		rt_solenoid_update (&rtsol_mpf_left,
			SOL_MPF_LEFT_MAGNET, SW_L_L_FLIPPER_BUTTON, 32, 16);
	
		rt_solenoid_update (&rtsol_mpf_right,
			SOL_MPF_RIGHT_MAGNET, SW_L_R_FLIPPER_BUTTON, 32, 16);
	}
}

#endif /* _TZ_DRIVER_H */
