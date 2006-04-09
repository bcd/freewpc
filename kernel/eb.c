
#include <freewpc.h>

/**
 * \file
 * \brief Common logic for dealing with extra balls
 *
 * This module implements the standard state machine of a pinball game.
 */


/** Defined in kernel/game.c */
U8 extra_balls;


void update_extra_ball_lamp (void)
{
#ifdef MACHINE_EXTRA_BALL_LAMP
#endif /* MACHINE_EXTRA_BALL_LAMP */
}


void increment_extra_balls (void)
{
	if (extra_balls < system_config.max_ebs_per_bip)
	{
		extra_balls++;
		update_extra_ball_lamp ();
	}
}


bool decrement_extra_balls (void)
{
	if (extra_balls > 0)
	{
		extra_balls--;
		update_extra_ball_lamp ();
		return (TRUE);
	}
	else
		return (FALSE);
}


void clear_extra_balls (void)
{
	extra_balls = 0;
}

