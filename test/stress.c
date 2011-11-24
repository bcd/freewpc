/*
 * Copyright 2009, 2010, 2011 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>

#ifndef CONFIG_STRESS_DRAIN_PROB
#define CONFIG_STRESS_DRAIN_PROB 30
#endif

U8 switch_stress_enable;

/**
 * Pulse the flippers periodically during stress test.
 */
static void switch_stress_flipper_task (void)
{
	for (;;)
	{
		task_sleep (TIME_1500MS);
#if (MACHINE_FLIPTRONIC == 1)
		flipper_override_pulse (WPC_LL_FLIP_SW);
#endif
		task_sleep (TIME_1500MS);
#if (MACHINE_FLIPTRONIC == 1)
		flipper_override_pulse (WPC_LR_FLIP_SW);
#endif
		task_sleep (TIME_500MS);
#ifdef DEBUGGER
		db_dump_all ();
#endif
	}
}


/**
 * Simulate a ball drain condition.
 * This is simulated occasionally during multiball play, and can be
 * forced by pressing the Start Button.  The manual approach is the
 * only way to end a ball, otherwise the simulation goes on forever.
 */
void switch_stress_drain (void)
{
#ifdef DEVNO_TROUGH
	device_t *dev = device_entry (DEVNO_TROUGH);
	device_call_op (dev, enter);
#endif
}


/**
 * This task runs during a switch stress test.  During a game,
 * it randomly invokes switch handlers as if the switches had actually
 * been activated by the pinball.
 */
void switch_stress_task (void)
{
	U8 sw;
	const switch_info_t *swinfo;
	task_pid_t tp;

	/* Delay a few seconds before starting the simulation.  This allows
	time for the Start Button to be used to add players, instead of simulating
	endball. */
	task_sleep_sec (3);

	task_create_peer (switch_stress_flipper_task);
	for (;;)
	{
		task_sleep (TIME_100MS);
		if (in_test)
			continue;

		/* Choose a switch at random.  Skip certain well-known switches that are
		 * never to be activated. */
		sw = random_scaled (NUM_SWITCHES);
#ifdef SW_ALWAYS_CLOSED
		if (sw == SW_ALWAYS_CLOSED)
			continue;
#endif
#ifdef MACHINE_OUTHOLE_SWITCH
		if (sw == MACHINE_OUTHOLE_SWITCH)
			continue;
#endif

		/* Lookup the switch properties.  Skip switches which aren't normally
		 * activated on the playfield.  For switches in a ball container,
		 * simulate device entry there, otherwise simulate a switch event. */
		swinfo = switch_lookup (sw);

		if (SW_HAS_DEVICE (swinfo))
		{
			device_t *dev = device_entry (SW_GET_DEVICE (swinfo));
			if (trough_dev_p (dev))
			{
				/* Don't always trigger the trough device.  The probability
				of a ball drain is treated as proportional to the number of
				balls in play.  So in big multiballs, we allow this to happen
				more frequently.
					We need to do this occasionally so that multiball modes
				will eventually end, else nothing else gets tested. */
				if (random () < CONFIG_STRESS_DRAIN_PROB * live_balls)
					switch_stress_drain ();
			}
			else if (dev->max_count < dev->size)
			{
				/* Don't throw an enter event if the device thinks
				it is full: it has "locked" as many balls as it
				can hold.  The device code will throw a fatal if
				it sees this, which should never happen with real
				balls. */
				dbprintf ("Sim. enter dev %d\n", dev->devno);
				device_call_op (dev, enter);
				task_sleep (TIME_1S);
			}
		}
		else if (swinfo->flags & SW_PLAYFIELD)
		{
			/* Simulate the switch */
			tp = task_create_gid (GID_SW_HANDLER, switch_sched_task);
			task_set_arg (tp, sw);
		}
	}
}


/**
 * Ensure that no simulation is the default.
 */
CALLSET_ENTRY (stress, init)
{
#ifdef CONFIG_SWITCH_STRESS
	switch_stress_enable = YES;
#else
	switch_stress_enable = NO;
#endif
}

static void stress_start_button_task (void)
{
	callset_invoke (sw_start_button);
	task_exit ();
}

CALLSET_ENTRY (stress, minute_elapsed)
{
#ifdef CONFIG_STRESS_AUTO
	if (switch_stress_enable && !in_game && deff_get_active () == DEFF_AMODE)
	{
		dbprintf ("Stress auto start.\n");
		task_create_anon (stress_start_button_task);
	}
#endif
}


/**
 * Start the simulation as soon as valid playfield is declared,
 * if it has been enabled.
 */
CALLSET_ENTRY (stress, start_ball)
{
	if (switch_stress_enable == YES)
		task_create_gid1 (GID_SWITCH_STRESS, switch_stress_task);
}


/**
 * Stop the simulation during endball, or when a game is aborted.
 */
CALLSET_ENTRY (stress, end_ball, stop_game)
{
	task_kill_gid (GID_SWITCH_STRESS);
}

