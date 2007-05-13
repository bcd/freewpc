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

/** The current clock delay counter */
__fastram__ U8 clock_delay_time;

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

/* For measuring the speed of the clock motor */
#if 0
__fastram__ U8 clock_calibration_ticks;
#endif

void tz_dump_clock (void)
{
	dbprintf ("\nClock switches now active: %02x\n", clock_sw);
	dbprintf ("Seen active: %02x\n", clock_sw_seen_active);
	dbprintf ("Seen inactive: %02x\n", clock_sw_seen_inactive);
	dbprintf ("State machine: %02x\n", clock_mode);
	dbprintf ("Target switches: %02x\n", clock_find_target);
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

extern inline void wpc_ext1_enable (U8 bits)
{
	wpc_asic_setbits (WPC_EXTBOARD1, bits);
}

extern inline void wpc_ext1_disable (U8 bits)
{
	wpc_asic_clearbits (WPC_EXTBOARD1, bits);
}

void tz_clock_rtt (void)
{
	/* Read latest switch state */
	wpc_ext1_enable (CLK_DRV_SWITCH_STROBE);
	clock_last_sw = clock_sw;
	clock_sw = ~ wpc_asic_read (WPC_SW_ROW_INPUT);
	wpc_ext1_disable (CLK_DRV_SWITCH_STROBE);

	/* Update solenoid drives based on desired direction
	 * and speed */
	switch (clock_mode)
	{
		clock_stopped:
		case CLOCK_STOPPED:
			rt_sol_disable (SOL_CLOCK_FORWARD);
			rt_sol_disable (SOL_CLOCK_REVERSE);
			break;

		clock_running_forward:
		case CLOCK_RUNNING_FORWARD:
			if (clock_delay_time != clock_speed)
				goto clock_stopped;
			rt_sol_disable (SOL_CLOCK_REVERSE);
			rt_sol_enable (SOL_CLOCK_FORWARD);
			break;

		clock_running_backward:
		case CLOCK_RUNNING_BACKWARD:
			if (clock_delay_time != clock_speed)
				goto clock_stopped;
			rt_sol_disable (SOL_CLOCK_FORWARD);
			rt_sol_enable (SOL_CLOCK_REVERSE);
			break;

		case CLOCK_CALIBRATING:
#if 0
			/* Calculate number of rtt cycles between minute optos */
			if (clock_sw & 0x0F)
			{
				/* On a minute opto : publish current count
				 * and continue */
				dbprintf ("Clock calibration count: %02x\n", 
					clock_calibration_ticks);
				clock_calibration_ticks = 0;
			}
			else
				clock_calibration_ticks++;
#endif

			/* Add to list of all switches seen if changed */
			if (clock_sw != clock_last_sw)
			{
				clock_sw_seen_active |= clock_sw;
				clock_sw_seen_inactive |= ~clock_sw;
				/* Once all switches have been seen, proceed
				 * to finding home position (12:00)
				 */
				if ((clock_sw_seen_active & clock_sw_seen_inactive) == 0xFF)
				{
					clock_mode = CLOCK_FIND;
					clock_find_target = 
						tz_clock_hour_to_opto[11] | CLK_SW_MIN00;
				}
			}
			goto clock_running_forward;
			break;

		case CLOCK_FIND:
			if (clock_sw == clock_find_target)
			{
				clock_mode = CLOCK_STOPPED;
				goto clock_stopped;
			}
			else if ((clock_sw & 0xF0) == (clock_find_target & 0xF0) && (clock_speed < 2))
			{
				/* If the clock is 'close' to its target, then slow down. */
				clock_speed = 3;
				goto clock_stopped;
			}
			else if (clock_delay_time != clock_speed)
			{
				goto clock_stopped;
			}
			else
			{
				if (1) /* TODO : (tz_clock_opto_to_hour[clock_sw >> 4] <= 6) */
					goto clock_running_backward;
				else
					goto clock_running_forward;
			}
			break;
	}

	if (--clock_delay_time == 0)
		clock_delay_time = clock_speed;
}


void tz_clock_start_forward (void)
{
	if (feature_config.disable_clock == NO)
		clock_mode = CLOCK_RUNNING_FORWARD;
}


void tz_clock_start_backward (void)
{
	if (feature_config.disable_clock == NO)
		clock_mode = CLOCK_RUNNING_BACKWARD;
}


void tz_clock_set_speed (U8 speed)
{
	if (feature_config.disable_clock == NO)
		clock_delay_time = clock_speed = speed;
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
	tz_clock_set_speed (1);
	clock_mode = CLOCK_FIND;
	clock_find_target = tz_clock_hour_to_opto[11] | CLK_SW_MIN00;
}


CALLSET_ENTRY (tz_clock, init)
{
	clock_mode = CLOCK_STOPPED;
	clock_sw_seen_active = 0;
	clock_sw_seen_inactive = 0;
	clock_sw = 0;
}

CALLSET_ENTRY (tz_clock, amode_start)
{
	/* If not all of the other clock switches have been seen in both
	 * active and inactive states, start the clock. */
	if ((clock_sw_seen_active & clock_sw_seen_inactive) != 0xFF)
	{
		tz_clock_set_speed (1);
#if 0
		clock_calibration_ticks = 3;
#endif
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

