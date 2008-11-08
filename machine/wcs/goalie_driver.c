
#include <freewpc.h>


void stop_goalie (void)
{
	goalie_stop ();
}

void stop_goalie_left (void)
{
}

void stop_goalie_center (void)
{
}

void stop_goalie_right (void)
{
}

void start_goalie (void)
{
	if (feature_config.disable_goalie == NO)
	{
		goalie_start ();
	}
}


CALLSET_ENTRY (goalie_driver, start_ball)
{
	start_goalie ();
}

CALLSET_ENTRY (goalie_driver, end_ball)
{
	stop_goalie ();
}

CALLSET_ENTRY (goalie_driver, end_game)
{
	stop_goalie ();
}

