
#include <freewpc.h>
#include <trunk_motor.h>
#include <diag.h>

#define TRUNK_POS_WALL 1
#define TRUNK_POS_HOLE 2
#define TRUNK_POS_LAMP 3
#define TRUNK_POS_MAGNET 4

U8 trunk_target_pos;
U8 trunk_curr_pos;
U8 trunk_time_since_last_sw;

/* TBD - timeout if switches not working */

CALLSET_ENTRY (trunk_drv, idle_every_100ms)
{
	if (trunk_time_since_last_sw)
	{
		if (--trunk_time_since_last_sw == 0)
		{
			trunk_motor_stop ();
			global_flag_on (GLOBAL_FLAG_TRUNK_ERROR);
		}
	}
}

void trunk_reset (void)
{
	global_flag_off (GLOBAL_FLAG_TRUNK_ERROR);
	trunk_time_since_last_sw = 0;
	trunk_curr_pos = 0;
}

void trunk_update (void)
{
	if (trunk_target_pos == 0)
		return;

	if (trunk_target_pos == trunk_curr_pos)
	{
		trunk_motor_stop ();
		return;
	}

	/* TBD - pause game timers while trunk moves */
	trunk_time_since_last_sw = 15;
	if (trunk_target_pos < trunk_curr_pos)
	{
		trunk_motor_start_forward ();
	}
	else
	{
		trunk_motor_start_reverse ();
	}
}

void trunk_move (U8 pos)
{
	if (global_flag_test (GLOBAL_FLAG_TRUNK_ERROR))
	{
		trunk_target_pos = 0;
	}
	else
	{
		trunk_target_pos = pos;
		trunk_update ();
	}
}

CALLSET_ENTRY (trunk_drv, sw_cube_pos_1)
{
	trunk_curr_pos = 0x1;
	trunk_update ();
}

CALLSET_ENTRY (trunk_drv, sw_cube_pos_2)
{
	trunk_curr_pos = 0x2;
	trunk_update ();
}

CALLSET_ENTRY (trunk_drv, sw_cube_pos_3)
{
	trunk_curr_pos = 0x3;
	trunk_update ();
}

CALLSET_ENTRY (trunk_drv, sw_cube_pos_4)
{
	trunk_curr_pos = 0x4;
	trunk_update ();
}

CALLSET_ENTRY (trunk_drv, init_complete)
{
	trunk_reset ();
}

CALLSET_ENTRY (trunk_drv, amode_start)
{
	trunk_move (TRUNK_POS_MAGNET);
}

CALLSET_ENTRY (trunk_drv, start_ball)
{
	/* update according to player game state */
	//trunk_move (TRUNK_POS_WALL);
	trunk_move (TRUNK_POS_HOLE);
}

CALLSET_ENTRY (trunk_drv, diagnostic_check)
{
	if (feature_config.disable_trunk)
		diag_post_error ("TRUNK DISABLED\nBY ADJUSTMENT\n", MACHINE_PAGE);
	if (global_flag_test (GLOBAL_FLAG_TRUNK_ERROR))
		diag_post_error ("TRUNK IS\nNOT WORKING\n", MACHINE_PAGE);
}

CALLSET_ENTRY (trunk_drv, ball_search)
{
}

