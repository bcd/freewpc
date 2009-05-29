
#include <freewpc.h>

extern U8 initials_enter_timer;


/**
 * Handle the start button.  There should be only one callset entry for
 * this, because only one of the cases should be handled at any time.
 * Modules that want to hook the start button should declare a
 * start_button_handler and modify the logic here.
 */
CALLSET_ENTRY (start_button, sw_start_button)
{
	/* if (free_timer_test (TMR_IGNORE_START_BUTTON))
	{
	}
	else */ if (deff_get_active () == DEFF_SYSTEM_RESET)
	{
	}
	else if (in_test)
	{
		SECTION_VOIDCALL (__test__, test_mode_start_button_handler);
	}
	else if (initials_enter_timer)
	{
		SECTION_VOIDCALL (__common__, initials_start_button_handler);
	}
	else
	{
		VOIDCALL (game_start_button_handler);
	}
}

