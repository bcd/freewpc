
#include <freewpc.h>

/**
 * Initialize a lamp inside a lamp effect so that it starts out
 * with the opposite of the default value of the lamp.
 */
void leff_toggle_basic (U8 lampno)
{
	if (lamp_test (lampno))
		leff_off (lampno);
	else
		leff_on (lampno);
}


/**
 * Flash a lamp inside a lamp effect.
 */
void le_flash (U8 lampno, task_ticks_t delay)
{
	leff_toggle_basic (lampno);
	task_sleep (delay);
	leff_toggle (lampno);
	task_sleep (delay);
}


/**
 * Twitch a lamp.  200ms seems reasonable.
 */
void le_twitch (U8 lampno)
{
	le_flash (lampno, TIME_200MS);
}


/**
 * Flicker a lamp; this is just a repeated flashing at high
 * speed.
 */
void le_flicker (U8 lampno, task_ticks_t duration)
{
	while (duration > 0)
	{
		le_flash (lampno, TIME_33MS);
		duration -= TIME_66MS;
	}
}


/**
 * Allocate one lamp inside a lamp effect.  If the lamp cannot be
 * allocated, the task exits.
 */
void le_alloc_single (U8 lampno)
{
	if (!lamp_leff2_test_and_allocate (lampno))
		task_exit ();
}


/**
 * Free one lamp inside a lamp effect.
 */
void le_free_single (U8 lampno)
{
	lamp_leff2_free (lampno);
}


/**
 * Mark the current task as a lamp effect.
 */
void le_init (void)
{
	leff_data_t * const cdata = task_current_class_data (leff_data_t);
	cdata->flags = L_SHARED;
}


void le_example (void)
{
	le_init ();
#ifdef MACHINE_SHOOT_AGAIN_LAMP
	le_alloc_single (MACHINE_SHOOT_AGAIN_LAMP);
	le_flicker (MACHINE_SHOOT_AGAIN_LAMP, TIME_4S);
	le_free_single (MACHINE_SHOOT_AGAIN_LAMP);
#endif /* MACHINE_SHOOT_AGAIN_LAMP */
	task_exit ();
}

