
#include <freewpc.h>


void sw_escape_button (void) __taskentry__
{
	extern void test_escape_button (void);

	test_escape_button ();

	if (!in_test)
		add_credit ();

	task_exit ();
}

void sw_down_button (void) __taskentry__
{
	extern void test_down_button (void);

	test_down_button ();

	if (!in_test)
		volume_down ();

	task_exit ();
}

void sw_up_button (void) __taskentry__
{
	extern void test_up_button (void);

	test_up_button ();
	
	if (!in_test)
		volume_down ();

	task_exit ();
}

void sw_enter_button (void) __taskentry__
{
	extern void test_enter_button (void);

	test_enter_button ();
	task_exit ();
}

