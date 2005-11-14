
#include <freewpc.h>


enum mech_clock_mode
{
	CLOCK_STOPPED,
	CLOCK_RUNNING_FORWARD,
	CLOCK_RUNNING_BACKWARD,
	CLOCK_CALIBRATING,
	CLOCK_FIND,
};


U8 clock_sw;
enum mech_clock_mode clock_mode;
U8 clock_speed;
U8 clock_delay_time;
U8 clock_sw_seen;
U8 clock_find_target;


void tz_dump_clock (void)
{
	dbprintf ("\nClock switches now active: %02x\n", clock_sw);
	dbprintf ("\nClock switches seen: %02x\n", clock_sw_seen);
}


/*
 * Twilight Zone Clock Driver
 */

#define CLK_SW_HOUR12	0x1
#define CLK_SW_HOUR3		0x2
#define CLK_SW_HOUR6		0x4
#define CLK_SW_HOUR9		0x8
#define CLK_SW_MIN00		0x10
#define CLK_SW_MIN15		0x20
#define CLK_SW_MIN30		0x40
#define CLK_SW_MIN45		0x80

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
	clock_sw = ~ (*(volatile U8 *)WPC_SW_ROW_INPUT);
	wpc_ext1_disable (CLK_DRV_SWITCH_STROBE);

	/* Add to list of all switches seen to be working. */
	clock_sw_seen |= clock_sw;

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
				goto clock_running_forward;
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


void tz_clock_reset (void) 
{
	clock_find_target = CLK_SW_HOUR12 | CLK_SW_MIN00;
	clock_mode = CLOCK_FIND;
	task_exit ();
}


void tz_clock_init (void) 
{
	clock_mode = CLOCK_CALIBRATING;
	clock_sw = 0;
	clock_sw_seen = 0;
	clock_delay_time = clock_speed = 4;
	task_create_gid (GID_CLOCK_MECH, tz_clock_reset);
}

