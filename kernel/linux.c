/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

/**
 * \file
 * \brief Simulation functions for use in a Linux environment
 */

#include <termios.h>
#include <time.h>
#include <freewpc.h>


extern void do_reset (void);
extern void do_swi3 (void);
extern void do_swi2 (void);
extern void do_firq (void);
extern void do_irq (void);
extern void do_swi (void);
extern void do_nmi (void);

extern void exit (int);

/** The number of IRQs per second. */ 
#define IRQS_PER_SEC 1024

/** The frequency of the realtime thread, in milliseconds */
#define RT_THREAD_FREQ 50

/** The frequency of the interface thread, in milliseconds */
#define IF_THREAD_FREQ 50

/** The number of IRQs that need to be asserted on every
 * iteration of the realtime thread. */
#define RT_ITERATION_IRQS ((IRQS_PER_SEC * RT_THREAD_FREQ) / 1000)

/** The rate at which the simulated clock should run */
int linux_irq_multiplier = 1;

/** An array of DMD page buffers */
U8 linux_dmd_pages[DMD_PAGE_COUNT][DMD_PAGE_SIZE];

/** A pointer to the low DMD page */
U8 *linux_dmd_low_page;

/** A pointer to the high DMD page */
U8 *linux_dmd_high_page;

/** A pointer to the visible DMD page */
U8 *linux_dmd_visible_page;

/** The simulated switch matrix inputs.  This acts as a buffer between the
 * simulation code and the actual product code, but it serves the same 
 * purpose.  Only one matrix is needed, however. 
 */
U8 linux_switch_matrix[SWITCH_BITS_SIZE];

/** The simulated lamp matrix outputs. */
U8 linux_lamp_matrix[NUM_LAMP_COLS];

/** The simulated solenoid outputs */
U8 linux_solenoid_outputs[SOL_ARRAY_WIDTH];

/** The simulated flipper outputs */
U8 linux_flipper_outputs;

/** The simulated flipper inputs */
U8 linux_flipper_inputs;

/** True if the IRQ is enabled */
bool linux_irq_enable;

/** Nonzero if an IRQ is pending */
int linux_irq_pending;

/** True if the FIRQ is enabled */
bool linux_firq_enable;

/** Nonzero if an FIRQ is pending */
int linux_firq_pending;

/** The number of IRQ cycles */
int linux_irq_count;

/** Pointer to the current switch matrix element */
U8 *linux_switch_data_ptr;

/** Pointer to the current lamp matrix element */
U8 *linux_lamp_data_ptr;

/** The jumper settings */
U8 linux_jumpers = 0;

/** The actual time at which the simulation was started */
time_t linux_boot_time;

/** The status of the CPU board LEDs */
U8 linux_cpu_leds;

/** The initial number of balls to 'install' */
int linux_installed_balls = -1;


/** Find the highest numbered bit set in a bitmask.
 * Returns -1 if no bits are set. */
static int scanbit (U8 bits)
{
	if (bits & 0x80) return 7;
	else if (bits & 0x40) return 6;
	else if (bits & 0x20) return 5;
	else if (bits & 0x10) return 4;
	else if (bits & 0x08) return 3;
	else if (bits & 0x04) return 2;
	else if (bits & 0x02) return 1;
	else if (bits & 0x01) return 0;
	else return -1;
}


static void linux_shutdown (void)
{
	printf ("Shutting down simulation.\n");
	exit (0);
}


void linux_write_string (const char *s)
{
	printf ("write_string: %s\n", s);
}


