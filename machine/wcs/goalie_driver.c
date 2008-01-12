
#include <freewpc.h>

static enum { RUNNING, STOPPING, LEFT_STOPPING, CENTER_STOPPING, RIGHT_STOPPING } goalie_mode;

void goalie_running (void)
{
	while (goalie_mode == RUNNING)
	{
		dbprintf ("Restarting goalie drive\n");
		sol_start (SOL_GOALIE_DRIVE, SOL_DUTY_25, TIME_1S);
		task_sleep (TIME_500MS);
	}

	dbprintf ("Goalie drive off\n");
	sol_stop (SOL_GOALIE_DRIVE);
	task_exit ();
}


void stop_goalie (void)
{
	dbprintf ("Stopping goalie\n");
	goalie_mode = STOPPING;
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
	dbprintf ("Starting goalie\n");
	goalie_mode = RUNNING;
	task_create_gid (GID_GOALIE_RUNNING, goalie_running);
}


CALLSET_ENTRY (goalie_driver, sw_left_button)
{
	if (!in_test)
		start_goalie ();
}

CALLSET_ENTRY (goalie_driver, sw_right_button)
{
	if (!in_test)
		stop_goalie ();
}
