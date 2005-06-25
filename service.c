
#include <wpc.h>
#include <sys/task.h>
#include <sys/sound.h>


void sw_escape_button (void) __taskentry__
{
	task_exit ();
}

void sw_down_button (void) __taskentry__
{
	task_exit ();
}

void sw_up_button (void) __taskentry__
{
	task_exit ();
}

void sw_enter_button (void) __taskentry__
{
	task_exit ();
}

