
#include <freewpc.h>
#include <trunk_motor.h>

#define TRUNK_POS_WALL 1
#define TRUNK_POS_HOLE 2
#define TRUNK_POS_LAMP 3
#define TRUNK_POS_MAGNET 4

U8 trunk_target_pos;
U8 trunk_curr_pos;

void trunk_update (void)
{
	if (trunk_target_pos == 0)
		return;

	if (trunk_target_pos == trunk_curr_pos)
	{
		trunk_motor_stop ();
		return;
	}
	else if (trunk_target_pos < trunk_curr_pos)
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
	trunk_target_pos = pos;
	trunk_update ();
}

CALLSET_ENTRY (trunk, sw_cube_pos_1)
{
	trunk_curr_pos = 0x1;
	trunk_update ();
}

CALLSET_ENTRY (trunk, sw_cube_pos_2)
{
	trunk_curr_pos = 0x2;
	trunk_update ();
}

CALLSET_ENTRY (trunk, sw_cube_pos_3)
{
	trunk_curr_pos = 0x3;
	trunk_update ();
}

CALLSET_ENTRY (trunk, sw_cube_pos_4)
{
	trunk_curr_pos = 0x4;
	trunk_update ();
}

CALLSET_ENTRY (trunk, init_complete)
{
	trunk_curr_pos = 0;
}

CALLSET_ENTRY (trunk, amode_start)
{
	trunk_move (TRUNK_POS_MAGNET);
}

CALLSET_ENTRY (trunk, start_ball)
{
	/* update according to player game state */
	//trunk_move (TRUNK_POS_WALL);
	trunk_move (TRUNK_POS_HOLE);
}


