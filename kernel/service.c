
#include <freewpc.h>


void sw_escape_button_handler (void) __taskentry__
{
	extern void test_escape_button (void);

	if (!in_test)
		add_credit ();
	else
		test_escape_button ();

	task_exit ();
}

void sw_down_button_handler (void) __taskentry__
{
	extern void test_down_button (void);

	test_down_button ();

	if (!in_test)
		volume_down ();

	task_exit ();
}

void sw_up_button_handler (void) __taskentry__
{
	extern void test_up_button (void);

	test_up_button ();
	
	if (!in_test)
		volume_up ();

	task_exit ();
}

void sw_enter_button_handler (void) __taskentry__
{
	extern void test_enter_button (void);

	test_enter_button ();
	task_exit ();
}


DECLARE_SWITCH_DRIVER (sw_escape_button)
{
	.fn = sw_escape_button_handler,
};

DECLARE_SWITCH_DRIVER (sw_down_button)
{
	.fn = sw_down_button_handler,
};

DECLARE_SWITCH_DRIVER (sw_up_button)
{
	.fn = sw_up_button_handler,
};

DECLARE_SWITCH_DRIVER (sw_enter_button)
{
	.fn = sw_enter_button_handler,
};

