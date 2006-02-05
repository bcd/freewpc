
#include <freewpc.h>


enum mech_clock_mode
{
	CLOCK_STOPPED = 0,
	CLOCK_RUNNING_FORWARD,
	CLOCK_RUNNING_BACKWARD,
	CLOCK_CALIBRATING,
	CLOCK_FIND,
};


/* Current state of the clock switches, as read during the last rtt */
U8 clock_sw;

/* Mode that drives the rtt state machine */
enum mech_clock_mode clock_mode;

/* Configured speed of the clock.  When set to higher values, the
 * clock will run more slowly.  This represents the number of idle
 * rtt cycles between pulses to the clock motor drives. */
U8 clock_speed;

U8 clock_delay_time;

/* Clock switches which have been seen to be active */
U8 clock_sw_seen_active;

/* Clock switches which have been seen to be inactive */
U8 clock_sw_seen_inactive;

/* Indicates the target switch values that will trigger the
 * clock to stop moving */
U8 clock_find_target;

/* Clock switches seen on the last rtt cycle, used to detect
 * switch transitions */
U8 clock_last_sw;

U8 clock_sw_changed;

U8 clock_calibration_ticks;

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
static U8 tz_clock_opto_to_hour[] =
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
	*(volatile U8 *)WPC_EXTBOARD1 |= bits;
}

extern inline void wpc_ext1_disable (U8 bits)
{
	*(volatile U8 *)WPC_EXTBOARD1 &= ~bits;
}


void tz_clock_rtt (void)
{
	/* Read latest switch state */
	wpc_ext1_enable (CLK_DRV_SWITCH_STROBE);
	clock_last_sw = clock_sw;
	clock_sw = ~ (*(volatile U8 *)WPC_SW_ROW_INPUT);
	wpc_ext1_disable (CLK_DRV_SWITCH_STROBE);

	/* Set transition flag if the value changed */
	if (clock_last_sw != clock_sw)
		clock_sw_changed++;

	/* Add to list of all switches seen */
	clock_sw_seen_active |= clock_sw;
	clock_sw_seen_inactive |= ~clock_sw;

	/* Update solenoid drives based on desired direction
	 * and speed */
	switch (clock_mode)
	{
		clock_stopped:
		case CLOCK_STOPPED:
			sol_off (SOL_CLOCK_FORWARD);
			sol_off (SOL_CLOCK_REVERSE);
			break;

		case CLOCK_RUNNING_FORWARD:
		case CLOCK_RUNNING_BACKWARD:
			if (clock_delay_time != clock_speed)
			{
				goto clock_stopped;
			}
			else if (clock_mode == CLOCK_RUNNING_FORWARD)
			{
		clock_running_forward:
				sol_on (SOL_CLOCK_FORWARD);
				sol_off (SOL_CLOCK_REVERSE);
			}
			else
			{
		clock_running_backward:
				sol_off (SOL_CLOCK_FORWARD);
				sol_on (SOL_CLOCK_REVERSE);
			}
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

			/* Once all switches have been seen, proceed
			 * to finding home position (12:00)
			 */
			if ((clock_sw_seen_active & clock_sw_seen_inactive) == 0xFF)
			{
				clock_mode = CLOCK_FIND;
				clock_find_target = 
					tz_clock_hour_to_opto[11] | CLK_SW_MIN00;
			}
			else
			{
				goto clock_running_forward;
			}
			break;

		case CLOCK_FIND:
			if (clock_sw == clock_find_target)
			{
				clock_mode = CLOCK_STOPPED;
				goto clock_stopped;
			}
			else if (clock_delay_time != clock_speed)
			{
				goto clock_stopped;
			}
			else
			{
				/* Direction to move depends on current state */
				goto clock_running_backward;
			}
			break;
	}

	if (--clock_delay_time == 0)
		clock_delay_time = clock_speed;
}


void tz_clock_start_forward (void)
{
	clock_mode = CLOCK_RUNNING_FORWARD;
}


void tz_clock_start_backward (void)
{
	clock_mode = CLOCK_RUNNING_BACKWARD;
}


void tz_clock_stop (void)
{
	clock_mode = CLOCK_STOPPED;
}


CALLSET_ENTRY (tz_clock, init)
{
	clock_sw = 0;
	clock_sw_seen_active = 0;
	clock_sw_seen_inactive = 0;
	clock_delay_time = clock_speed = 1;
	clock_calibration_ticks = 4;
	clock_mode = CLOCK_CALIBRATING;
}