/** Simulated write of an I/O register */
void linux_asic_write (U16 addr, U8 val)
{
	switch (addr)
	{
		case WPC_DEBUG_DATA_PORT:
			putchar (val);
			break;

		case WPC_SHIFTADDR:
		case WPC_SHIFTBIT:
		case WPC_SHIFTBIT2:
			fatal (ERR_CANT_GET_HERE);

		case WPC_FLIPTRONIC_PORT_A:
			linux_flipper_outputs = val;
			break;

		case WPC_LEDS:
			linux_cpu_leds = val;
			break;

		case WPC_RAM_BANK:
		case WPC_RAM_LOCK:
		case WPC_RAM_LOCKSIZE:
		case WPC_ROM_LOCK:
		case WPC_ZEROCROSS_IRQ_CLEAR:
		case WPC_ROM_BANK:
			/* nothing to do */
			break;

		case WPC_DMD_LOW_PAGE:
			linux_dmd_low_page = linux_dmd_pages[val];
			break;

		case WPC_DMD_HIGH_PAGE:
			linux_dmd_high_page = linux_dmd_pages[val];
			break;

		case WPC_DMD_ACTIVE_PAGE:
			linux_dmd_visible_page = linux_dmd_pages[val];
			break;

		case WPC_SOL_FLASH2_OUTPUT:
		case WPC_SOL_HIGHPOWER_OUTPUT:
		case WPC_SOL_FLASH1_OUTPUT:
		case WPC_SOL_LOWPOWER_OUTPUT:
			linux_solenoid_outputs[addr-WPC_SOL_FLASH2_OUTPUT] = val;
			break;

		case WPC_LAMP_ROW_OUTPUT:
			*linux_lamp_data_ptr = val;
			break;

		case WPC_LAMP_COL_STROBE:
			if (val != 0)
				linux_lamp_data_ptr = linux_lamp_matrix + scanbit (val);
			break;

		case WPC_SW_COL_STROBE:
#if defined (MACHINE_PIC) && (MACHINE_PIC == 1)
			if ((val >= 0x16) && (val < 0x16 + 8))
				linux_switch_data_ptr = linux_switch_matrix + val - 1;
#else
			if (val != 0)
				linux_switch_data_ptr = linux_switch_matrix + 1 + scanbit (val);
#endif
			break;

		default:
			; // printf ("Error: invalid I/O write to 0x%04X, val=0x%02X\n", addr, val);
	}
}


/** Simulated read of an I/O register */
U8 linux_asic_read (U16 addr)
{
	switch (addr)
	{
		case WPC_LEDS:
			return linux_cpu_leds;
			break;

		case WPC_CLK_HOURS_DAYS:
		case WPC_CLK_MINS:
		{
			time_t now = time (NULL);
			int minutes_on = (now - linux_boot_time) / 60;
			if (addr == WPC_CLK_HOURS_DAYS)
				return minutes_on / 60;
			else
				return minutes_on % 60;
			break;
		}

		case WPC_SW_ROW_INPUT:
			return *linux_switch_data_ptr;

		case WPC_SW_JUMPER_INPUT:
			return linux_jumpers;

		case WPC_SW_CABINET_INPUT:
			return linux_switch_matrix[0];

		case WPC_PERIPHERAL_TIMER_FIRQ_CLEAR:
			return 0;

		case WPC_FLIPTRONIC_PORT_A:
			return linux_flipper_inputs;

		case WPC_ROM_BANK:
			return 0;

		default:
			; // printf ("Error: invalid I/O read from 0x%04X\n", addr);
			return 0;
	}
}


static void linux_time_step (void)
{
	++linux_irq_count;
#if 0
	printf ("irq time #%d\n", linux_irq_count);
#endif
}


/** Permanent thread that handles realtime events.
 *
 * This thread monitors system timing and invokes the interrupt handlers
 * as often as needed, to simulate what would happen in a real environment.
 */
static void linux_realtime_thread (void)
{
	int i;

	/* TODO - boost priority of this process, so that it always
	 * takes precedence over higher priority stuff. */

	task_set_flags (TASK_PROTECTED);
	for (;;)
	{
		/** Sleep until the next iteration.
		 * TODO - this should do more accurate timing, and wait slightly
		 * less, based on how long it took to do the real work. */
		task_sleep ((RT_THREAD_FREQ * TIME_33MS) / 33);

		linux_irq_pending += RT_ITERATION_IRQS;

		if (linux_irq_enable)
			while (linux_irq_pending-- > 0)
			{
				/** Advance the simulator clock */
				linux_time_step ();
	
				/** Invoke IRQ handler */
				do_irq ();
			}

		/** Check for external interrupts on the FIRQ line */
		if (linux_firq_enable)
			while (linux_firq_pending-- > 0)
			{
				do_firq ();
			}
	}
}


static void linux_switch_toggle (unsigned int sw)
{
	linux_switch_matrix[sw / 8] ^= (1 << (sw % 8));
}


static void linux_switch_depress (unsigned int sw)
{
	linux_switch_toggle (sw);
	task_sleep (TIME_100MS);
	linux_switch_toggle (sw);
}


