/*
 * Copyright 2006-2009 by Brian Dominy <brian@oddchange.com>
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

/* Full rotation of the clock takes:
 * 	
 * 	Duty
 * 	100% = ~12 seconds
 * 	50% = ~25 seconds
 * 	25% = ~55 seconds
 */

/* CALLSET_SECTION (clock, __machine__) */
#include <freewpc.h>
#include <clock_mech.h>
#include <diag.h>

/** The logical states of the clock driver state machine */
enum mech_clock_mode
{
	/** The clock should not be running at all */
	CLOCK_STOPPED = 0,
	/** The clock should run forward */
	CLOCK_RUNNING_FORWARD,
	/** The clock should run backward */
	CLOCK_RUNNING_BACKWARD,
	/** The clock should run forward after being unpaused */
	CLOCK_PAUSED_FORWARD,
	/** The clock should run backward after being unpaused */
	CLOCK_PAUSED_BACKWARD,
	/** The clock should be calibrated */
	CLOCK_CALIBRATING,
	/** The clock is trying to find a particular location,
	 * and will automatically go to STOPPED when it gets there. */
	CLOCK_FIND,
};


/* Current state of the clock switches, as read during the last rtt.
The upper 4-bits gives the state of the hour optos.  The lower 4-bits
gives the state of the minutes optos */
__fastram__ U8 clock_sw;

#define CLK_SW_HOUR(sw)   ((sw) & 0xF0)
#define CLK_SW_MIN(sw)    ((sw) & 0x0F)

#define CLK_SW_HOUR_EQUAL_P(sw1,sw2) \
	CLK_SW_HOUR(sw1) == CLK_SW_HOUR(sw2)

/* Mode that drives the rtt state machine */
__fastram__ enum mech_clock_mode clock_mode;

/** Clock switches which have been seen to be active */
__fastram__ U8 clock_sw_seen_active;

/** Clock switches which have been seen to be inactive */
__fastram__ U8 clock_sw_seen_inactive;

/** Indicates the target switch values that will trigger the
 * clock to stop moving */
__fastram__ U8 clock_find_target;

/** Clock switches seen on the last rtt cycle, used to detect
 * switch transitions */
__fastram__ U8 clock_last_sw;

/** The clock time "decoded" as the number of 15-minute intervals
past 12:00, ranging from 0 to 47. */
U8 clock_decode;

/** The last clock switch(es) that were seen active */
U8 clock_minute_sw;

/** The task that currently owns the clock */
task_gid_t clock_owner;

/** The current clock hour, as an integer from 0-11 */
U8 clock_hour;

U8 mech_speed_stored;
extern struct timed_mode_ops mpf_mode;

/* rtc hour and minute */
extern U8 hour;
extern U8 minute;

void tz_dump_clock (void)
{
	dbprintf ("\nClock switches now active: %02x\n", clock_sw);
	dbprintf ("Seen active: %02x\n", clock_sw_seen_active);
	dbprintf ("Seen inactive: %02x\n", clock_sw_seen_inactive);
	dbprintf ("State machine: %02x\n", clock_mode);
	dbprintf ("Target switches: %02x\n", clock_find_target);
}


void tz_debugger_hook (char c)
{
	switch (c)
	{
		case 'c':
			tz_dump_clock ();
			break;
		default:
			break;
	}
}


/* Given an hour value (0-11), return the value of the hour
 * optos that matches it.  The reading is valid from
 * H:30 to (H+1):29.  */
static U8 tz_clock_hour_to_opto[] =
{ 0x10, 0x00, 0x40, 0xC0, 0xD0, 0x50, 0x70, 0x60, 0x20, 0x30, 0xB0, 0x90 };

/* The same information, in reverse: given an opto reading,
 * returns the hour value */
U8 tz_clock_opto_to_hour[] =
{ 1, 0, 8, 9, 2, 5, 7, 6, 0, 11, 0, 10, 3, 4, 0, 0 };

/* The lower nibble of the current clock switch reading
 * returns the minute optos.  The opto is only active when
 * the minute hand is directly over the given position. */
#define CLK_SW_MIN00		0x2
#define CLK_SW_MIN15		0x1
#define CLK_SW_MIN30		0x8
#define CLK_SW_MIN45		0x4

/* Switch strobe enable on the aux board */
#define SOL_SWITCH_STROBE 47
#define CLK_DRV_SWITCH_STROBE		(1 << (SOL_SWITCH_STROBE % 8))

