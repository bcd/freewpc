
#include <freewpc.h>

static enum {
	RUNNING,
	STOPPING,
	LEFT_STOPPING,
	CENTER_STOPPING,
	RIGHT_STOPPING
} goalie_mode;


void goalie_running (void)
{
	while (goalie_mode == RUNNING)
	{
		sol_start (SOL_GOALIE_DRIVE, SOL_DUTY_25, TIME_1S);
		task_sleep (TIME_500MS);
	}
	sol_stop (SOL_GOALIE_DRIVE);
	task_exit ();
}


void stop_goalie (void)
{
	goalie_mode = STOPPING;
	task_kill_gid (GID_GOALIE_RUNNING);
	sol_stop (SOL_GOALIE_DRIVE);
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
		goalie_mode = RUNNING;
		task_create_gid (GID_GOALIE_RUNNING, goalie_running);
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

