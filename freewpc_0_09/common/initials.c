
#include <freewpc.h>


U8 initials_enter_timer;



void initials_enter_init (void)
{
	initials_enter_timer = 30;
}


void initials_enter_draw (void)
{
}


void initials_enter_left (void)
{
}


void initials_enter_right (void)
{
}


void initials_enter_start (void)
{
}


struct window_ops initials_enter_window = {
	.init = initials_enter_init,
	.draw = initials_enter_draw,
	.left = initials_enter_left,
	.right = initials_enter_right,
	.start = initials_enter_start,
};


void initials_enter (void)
{
	initials_enter_timer = 30;
	window_push (&initials_enter_window);
}