extern inline void clock_switch_enable (void)
{
	U8 *in = sol_get_read_reg (SOL_SWITCH_STROBE);
	IOPTR out = sol_get_write_reg (SOL_SWITCH_STROBE);
	writeb (out, *in | sol_get_bit (SOL_SWITCH_STROBE));
}

extern inline void clock_switch_disable (void)
{
	U8 *in = sol_get_read_reg (SOL_SWITCH_STROBE);
	IOPTR out = sol_get_write_reg (SOL_SWITCH_STROBE);
	writeb (out, *in & ~sol_get_bit (SOL_SWITCH_STROBE));
}


/** Decodes the current clock switches to determine what time
 * it is. */
U8 tz_clock_gettime (void)
{
	clock_decode = clock_hour * 4;

	/* Adjust according to the last minute opto seen. */
	if (clock_minute_sw & CLK_SW_MIN15)
		clock_decode += 1;
	else if (clock_minute_sw & CLK_SW_MIN30)
		clock_decode += 2;
	else if (clock_minute_sw & CLK_SW_MIN45)
		clock_decode += 3;

	while (clock_decode >= 48)
		clock_decode -= 48;

	return clock_decode;
}

/** The real-time task driver for the clock.
 * This function is called once every 8ms. */
void tz_clock_switch_rtt (void)
{
	/* Read latest switch state.  Enabling the output at
	 * bit 7 on the I/O extender switches the row input
	 * from the switch matrix to the 9th column of clock
	 * switches. */
	clock_switch_enable ();
	clock_last_sw = clock_sw;
	clock_sw = ~ readb (WPC_SW_ROW_INPUT);
	clock_switch_disable ();

	/* Unless any switches change, there's nothing else to do */
	if ((clock_sw != clock_last_sw))
	{
		/* Always remember the last minute opto seen.  The hour optos
		can be read at any time, but the minute optos are only active
		when the arm actually crosses one of the 15 minute marks. */
		if (CLK_SW_MIN (clock_sw))
		{
			clock_minute_sw = CLK_SW_MIN (clock_sw);
			if (clock_minute_sw == CLK_SW_MIN30 &&
				clock_mode == CLOCK_RUNNING_FORWARD)
			{
				/* When the minute hand is at :30, the hour switches
				tell us exactly what the hour is. */
				clock_hour = tz_clock_opto_to_hour[clock_sw >> 4];
			}
			else if (clock_minute_sw == CLK_SW_MIN00 &&
			//	CLK_SW_MIN (clock_last_sw) == CLK_SW_MIN45 &&
				clock_mode == CLOCK_RUNNING_FORWARD)
			{
				clock_hour++;
				if (clock_hour == 12)
					clock_hour = 0;
			}
			else if (clock_minute_sw == CLK_SW_MIN00
			&& clock_mode == CLOCK_RUNNING_BACKWARD)
			{	
				if (clock_hour)
					clock_hour--;
				else
					clock_hour = 11;
			}
		}
		/* If searching for a specific target, see if we're there */
		if ((clock_mode == CLOCK_FIND))
		{
			if (clock_sw == clock_find_target)
			{
				/* Yep, stop NOW! */
				clock_mech_stop_from_interrupt ();
				global_flag_on (GLOBAL_FLAG_CLOCK_HOME);
			}
			else if (CLK_SW_HOUR_EQUAL_P (clock_sw, clock_find_target)
				&& (clock_mech_get_speed () < BIVAR_DUTY_25))
			{
				/* No, but we're close.  Slow down a bit. */
				clock_mech_set_speed (BIVAR_DUTY_25);
			}
			/* Otherwise, the clock keeps running as it was */
			/* BUG workaround: Goes very slowly backwards when it's miles away from home */
		//	if (clock_mech_get_speed () == BIVAR_DUTY_25
		//		&& (clock_hour != 11 || clock_hour != 0))
		//	{
		//		clock_mech_set_speed (BIVAR_DUTY_100);
		//	}

		}
		else if ((clock_mode == CLOCK_CALIBRATING))
		{
			/* Update the active/inactive switch list for calibration */
			clock_sw_seen_inactive |= ~clock_sw;
			clock_sw_seen_active |= clock_sw;
		}
	}
}


/**
 * Clear the current clock owner.
 */
void tz_clock_clear_owner (void)
{
	clock_owner = NULL;
}


/**
 * Allocate the clock device.
 */
bool tz_clock_alloc (task_gid_t owner)
{
	if (clock_owner == owner)
		return TRUE;
	if (clock_owner != NULL)
		return FALSE;
	clock_owner = owner;
	return TRUE;
}


/**
 * Free the clock device.
 */
void tz_clock_free (task_gid_t owner)
{
	if (clock_owner == owner)
		tz_clock_clear_owner ();
}

