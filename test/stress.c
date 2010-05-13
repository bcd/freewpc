/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

/* CALLSET_SECTION (stress, __test2__) */

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
 * This task runs during a switch stress test.  During a game,
 * it randomly invokes switch handlers as if the switches had actually
 * been activated by the pinball.
 */
void switch_stress_task (void)
{
	U8 sw;
	const switch_info_t *swinfo;
	task_pid_t tp;

	device_add_live ();
	task_create_peer (switch_stress_flipper_task);
	for (;;)
	{
		task_sleep (TIME_100MS);

		/* Choose a switch at random.  Skip certain well-known switches that are
		 * never to be activated. */
		sw = random_scaled (NUM_SWITCHES);
		if (sw == SW_ALWAYS_CLOSED)
			continue;
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
				/* Don't trigger the trough device, because that will lead
				to endball and we want to keep the simulation going.
				Occasionally, though, throw single_ball_play so that any
				multiball running will stop */
				if (random () < 30)
					callset_invoke (single_ball_play);
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


void switch_stress_endball (void)
{
#ifdef DEVNO_TROUGH
	device_t *dev = device_entry (DEVNO_TROUGH);
	device_call_op (dev, enter);
#endif
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
 * Stop the simulation as soon as a ball enters the trough.
 * Also force it to stop on endball and endgame.
 */
CALLSET_ENTRY (stress, dev_trough_enter, end_ball, stop_game)
{
	task_kill_gid (GID_SWITCH_STRESS);
}

