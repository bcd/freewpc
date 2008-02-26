/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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
#include <rtsol.h>
#include <tz/clock.h>

enum mech_clock_mode
{
	CLOCK_STOPPED = 0,
	CLOCK_RUNNING_FORWARD,
	CLOCK_RUNNING_BACKWARD,
	CLOCK_CALIBRATING,
	CLOCK_FIND,
};


/* Current state of the clock switches, as read during the last rtt */
__fastram__ U8 clock_sw;

/* Mode that drives the rtt state machine */
__fastram__ enum mech_clock_mode clock_mode;

/** Configured speed of the clock.  When set to higher values, the
 * clock will run more slowly.  This represents the number of idle
 * rtt cycles between pulses to the clock motor drives. */
__fastram__ U8 clock_speed;

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

U8 clock_calibration_time;

U8 clock_is_working;


void tz_dump_clock (void)
{
	dbprintf ("\nClock switches now active: %02x\n", clock_sw);
	dbprintf ("Seen active: %02x\n", clock_sw_seen_active);
	dbprintf ("Seen inactive: %02x\n", clock_sw_seen_inactive);
	dbprintf ("State machine: %02x\n", clock_mode);
	dbprintf ("Target switches: %02x\n", clock_find_target);
	dbprintf ("Clock speed: %02x\n", clock_speed);
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


/*
 * Twilight Zone Clock Driver
 */

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

/* Drives for the clock and the switch strobe on the aux board */
#define CLK_DRV_REVERSE				0x20
#define CLK_DRV_FORWARD				0x40
#define CLK_DRV_SWITCH_STROBE		0x80

extern inline void wpc_ext1_enable (const U8 bits)
{
	wpc_asic_setbits (WPC_EXTBOARD1, bits);
}

extern inline void wpc_ext1_disable (const U8 bits)
{
	wpc_asic_clearbits (WPC_EXTBOARD1, bits);
}


/** A lower priority periodic function. */
CALLSET_ENTRY (tz_clock, idle_every_100ms)
{
	if (clock_mode == CLOCK_CALIBRATING)
	{
		if ((clock_sw_seen_active & clock_sw_seen_inactive) == 0xFF)
		{
			clock_mode = CLOCK_FIND;
			clock_find_target = tz_clock_hour_to_opto[11] | CLK_SW_MIN00;
		}
		else if (--clock_calibration_time == 0)
		{
			dbprintf ("Calibration aborted.\n");
			clock_mode = CLOCK_STOPPED;
			clock_calibration_time = 1;
			clock_is_working = 0;
		}
		else
		{
			sol_stop (SOL_CLOCK_REVERSE);
			sol_start (SOL_CLOCK_FORWARD, clock_speed, TIME_1S);
		}
	}
	/* Refresh clock outputs when active */
	else if (clock_mode == CLOCK_RUNNING_FORWARD)
	{
		sol_stop (SOL_CLOCK_REVERSE);
		sol_start (SOL_CLOCK_FORWARD, clock_speed, TIME_1S);
	}
	else if (clock_mode == CLOCK_FIND || clock_mode == CLOCK_RUNNING_BACKWARD)
	{
		sol_stop (SOL_CLOCK_FORWARD);
		sol_start (SOL_CLOCK_REVERSE, clock_speed, TIME_1S);
	}
}


/** The real-time task driver for the clock.
 * This function is called once every 33ms. */
void tz_clock_rtt (void)
{
	/* When the clock is stopped, there is nothing to do.
	 * This is the common case.  Even if by chance the
	 * clock outputs happen to be driving it, they will
	 * stop eventually due to the design of the solenoid
	 * refresh. */
	if (likely (clock_mode == CLOCK_STOPPED))
		return;

	/* Read latest switch state.  Enabling the output at
	 * bit 7 on the I/O extender switches the row input
	 * from the switch matrix to the 9th column of clock
	 * switches. */
	wpc_ext1_enable (CLK_DRV_SWITCH_STROBE);
	clock_last_sw = clock_sw;
	clock_sw = ~ wpc_asic_read (WPC_SW_ROW_INPUT);
	wpc_ext1_disable (CLK_DRV_SWITCH_STROBE);

	/* Unless any switches change, there's nothing else to do */
	if (unlikely (clock_sw != clock_last_sw))
	{
		/* Update the active/inactive switch list for calibration */
		clock_sw_seen_active |= clock_sw;
		clock_sw_seen_inactive |= ~clock_sw;

		/* If searching for a specific target, see if we're there */
		if (unlikely (clock_mode == CLOCK_FIND))
		{
			if (clock_sw == clock_find_target)
			{
				/* Yep, stop NOW! */
				goto stop_clock;
			}
			else if ((clock_sw & 0xF0) == (clock_find_target & 0xF0) && (clock_speed >= 0xAA))
			{
				/* No, but we're close.  Slow down a bit. */
				clock_speed = 0x44;
			}
			/* Otherwise, the clock keeps running as it was */
		}
	}
	return;

stop_clock:
	/* Before stopping the clock, give one last 16ms pulse to try to
	 * put the minute hand exactly over the desired minute opto. */
	if (clock_mode == CLOCK_RUNNING_FORWARD)
	{
		sol_start_real (SOL_CLOCK_FORWARD, 0xFF, 4);
		rt_sol_disable (SOL_CLOCK_REVERSE);
	}
	else
	{
		sol_start_real (SOL_CLOCK_REVERSE, 0xFF, 4);
		rt_sol_disable (SOL_CLOCK_FORWARD);
	}
	clock_mode = CLOCK_STOPPED;
	return;
}


void tz_clock_start_forward (void)
{
	if (clock_is_working && feature_config.disable_clock == NO)
		clock_mode = CLOCK_RUNNING_FORWARD;
}


void tz_clock_start_backward (void)
{
	if (clock_is_working && feature_config.disable_clock == NO)
		clock_mode = CLOCK_RUNNING_BACKWARD;
}


void tz_clock_set_speed (U8 speed)
{
	if (clock_is_working && feature_config.disable_clock == NO)
		clock_speed = speed;
}


void tz_clock_stop (void)
{
	clock_mode = CLOCK_STOPPED;
}


void tz_clock_reset (void)
{
	if (feature_config.disable_clock == YES)
	{
		tz_clock_stop ();
		return;
	}

	/* Find the home position at super speed */
	tz_clock_set_speed (0xEE);
	clock_mode = CLOCK_FIND;
	clock_find_target = tz_clock_hour_to_opto[11] | CLK_SW_MIN00;
}


CALLSET_ENTRY (tz_clock, init)
{
	clock_mode = CLOCK_STOPPED;
	clock_sw_seen_active = 0;
	clock_sw_seen_inactive = 0;
	clock_sw = 0;
	clock_is_working = 1;
	clock_speed = 0xEE;
}

CALLSET_ENTRY (tz_clock, amode_start)
{
	/* If not all of the other clock switches have been seen in both
	 * active and inactive states, start the clock. */
	if ((clock_sw_seen_active & clock_sw_seen_inactive) != 0xFF)
	{
		tz_clock_set_speed (0xEE);
		clock_calibration_time = 100;
		clock_is_working = 1;
		clock_mode = CLOCK_CALIBRATING;
	}
	else
	{
		tz_clock_reset ();
	}
}


CALLSET_ENTRY (tz_clock, amode_stop)
{
	/* Stop the calibration or real-time clock display that
	 * runs in the attract mode. */
	tz_clock_stop ();
}


CALLSET_ENTRY (tz_clock, test_start)
{
	/* Stop the clock unconditionally when entering test mode. */
	tz_clock_stop ();
}

CALLSET_ENTRY (tz_clock, start_game)
{
}

CALLSET_ENTRY (tz_clock, start_ball)
{
	/* Reset the clock to 12:00 at the start of ball */
	tz_clock_reset ();
}

CALLSET_ENTRY (tz_clock, end_ball)
{
	/* Reset the clock to 12:00 at the end of ball */
	tz_clock_reset ();
}