void tz_clock_show_time (U8 hours, U8 minutes)
{
	if (hours > 12)
		hours = 12;
	else if (hours == 0)
		hours = 12;
	if (minutes > 59);
		minutes = 59;
	
	if (minutes < 15)
		clock_find_target = tz_clock_hour_to_opto[hours - 1] | CLK_SW_MIN00;
	else if (minutes < 30)
		clock_find_target = tz_clock_hour_to_opto[hours - 1] | CLK_SW_MIN15;
	else if (minutes < 45)
		clock_find_target = tz_clock_hour_to_opto[hours - 1] | CLK_SW_MIN30;
	else if (minutes >= 45)
		clock_find_target = tz_clock_hour_to_opto[hours - 1] | CLK_SW_MIN45;
	clock_mode = CLOCK_FIND;

}

/**
* Pause the clock and restart again
*/
bool clock_moving (void)
{
	if (clock_mode == CLOCK_RUNNING_FORWARD || clock_mode == CLOCK_RUNNING_BACKWARD)
		return TRUE;
	else
		return FALSE;
}

bool clock_paused (void)
{
	if (clock_mode == CLOCK_PAUSED_FORWARD || clock_mode == CLOCK_PAUSED_BACKWARD)
		return TRUE;
	else
		return FALSE;
}

bool should_pause_clock (void)
{
	if (kickout_locks > 0 || timed_mode_running_p (&mpf_mode))
		return TRUE;
	else
		return FALSE;
}

CALLSET_ENTRY (clock, idle_every_second)
{
	if (!in_live_game)
		return;
	if (should_pause_clock () && clock_moving ())
	{
		mech_speed_stored = clock_mech_get_speed ();
		if (clock_mode == CLOCK_RUNNING_FORWARD)
		{
			clock_mode = CLOCK_PAUSED_FORWARD;
		}
		else
		{
			clock_mode = CLOCK_PAUSED_BACKWARD;
		}
		clock_mech_stop ();
	}
	else if (clock_paused () && !should_pause_clock ())
	{
		clock_mech_set_speed (mech_speed_stored);
		if (clock_mode == CLOCK_PAUSED_FORWARD)
		{
			tz_clock_start_forward ();
		}
		else
		{
			tz_clock_start_backward ();
		}
	}
}
void tz_clock_start_forward (void)
{
	if (in_test || global_flag_test (GLOBAL_FLAG_CLOCK_WORKING))
	{
		global_flag_off (GLOBAL_FLAG_CLOCK_HOME);
		clock_mode = CLOCK_RUNNING_FORWARD;
		clock_mech_start_forward ();
	}
}

void tz_clock_start_backward (void)
{
	if (in_test || global_flag_test (GLOBAL_FLAG_CLOCK_WORKING))
	{
		global_flag_off (GLOBAL_FLAG_CLOCK_HOME);
		clock_mode = CLOCK_RUNNING_BACKWARD;
		clock_mech_start_reverse ();
	}
}

void tz_clock_reverse_direction (void)
{
	if (clock_mode == CLOCK_RUNNING_FORWARD)
	{
		clock_mode = CLOCK_RUNNING_BACKWARD;
		clock_mech_start_reverse ();
	}
	else if (clock_mode == CLOCK_RUNNING_BACKWARD)
	{
		clock_mode = CLOCK_RUNNING_FORWARD;
		clock_mech_start_forward ();
	}
}

void tz_clock_stop (void)
{
	clock_mode = CLOCK_STOPPED;
	clock_mech_stop ();
}

void tz_clock_error (void)
{
	audit_increment (&feature_audits.clock_errors);
	tz_clock_stop ();
	global_flag_off (GLOBAL_FLAG_CLOCK_WORKING);
	tz_dump_clock ();
}


/**
 * Reset the mechanical clock to the home position.
 */
void tz_clock_reset (void)
{
	if (feature_config.disable_clock == YES)
	{
		/* Don't do anything if the clock is disabled */
		tz_clock_stop ();
	}
	else
	{
		dbprintf ("Clock resetting to home.\n");
		/* See where the clock is and start it if it's not already home. */
		clock_find_target = tz_clock_hour_to_opto[11] | CLK_SW_MIN00;
		if (clock_sw != clock_find_target && !global_flag_test (GLOBAL_FLAG_CLOCK_HOME))
		{
			if (clock_hour <= 6)
				clock_mech_start_reverse ();
			else
				clock_mech_start_forward ();
			timer_start_free (GID_CLOCK_FINDING, TIME_15S);
			
			if (clock_mode != CLOCK_FIND && clock_mode != CLOCK_CALIBRATING)
				clock_mech_set_speed (BIVAR_DUTY_100);
			clock_mode = CLOCK_FIND;
		}
	}
}

