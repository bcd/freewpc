
#include <freewpc.h>


void autofire_launch_proc (void) __taskentry__
{
	extern void autofire_add_ball (void);

	autofire_add_ball ();
	task_exit ();
}

void autofire_to_gumball_proc (void) __taskentry__
{
	extern void gumball_load_from_trough (void);

	gumball_load_from_trough ();
	task_exit ();
}


void release_gumball_proc (void) __taskentry__
{
	extern void gumball_release (void);

	gumball_release ();
	task_exit ();
}

