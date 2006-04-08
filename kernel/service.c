
#include <freewpc.h>

/**
 * \file
 * \brief Handle the service button switches inside the coin door.
 */

void sw_escape_button_handler (void)
{
	extern void test_escape_button (void);

	if (!in_test)
		add_credit ();
	else
		call_far (TEST_PAGE, test_escape_button ());
}

void sw_down_button_handler (void)
{
	extern void test_down_button (void);

	call_far (TEST_PAGE, test_down_button ());

	if (!in_test)
		volume_down ();
}

void sw_up_button_handler (void)
{
	extern void test_up_button (void);

	call_far (TEST_PAGE, test_up_button ());
	
	if (!in_test)
		volume_up ();
}

void sw_enter_button_handler (void)
{
	extern void test_enter_button (void);

	call_far (TEST_PAGE, (test_enter_button ()));
}

/* Declare switch drivers for the coin door buttons */

DECLARE_SWITCH_DRIVER (sw_escape_button)
{
	.fn = sw_escape_button_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_down_button)
{
	.fn = sw_down_button_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_up_button)
{
	.fn = sw_up_button_handler,
	.flags = SW_IN_TEST,
};

DECLARE_SWITCH_DRIVER (sw_enter_button)
{
	.fn = sw_enter_button_handler,
	.flags = SW_IN_TEST,
};