/* Manually set the clock to home */
CALLSET_ENTRY (tz_clock, clock_at_home)
{
	tz_clock_stop ();
	global_flag_on (GLOBAL_FLAG_CLOCK_HOME);
	clock_hour = 0;
	clock_sw_seen_active = 0xFF;
	clock_sw_seen_inactive = 0xFF;
}

/**
 * A periodic, lower priority function that updates the
 * state machine depending on what has been seen recently.
 */
CALLSET_ENTRY (tz_clock, idle_every_100ms)
{
	/* When calibrating, once all switches have been active and inactive
	 * at least once, claim victory and go back to the home position. */
	if (clock_mode == CLOCK_CALIBRATING)
	{
		if ((clock_sw_seen_active & clock_sw_seen_inactive) == 0xFF)
		{
			/* CALIBRATING -> FIND */
			dbprintf ("Calibration complete.\n");
			tz_clock_reset ();
		}
		/* If calibration doesn't succeed within a certain number
		 * of iterations, give up. */
		if (!timer_find_gid (GID_CLOCK_CALIBRATING))
		{
			dbprintf ("Calibration aborted.\n");
			tz_clock_error ();
		}
	}
	else if (clock_mode == CLOCK_FIND && !timer_find_gid (GID_CLOCK_FINDING))
	{
		dbprintf ("Finding failed.\n");
		tz_clock_stop ();
	}
}


/**
 * Reinitialize the mechanical clock driver.
 */
CALLSET_ENTRY (tz_clock, init)
{
	global_flag_off (GLOBAL_FLAG_CLOCK_HOME);
	clock_mode = CLOCK_STOPPED;
	clock_sw_seen_active = 0;
	clock_sw_seen_inactive = 0;
	clock_sw = 0;
	clock_minute_sw = 0;
	clock_hour = 0;
	global_flag_on (GLOBAL_FLAG_CLOCK_WORKING);
	clock_mech_set_speed (BIVAR_DUTY_100);
	mech_speed_stored = BIVAR_DUTY_100;
	tz_clock_clear_owner ();
}

CALLSET_ENTRY (tz_clock, amode_start)
{
	if (feature_config.disable_clock == YES)
	{
		global_flag_off (GLOBAL_FLAG_CLOCK_WORKING);
	}

	/* If not all of the other clock switches have been seen in both
	 * active and inactive states, start the clock. */
	else if ((clock_sw_seen_active & clock_sw_seen_inactive) != 0xFF
			&& !global_flag_test (GLOBAL_FLAG_CLOCK_HOME))
	{
		dbprintf ("Clock calibration started.\n");
		timer_restart_free (GID_CLOCK_CALIBRATING, TIME_15S);
		global_flag_on (GLOBAL_FLAG_CLOCK_WORKING);
		clock_mech_set_speed (BIVAR_DUTY_100);
		clock_mode = CLOCK_CALIBRATING;
		clock_mech_start_forward ();
	}
	else if (global_flag_test (GLOBAL_FLAG_CLOCK_HOME))
		tz_clock_show_time (hour, minute);
	else	
		tz_clock_reset ();
}

void tz_clock_set_speed (U8 speed)
{
	if (speed > 3)
		speed = 3;
	switch (speed)
	{	
		default:
		case 0:
		case 1:
			clock_mech_set_speed (BIVAR_DUTY_25);
			break;
		case 2:
			clock_mech_set_speed (BIVAR_DUTY_50);
			break;
		case 3:
			clock_mech_set_speed (BIVAR_DUTY_100);
			break;
			
	}
}

CALLSET_ENTRY (tz_clock, diagnostic_check)
{
	if (feature_config.disable_clock)
		diag_post_error ("CLOCK DISABLED\nBY ADJUSTMENT\n", PAGE);
	while (unlikely (clock_mode == CLOCK_CALIBRATING))
		task_sleep (TIME_100MS);
	if (!global_flag_test (GLOBAL_FLAG_CLOCK_WORKING))
		diag_post_error ("CLOCK IS\nNOT WORKING\n", PAGE);
}

/**
 * Stop the clock when entering test mode
 */
CALLSET_ENTRY (tz_clock, amode_stop, test_start)
{
	tz_clock_stop ();
}

/**
 * Reset the clock to the home position at the start of
 * each ball.
 */
CALLSET_ENTRY (tz_clock, start_ball, end_ball)
{
	tz_clock_reset ();
}