static switchnum_t keymaps[256] = {
	['1'] = MACHINE_START_SWITCH,
#ifdef MACHINE_BUYIN_SWITCH
	['2'] = MACHINE_BUYIN_SWITCH,
#endif
	['4'] = SW_CENTER_COIN,
	['5'] = SW_RIGHT_COIN,
	['6'] = SW_FOURTH_COIN,
	['7'] = SW_ESCAPE,
	['8'] = SW_DOWN,
	['9'] = SW_UP,
	['0'] = SW_ENTER,
	[','] = SW_LL_FLIP_SW,
	['.'] = SW_LR_FLIP_SW,
	['`'] = SW_COINDOOR_CLOSED,
	[' '] = MACHINE_TILT_SWITCH,
	['!'] = MACHINE_SLAM_TILT_SWITCH,
#ifdef MACHINE_LAUNCH_SWITCH
	['\r'] = MACHINE_LAUNCH_SWITCH,
#endif
};

static void linux_interface_thread (void)
{
	char inbuf[1];
	switchnum_t sw;
	struct termios tio;

	tcgetattr (0, &tio);
	tio.c_lflag &= ~ICANON;
	tcsetattr (0, TCSANOW, &tio);

	task_set_flags (TASK_PROTECTED);
	for (;;)
	{
		// task_sleep (IF_THREAD_FREQ);
		ssize_t res = pth_read (0, inbuf, 1);
		if (res > 0)
		{
			switch (*inbuf)
			{
				case 'q':
					linux_shutdown ();
					break;

				case 'T':
					task_dump ();
					break;

				default:
					sw = keymaps[(int)*inbuf];
					if (sw)
						linux_switch_depress (sw);
					else if (*inbuf == '3')
						linux_switch_depress (SW_LEFT_COIN);
					else
						printf ("invalid key '%c' pressed (0x%02X)", 
							*inbuf, *inbuf);
			}
		}
	}
}


void linux_trough_init (int balls)
{
	int i;

	device_t *dev = &device_table[DEVNO_TROUGH];
	for (i=0; i < dev->props->sw_count && balls; i++, balls--)
		linux_switch_toggle (dev->props->sw[i]);
}



/** Initialize the Linux simulation.
 *
 * This is called during normal initialization, in place of the hardware
 * specific init.
 */
void linux_init (void)
{
	switchnum_t sw;
	
	task_create_gid (GID_LINUX_REALTIME, linux_realtime_thread);
	task_create_gid (GID_LINUX_INTERFACE, linux_interface_thread);

	/* Initialize the state of the opto switches, so that they appear
	 * reversed correctly. */
	for (sw = 0; sw < NUM_SWITCHES; sw++)
		if (switch_is_opto (sw))
			linux_switch_toggle (sw);

	/* Initial the trough to contain all the balls */
	linux_trough_init (linux_installed_balls);
}


/** Entry point into the simulation.
 *
 * This function substitutes for the reset vector being thrown on actual
 * hardware.
 */
int main (int argc, char *argv[])
{
	int argn = 1;

	/** Do initialization that the hardware would normally do, before
	 * the reset vector is invoked. */
	linux_irq_enable = linux_firq_enable = TRUE;
	linux_irq_pending = linux_firq_pending = 0;
	linux_irq_count = 0;

	linux_asic_write (WPC_LAMP_COL_STROBE, 0x1);
	linux_asic_write (WPC_SW_COL_STROBE, 0x1);
	linux_asic_write (WPC_DMD_LOW_PAGE, 0);
	linux_asic_write (WPC_DMD_HIGH_PAGE, 0);
	linux_asic_write (WPC_DMD_ACTIVE_PAGE, 0);

	linux_boot_time = time (NULL);

	/* Parse command-line arguments */
	while (argn < argc)
	{
		const char *arg = argv[argn++];
		if (!strcmp (arg, "-h"))
		{
			printf ("Syntax: freewpc [<options>]\n");
			exit (0);
		}
		else if (!strcmp (arg, "-s"))
		{
			linux_irq_multiplier = strtoul (argv[argn++], NULL, 0);
		}
		else if (!strcmp (arg, "--balls"))
		{
			linux_installed_balls = strtoul (argv[argn++], NULL, 0);
		}
	}
	
	/* Jump to the reset vector */
	do_reset ();
	return 0;
}


