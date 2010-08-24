/*
 * Copyright 2006-2010 by Brian Dominy <brian@oddchange.com>
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
 * \brief Entry point to the builtin simulator for the WPC hardware.
 *
 * When CONFIG_NATIVE is defined at build time, access to WPC I/O registers
 * is redirected to function calls here that simulate the behavior.  This
 * allows FreeWPC to be tested directly on a Linux or Windows development machine,
 * even when there is no PinMAME.
 *
 * If CONFIG_UI is also defined, then instead of plain console output,
 * the state of the system is redrawn using the generic UI functions.
 * The current UI is implemented using ncurses, but other UI backends may
 * be added later.
 */

#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <freewpc.h>
#include "/usr/include/sys/time.h"
#include <simulation.h>

extern void do_swi3 (void);
extern void do_swi2 (void);
extern void do_firq (void);
extern void do_irq (void);
extern void do_swi (void);
extern void do_nmi (void);

extern void exit (int);

extern const device_properties_t device_properties_table[];
extern const switch_info_t switch_table[];

extern U8 *sim_switch_matrix_get (void);
extern void sim_switch_toggle (int sw);
extern int sim_switch_read (int sw);
extern void sim_switch_init (void);


/** The rate at which the simulated clock should run */
int linux_irq_multiplier = 1;

/** A pointer to the low DMD page */
U8 *linux_dmd_low_page;

/** A pointer to the high DMD page */
U8 *linux_dmd_high_page;

/** The simulated lamp matrix outputs. */
U8 linux_lamp_matrix[NUM_LAMP_COLS];

/** The simulated solenoid outputs */
U8 linux_solenoid_outputs[SOL_COUNT / 8];

/** True if the IRQ is enabled */
bool linux_irq_enable;

/** Nonzero if an IRQ is pending */
int linux_irq_pending;

/** True if the FIRQ is enabled */
bool linux_firq_enable;

volatile int sim_debug_init = 0;

/** Pointer to the current switch matrix element */
U8 *linux_switch_data_ptr;

/** Pointer to the current lamp matrix element */
U8 *linux_lamp_data_ptr;

/** Current row output register */
U8 linux_lamp_row_data;

/** The jumper settings */
U8 linux_jumpers = LC_USA_CANADA << 2;

/** The triac outputs */
U8 linux_triac_latch, linux_triac_outputs;

/** The actual time at which the simulation was started */
time_t linux_boot_time;

/** The status of the CPU board LEDs */
U8 linux_cpu_leds;

volatile U8 simulated_orkin_control_port = WPC_DEBUG_WRITE_READY;

U8 simulated_orkin_data_port = 0x0;

#ifdef MACHINE_TZ
int col9_enabled = 0;
#endif

/** The initial number of balls to 'install' as given on the command-line. */
#ifdef DEVNO_TROUGH
int linux_installed_balls = MACHINE_TROUGH_SIZE;
#else
int linux_installed_balls = 0;
#endif

/** The file descriptor to read from for input */
int linux_input_fd = 0;

/** The stream to write to for output */
FILE *linux_output_stream;

/** Debug output buffer */
char linux_debug_output_buffer[256];
char *linux_debug_output_ptr;

/** The contents of the 16 PIC serial data registers */
U8 pic_serial_data[16] = { 0, };

/** The value for the machine number to program into the PIC.
This defaults to the correct value as specified by MACHINE_NUMBER,
but it can be changed to test mismatches. */
#ifndef MACHINE_NUMBER
#define MACHINE_NUMBER 0
#endif
unsigned int pic_machine_number = MACHINE_NUMBER;

unsigned int signo_under_trace = SIGNO_SOL + 0;

const char *exec_file = NULL;

int exec_late_flag = 0;

int crash_on_error = 0;


/** A dummy function intended to be used for debugging under GDB. */
void gdb_break (void)
{
	barrier ();
}

/** Prints log messages, requested status, etc. to the console.
 * This is the only function that should use printf.
 */
void simlog (enum sim_log_class class, const char *format, ...)
{
	va_list ap;
	FILE *ofp;

	va_start (ap, format);

#ifdef CONFIG_UI
	ui_write_debug (class, format, ap);

	if (linux_output_stream == stdout)
		ofp = NULL;
	else
		ofp = linux_output_stream;

#else
	ofp = linux_output_stream;
#endif

	if (ofp)
	{
		if (class != SLC_DEBUG_PORT)
			fprintf (ofp, "[SIM] ");
		(void)vfprintf (ofp, format, ap);
		fputc ('\n', ofp);
		fflush (ofp);
	}

	va_end (ap);
}


#if (MACHINE_PIC == 1)
void simulation_pic_init (void)
{
	static U8 real_serial_number[] = { 0, 0, 0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 0, 0, 0 };
	U32 tmp;

	/* Initialize the PIC with the desired machine number. */
	real_serial_number[0] = pic_machine_number / 100;
	real_serial_number[1] = (pic_machine_number / 10) % 10;
	real_serial_number[2] = pic_machine_number % 10;

	/* TODO : Initialize the three byte switch matrix unlock code */

	/* Now encode the 17-byte serial number into the 16 PIC registers. */

	pic_serial_data[10] = 0x0;
	pic_serial_data[2] = 0x0;

	tmp = 100 * real_serial_number[1] + 10 * real_serial_number[7] +
		real_serial_number[4] + 5 * pic_serial_data[10];
	tmp = (tmp * 0x1BCD) + 0x1F3F0UL;
	pic_serial_data[1] = (tmp >> 16) & 0xFF;
	pic_serial_data[11] = (tmp >> 8) & 0xFF;
	pic_serial_data[9] = tmp  & 0xFF;

	tmp = 10000 * real_serial_number[2] + 1000 * real_serial_number[15] +
		100 * real_serial_number[0] + 10 * real_serial_number[8] +
		real_serial_number[6] + 2 * pic_serial_data[10] +
		pic_serial_data[2];
	tmp = (tmp * 0x107F) + 0x71E259UL;
	pic_serial_data[7] = (tmp >> 24) & 0xFF;
	pic_serial_data[12] = (tmp >> 16) & 0xFF;
	pic_serial_data[0] = (tmp >> 8) & 0xFF;
	pic_serial_data[8] = tmp  & 0xFF;

	tmp = 1000 * real_serial_number[16] + 100 * real_serial_number[3] +
		10 * real_serial_number[5] + real_serial_number[14] +
		pic_serial_data[2];
	tmp = (tmp * 0x245) + 0x3D74;
	pic_serial_data[3] = (tmp >> 16) & 0xFF;
	pic_serial_data[14] = (tmp >> 8) & 0xFF;
	pic_serial_data[6] = tmp  & 0xFF;

	tmp = 10000 * real_serial_number[13] + 1000 * real_serial_number[12] +
		100 * real_serial_number[11] + 10 * real_serial_number[10] +
		real_serial_number[9];
	tmp = 99999UL - tmp;
	pic_serial_data[15] = (tmp >> 8) & 0xFF;
	pic_serial_data[4] = tmp & 0xFF;
}


/** Access the simulated PIC.
 * writep is 1 if this is a write command, 0 on a read.
 * For writes, WRITE_VAL specifies the value to be written.
 *
 * For reads, it returns the read value, otherwise it returns
 * zero.
 */
U8 simulation_pic_access (int writep, U8 write_val)
{
	static U8 last_write = 0xFF;
	static int writes_until_unlock_needed = 1000;
	static int unlock_mode = 0;
	static U32 unlock_code;
	const U32 expected_unlock_code = 0;

	if (writep)
	{
		/* Handles writes to the PIC */
		if (last_write == 0xFF && write_val != WPC_PIC_RESET)
		{
			simlog (SLC_DEBUG, "PIC write %02X before reset.", write_val);
		}
		else if (unlock_mode > 0)
		{
			unlock_code = (unlock_code << 8) | write_val;
			if (++unlock_mode > 3)
			{
				if (unlock_code != expected_unlock_code)
				{
					static int already_warned = 0;
					if (!already_warned)
						simlog (SLC_DEBUG, "Invalid PIC unlock code %X (expected %X)\n",
							unlock_code, expected_unlock_code);
					already_warned = 1;
					unlock_mode = -1;
				}
				else
				{
					unlock_mode = 0;
				}
			}
		}
		else if (write_val == WPC_PIC_UNLOCK)
		{
			unlock_mode = 1;
			unlock_code = 0;
		}
		else
		{
			last_write = write_val;
			if (writes_until_unlock_needed > 0)
				writes_until_unlock_needed--;
		}
		return 0;
	}
	else
	{
		/* Handles reads to the PIC */
		switch (last_write)
		{
			case WPC_PIC_RESET:
			case WPC_PIC_UNLOCK:
				return 0;

			case WPC_PIC_COUNTER:
				return writes_until_unlock_needed;

			case WPC_PIC_COLUMN(0): case WPC_PIC_COLUMN(1):
			case WPC_PIC_COLUMN(2): case WPC_PIC_COLUMN(3):
			case WPC_PIC_COLUMN(4): case WPC_PIC_COLUMN(5):
			case WPC_PIC_COLUMN(6): case WPC_PIC_COLUMN(7):
			{
				unsigned int col;
				U8 val;

				if (unlock_mode > 0)
				{
					simlog (SLC_DEBUG, "Column read in unlock mode");
					return 0;
				}

				col = last_write - WPC_PIC_COLUMN(0);
				if (col >= 8)
				{
					simlog (SLC_DEBUG, "Invalid column %d", col);
					return 0;
				}

				val = sim_switch_matrix_get ()[col + 1];
				return val;
			}

			case WPC_PIC_SERIAL(0): case WPC_PIC_SERIAL(1):
			case WPC_PIC_SERIAL(2): case WPC_PIC_SERIAL(3):
			case WPC_PIC_SERIAL(4): case WPC_PIC_SERIAL(5):
			case WPC_PIC_SERIAL(6): case WPC_PIC_SERIAL(7):
			case WPC_PIC_SERIAL(8): case WPC_PIC_SERIAL(9):
			case WPC_PIC_SERIAL(10): case WPC_PIC_SERIAL(11):
			case WPC_PIC_SERIAL(12): case WPC_PIC_SERIAL(13):
			case WPC_PIC_SERIAL(14): case WPC_PIC_SERIAL(15):
				return pic_serial_data[last_write - WPC_PIC_SERIAL(0)];

			default:
				simlog (SLC_DEBUG, "Invalid PIC address read");
				return 0;
		}
	}
}
#endif /* MACHINE_PIC */

__noreturn__ void linux_shutdown (U8 error_code)
{
	simlog (SLC_DEBUG, "Shutting down simulation.");
	protected_memory_save ();
#ifdef CONFIG_UI
	ui_exit ();
#endif
	if (crash_on_error && error_code)
		*(int *)0 = 1;
	exit (error_code);
}


bool linux_switch_poll_logical (unsigned int sw)
{
	return sim_switch_read (sw) ^ switch_is_opto (sw);
}


/** Write to a multiplexed output; i.e. a register in which distinct
 * outputs are multiplexed together into a single I/O location.
 * UI_UPDATE provides a function for displaying the contents of a single
 * output; it takes the output number and a 0/1 state.
 * INDEX gives the output number of the first bit of the byte of data.
 * MEMP points to the data byte, containing 8 outputs.
 * NEWVAL is the value to be written; it is assigned to *MEMP.
 */
static void mux_write (void (*ui_update) (int, int), int index, U8 *memp, U8 newval, unsigned int sigbase)
{
	U8 oldval = *memp;
	int n;
	for (n = 0; n < 8; n++)
	{
		if ((newval & (1 << n)) != (oldval & (1 << n)))
		{
#ifdef CONFIG_UI
			ui_update (index + n, newval & (1 << n));
#endif
			signal_update (sigbase+index+n, newval & (1 << n));
		}
	}
	*memp = newval;
}


/** Update the output side of the triac/flipper relay register.
 * This becomes zero when a zerocrossing occurs.  When the input side of the latch
 * is written, GI strings can be turned on but not turned off.
 */
void sim_triac_update (U8 val)
{
	mux_write (ui_write_triac, 0, &linux_triac_outputs, val, SIGNO_TRIAC);
}


/** Simulate writing to a set of 8 solenoids. */
static void sim_sol_write (int index, U8 *memp, U8 val)
{
	int n;

	/* Update the state of each solenoid from the signal coming
	into it. */
	for (n = 0; n < 8; n++)
	{
		unsigned int solno = index+n;

		if (solno < SOL_COUNT)
			sim_coil_change (solno, val & (1 << n));
	}

	/* Commit the new state */
	mux_write (ui_write_solenoid, index, memp, val, SIGNO_SOL);
}


/** Simulate the write of a WPC I/O register */
void writeb (IOPTR addr, U8 val)
{
	switch (addr)
	{
		case WPC_PARALLEL_STROBE_PORT:
		case WPC_TICKET_DISPENSE:
			break;

		case WPC_PARALLEL_DATA_PORT:
		case WPC_DEBUG_DATA_PORT:
			*linux_debug_output_ptr++ = val;
			if (val == '\n')
			{
				*--linux_debug_output_ptr = '\0';
				simlog (SLC_DEBUG_PORT, linux_debug_output_buffer);
				linux_debug_output_ptr = linux_debug_output_buffer;
			}
			break;

		/* In simulation, the hardware shifter is not used;
		rather, native shift instructions are used.   So these
		writes should never occur. */
		case WPC_SHIFTADDR:
		case WPC_SHIFTBIT:
		case WPC_SHIFTBIT2:
			fatal (ERR_CANT_GET_HERE);

#if (MACHINE_WPC95 == 1)
		case WPC95_FLIPPER_COIL_OUTPUT:
			sim_sol_write (32, &linux_solenoid_outputs[4], val);
#elif (MACHINE_FLIPTRONIC == 1)
		case WPC_FLIPTRONIC_PORT_A:
			sim_sol_write (32, &linux_solenoid_outputs[4], ~val);
#endif
			break;

		case WPC_LEDS:
			linux_cpu_leds = val;
			signal_update (SIGNO_DIAG_LED, (val & 0x80) ? 1 : 0);
			break;

		case WPC_RAM_BANK:
		case WPC_RAM_LOCK:
		case WPC_RAM_LOCKSIZE:
		case WPC_ROM_LOCK:
		case WPC_ROM_BANK:
			/* nothing to do, not implemented yet */
			break;

		case WPC_ZEROCROSS_IRQ_CLEAR:
			if ((val & 0x0F) == 6)
				sim_watchdog_reset ();
			break;

#if (MACHINE_DMD == 1)
		case WPC_DMD_LOW_PAGE:
			asciidmd_map_page (0, val);
			break;

		case WPC_DMD_HIGH_PAGE:
			asciidmd_map_page (1, val);
			break;

#if (MACHINE_WPC95 == 1)
		case WPC_DMD_3200_PAGE:
			asciidmd_map_page (3, val);
			break;

		case WPC_DMD_3000_PAGE:
			asciidmd_map_page (2, val);
			break;

		case WPC_DMD_3600_PAGE:
			asciidmd_map_page (5, val);
			break;

		case WPC_DMD_3400_PAGE:
			asciidmd_map_page (4, val);
			break;
#endif

		case WPC_DMD_ACTIVE_PAGE:
			asciidmd_set_visible (val);
			break;

		case WPC_DMD_FIRQ_ROW_VALUE:
			/* Writing to this register has no effect in
			simulation, because FIRQ is automatically asserted
			to update the DMD occasionally. */
			break;
#endif /* MACHINE_DMD */

		case WPC_GI_TRIAC:
			/* The input side of the triac has a latch; store only the G.I.
			related bits there */
			linux_triac_latch = val & TRIAC_GI_MASK;

			/* The outputs are comprised of whatever GI strings are already
			on, plus whatever outputs (GIs and relays) were just written. */
			val |= linux_triac_outputs;
			sim_triac_update (val);
			break;

		case WPC_SOL_GEN_OUTPUT:
			sim_sol_write (24, &linux_solenoid_outputs[3], val);
			break;

		case WPC_SOL_HIGHPOWER_OUTPUT:
			sim_sol_write (0, &linux_solenoid_outputs[0], val);
			break;

		case WPC_SOL_FLASHER_OUTPUT:
			sim_sol_write (16, &linux_solenoid_outputs[2], val);
			break;

		case WPC_SOL_LOWPOWER_OUTPUT:
			sim_sol_write (8, &linux_solenoid_outputs[1], val);
			break;

#ifdef MACHINE_SOL_EXTBOARD1
		case WPC_EXTBOARD1:
			sim_sol_write (40, &linux_solenoid_outputs[5], val);
			break;
#endif

#if (MACHINE_ALPHANUMERIC == 1)
		case WPC_ALPHA_POS:
			sim_seg_set_column (val);
			break;
		case WPC_ALPHA_ROW1:
			sim_seg_write (0, 0, val);
			break;
		case WPC_ALPHA_ROW1+1:
			sim_seg_write (0, 1, val);
			break;
		case WPC_ALPHA_ROW2:
			sim_seg_write (1, 0, val);
			break;
		case WPC_ALPHA_ROW2+1:
			sim_seg_write (1, 1, val);
			break;
#endif

		case WPC_LAMP_ROW_OUTPUT:
			linux_lamp_row_data = val;
			goto do_lamp_update;

		case WPC_LAMP_COL_STROBE:
			if (val != 0)
				linux_lamp_data_ptr = linux_lamp_matrix + scanbit (val);
			else
				linux_lamp_data_ptr = NULL;
do_lamp_update:
			if (linux_lamp_data_ptr != NULL)
				mux_write (ui_write_lamp, 8 * (linux_lamp_data_ptr - linux_lamp_matrix),
					linux_lamp_data_ptr, linux_lamp_row_data, SIGNO_LAMP);
			break;

#if (MACHINE_PIC == 1)
		case WPCS_PIC_WRITE:
			simulation_pic_access (1, val);
#else
		case WPC_SW_COL_STROBE:
			if (val != 0)
				linux_switch_data_ptr = sim_switch_matrix_get () + 1 + scanbit (val);
#endif
			break;

		case WPCS_DATA:
			wpc_sound_write (val);
			break;

		case WPCS_CONTROL_STATUS:
			wpc_sound_reset ();
			break;

		case WPC_PERIPHERAL_TIMER_FIRQ_CLEAR:
			hwtimer_write (val);
			break;

		default:
			printf ("Error: unhandled I/O write to 0x%04X, val=0x%02X\n", addr, val);
	}
}


/** Simulated read of an I/O register */
U8 readb (IOPTR addr)
{
	switch (addr)
	{
		case WPC_DEBUG_DATA_PORT:
			if (simulated_orkin_control_port & WPC_DEBUG_READ_READY)
			{
				simulated_orkin_control_port ^= WPC_DEBUG_READ_READY;
				return simulated_orkin_data_port;
			}
			else
				return 0;

		case WPC_DEBUG_CONTROL_PORT:
			return simulated_orkin_control_port;

		case WPC_LEDS:
			return linux_cpu_leds; /* don't think the LEDs can actually be read? */

		case WPC_CLK_HOURS_DAYS:
		case WPC_CLK_MINS:
		{
			/* The time-of-day registers return the system time of the
			simulator itself. */
			time_t now = time (NULL);
			int minutes_on =
				((now - linux_boot_time) * linux_irq_multiplier) / 60;
			if (addr == WPC_CLK_HOURS_DAYS)
				return minutes_on / 60;
			else
				return minutes_on % 60;
			break;
		}

#if (MACHINE_PIC == 1)
		case WPCS_PIC_READ:
			return simulation_pic_access (0, 0);
#else
		case WPC_SW_ROW_INPUT:
#ifdef MACHINE_TZ
			if (col9_enabled)
				return sim_switch_matrix_get ()[9]; /* TODO - not right */
			else
#endif
				return *linux_switch_data_ptr;
#endif

		case WPC_SW_JUMPER_INPUT:
			return linux_jumpers;

		case WPC_SW_CABINET_INPUT:
			return sim_switch_matrix_get ()[0];

		case WPC_PERIPHERAL_TIMER_FIRQ_CLEAR:
			return hwtimer_read ();

#if (MACHINE_WPC95 == 1)
		case WPC95_FLIPPER_SWITCH_INPUT:
				return ~sim_switch_matrix_get ()[9];
#elif (MACHINE_FLIPTRONIC == 1)
		case WPC_FLIPTRONIC_PORT_A:
				return ~sim_switch_matrix_get ()[9];
#endif

		case WPC_ROM_BANK:
		case WPC_TICKET_DISPENSE:
			return 0;

		case WPCS_CONTROL_STATUS:
			return 0;

		case WPC_ZEROCROSS_IRQ_CLEAR:
			return sim_zc_read () ? 0x80 : 0x0;

#ifdef MACHINE_SOL_EXTBOARD1
		case WPC_EXTBOARD1:
			/* TODO */
#endif
			return 0;

		default:
			printf ("Error: unhandled I/O read from 0x%04X\n", addr);
			return 0;
	}
}


/** Realtime callback function.
 *
 * This event simulates an elapsed 1ms.
 */
CALLSET_ENTRY (native, realtime_tick)
{
#define FIRQ_FREQ 8
#define PERIODIC_FREQ 16

	static unsigned long next_firq_time = FIRQ_FREQ;
	static unsigned long next_periodic_time = PERIODIC_FREQ;

	/* Update all of the simulator modules that need periodic processing */
	sim_time_step ();

	/* Simulate an IRQ every 1ms */
	if (linux_irq_enable)
		tick_driver ();

	/* Simulate an FIRQ every 8ms */
	if (linux_firq_enable)
	{
		while (realtime_read () >= next_firq_time)
		{
			do_firq ();
			next_firq_time += FIRQ_FREQ;
		}
	}

	/* Call periodic processes every 16ms */
	if (realtime_read () >= next_periodic_time)
	{
		db_periodic ();
		if (likely (periodic_ok))
			do_periodic ();
		next_periodic_time += PERIODIC_FREQ;
	}
}


/** A mapping from keyboard command to switch */
static switchnum_t keymaps[256] = {
#ifdef MACHINE_START_SWITCH
	['1'] = MACHINE_START_SWITCH,
#endif
#ifdef MACHINE_BUYIN_SWITCH
	['2'] = MACHINE_BUYIN_SWITCH,
#endif
	/* '3': SW_LEFT_COIN is omitted on purpose... see below */
	['4'] = SW_CENTER_COIN,
	['5'] = SW_RIGHT_COIN,
	['6'] = SW_FOURTH_COIN,
	['7'] = SW_ESCAPE,
	['8'] = SW_DOWN,
	['9'] = SW_UP,
	['0'] = SW_ENTER,
	[','] = SW_LEFT_BUTTON,
	['.'] = SW_RIGHT_BUTTON,
	['-'] = SW_COIN_DOOR_CLOSED,
#ifdef MACHINE_TILT_SWITCH
	['T'] = MACHINE_TILT_SWITCH,
#endif
#ifdef MACHINE_SLAM_TILT_SWITCH
	['!'] = MACHINE_SLAM_TILT_SWITCH,
#endif
#ifdef MACHINE_LAUNCH_SWITCH
	[' '] = MACHINE_LAUNCH_SWITCH,
#endif
};


void linux_key_install (char key, unsigned int swno)
{
	keymaps[(int)key] = swno;
}


/** Read a character from the keyboard.
 * If input is closed, shutdown the program. */
char linux_interface_readchar (void)
{
	char inbuf;
	ssize_t res = pth_read (linux_input_fd, &inbuf, 1);
	if (res <= 0)
	{
		task_sleep_sec (2);
		linux_shutdown (0);
	}
	return inbuf;
}


/** Turn on/off keybuffering.  Pass a zero to put the
console in raw mode, so keystrokes are not echoed.
Pass nonzero flag to go back to the default mode. */
static void keybuffering (int flag)
{
   struct termios tio;

   tcgetattr (0, &tio);
   if (!flag) /* 0 = no buffering = not default */
      tio.c_lflag &= ~ICANON;
   else /* 1 = buffering = default */
      tio.c_lflag |= ICANON;
   tcsetattr (0, TCSANOW, &tio);
}


/** Main loop for handling the user interface. */
static void linux_interface_thread (void)
{
	char inbuf[2];
	switchnum_t sw;
	int simulator_keys = 1;
	int toggle_mode = 1;

	/* Put stdin in raw mode so that 'enter' doesn't have to
	be pressed after each keystroke. */
	keybuffering (0);

	/* Let the system initialize before accepting keystrokes */
	task_sleep_sec (3);

	if (exec_file && exec_late_flag)
		exec_script_file (exec_file);

	for (;;)
	{
#ifdef CONFIG_GTK
		gtk_poll ();
		task_yield ();
#else
		*inbuf = linux_interface_readchar ();

		/* If switch simulation is turned off, then keystrokes
		are fed directly into the runtime debugger. */
		if (simulator_keys == 0)
		{
			/* Except tilde turns it off as usual. */
			if (*inbuf == '`')
			{
				simlog (SLC_DEBUG, "Input directed to switch matrix.");
				simulator_keys ^= 1;
			}
			else if ((simulated_orkin_control_port & WPC_DEBUG_READ_READY) == 0)
			{
				simulated_orkin_control_port |= WPC_DEBUG_READ_READY;
				simulated_orkin_data_port = *inbuf;
			}
			continue;
		}

		switch (*inbuf)
		{
			case '\r':
			case '\n':
				break;

			case ':':
			{
				/* Read and execute a script command */
				char cmd[128];
				char *p = cmd;

				memset (p, 0, 128);
				ui_print_command (" ");
				for (;;)
				{
					*p = linux_interface_readchar ();
					if (*p == '\x1B')
					{
						break;
					}
					else if (*p == '\010')
					{
						*p = '\0';
						p--;
					}
					else if ((*p == '\r') || (*p == '\n'))
					{
						*p = '\0';
						exec_script (cmd);
						break;
					}
					ui_print_command (cmd);
					p++;
				}
				ui_print_command ("");
				break;
			}

			case 'C':
				gdb_break ();
				break;

			case '{':
				signal_trace_start (signo_under_trace);
				break;

			case '}':
				signal_trace_stop (signo_under_trace);
				break;

			case 'q':
				node_kick (&open_node);
				break;

#if MAX_DEVICES >= 1
			case 'w':
				node_move (&device_nodes[1], &open_node);
				break;
#endif
#if MAX_DEVICES >= 2
			case 'e':
				node_move (&device_nodes[2], &open_node);
				break;
#endif
#if MAX_DEVICES >= 3
			case 'r':
				node_move (&device_nodes[3], &open_node);
				break;
#endif
#if MAX_DEVICES >= 4
			case 't':
				node_move (&device_nodes[3], &open_node);
				break;
#endif

#ifndef MACHINE_LAUNCH_SWITCH
			case ' ':
				node_move (&open_node, &shooter_node);
				break;
#endif

			case '`':
				/* The tilde toggles between keystrokes being treated as switches,
				and as input into the runtime debugger. */
				simulator_keys ^= 1;
				simlog (SLC_DEBUG, "Input directed to built-in debugger.");
				break;

			case '\x1b':
				linux_shutdown (0);
				break;

			case 'T':
				task_dump ();
				break;

			case 'S':
				*inbuf = linux_interface_readchar ();
				task_sleep_sec (*inbuf - '0');
				break;

			case '+':
				inbuf[0] = linux_interface_readchar ();
				inbuf[1] = linux_interface_readchar ();

				if (inbuf[0] == 'D')
					sw = inbuf[1] - '1';
				else if (inbuf[0] == 'F')
					sw = (inbuf[1] - '1')
						+ NUM_PF_SWITCHES + NUM_DEDICATED_SWITCHES;
				else
					sw = (inbuf[0] - '1') * 8 + (inbuf[1] - '1');
				sim_switch_depress (sw);
				break;

			case '3':
				sim_switch_depress (SW_LEFT_COIN);
				break;

			case '#':
				/* Treat '#' as a comment until end of line.
				This is useful for creating scripts. */
				do {
					*inbuf = linux_interface_readchar ();
				} while (*inbuf != '\n');
				break;

			case '"':
				simlog (SLC_DEBUG, "next key will toggle, not press");
				toggle_mode = 0;
				break;

			default:
				/* For all other keystrokes, use the keymap table
				to turn the keystroke into a switch trigger. */
				sw = keymaps[(int)*inbuf];
				if (sw)
				{
					if ((switch_table[sw].flags & SW_EDGE) || !toggle_mode)
					{
						simlog (SLC_DEBUG, "switch %d toggled",  sw);
						sim_switch_toggle (sw);
						toggle_mode = 1;
					}
#if (MACHINE_FLIPTRONIC == 1)
					else if (sw >= 72)
					{
						flipper_button_depress (sw);
					}
#endif
					else
					{
						sim_switch_depress (sw);
					}
				}
				else
					simlog (SLC_DEBUG, "invalid key '%c' pressed (0x%02X)",
						*inbuf, *inbuf);
			}
#endif
	}
}


/** Initialize the Linux simulation.
 *
 * This is called during normal initialization, during the hardware
 * bringup.  This performs final initialization before the system
 * is ready.
 */
void linux_init (void)
{
	void realtime_loop (void);

	/* This is done here, because the task subsystem isn't ready
	inside main () */
	task_create_gid_while (GID_LINUX_REALTIME, realtime_loop, TASK_DURATION_INF);
	task_create_gid_while (GID_LINUX_INTERFACE, linux_interface_thread, TASK_DURATION_INF);

	/* Initial the trough to contain all the balls.  By default,
	 * it will fill the trough, based on its actual size.  You
	 * can use the --balls option to override this. */
	node_init ();
	linux_boot_time = time (NULL);
}


void malloc_init (void)
{
}


/** Entry point into the simulation.
 *
 * This function substitutes for the reset vector being thrown on actual
 * hardware.
 */
int main (int argc, char *argv[])
{
	int argn = 1;

	/* Parse command-line arguments */
	linux_output_stream = stdout;

	while (argn < argc)
	{
		const char *arg = argv[argn++];
		if (!strcmp (arg, "-h"))
		{
			printf ("Syntax: freewpc [<options>]\n");
			printf ("-f <file>           Read input commands from file (default : stdin)\n");
			printf ("-o <file>           Log debug messages to file (default : stdout)\n");
			printf ("--debuginit         Wait for GDB attach during init (default: no)\n");
			printf ("--exec <file>       Read script commands from file\n");
			exit (0);
		}
		else if (!strcmp (arg, "-f"))
		{
			linux_input_fd = open (argv[argn++], O_RDONLY);
		}
		else if (!strcmp (arg, "-o"))
		{
			linux_output_stream = fopen (argv[argn++], "w");
			if (linux_output_stream == NULL)
			{
				printf ("Error: could not open log file\n");
				exit (1);
			}
		}
		else if (!strcmp (arg, "--debuginit"))
		{
			sim_debug_init = 1;
		}
		else if (!strcmp (arg, "--exec"))
		{
			exec_file = argv[argn++];
		}
		else if (!strcmp (arg, "--late"))
		{
			exec_late_flag = 1;
		}
		else if (!strcmp (arg, "--error-crash"))
		{
			crash_on_error = 1;
		}
		else if (strchr (arg, '='))
		{
			char varval[64];
			unsigned int val;
			char *s;

			strcpy (varval, arg);
			s = strchr (varval, '=');
			*s = '\0';
			val = strtoul (s+1, NULL, 0);
			//conf_set_later (varval, val);
		}
		else
		{
			printf ("invalid argument %s\n", arg);
			exit (1);
		}
	}

#ifdef CONFIG_UI
	/* Initialize the user interface */
#ifdef CONFIG_GTK
	gtk_init (&argc, &argv);
#endif
	ui_init ();
#endif

	/* Initialize signal tracker */
	signal_init ();

	/** Do initialization that the hardware would normally do before
	 * the reset vector is invoked. */
	signal_update (SIGNO_RESET, 1);
	linux_irq_enable = linux_firq_enable = TRUE;
	linux_irq_pending = 0;
	linux_debug_output_ptr = linux_debug_output_buffer;
	simulated_orkin_control_port = 0;
	sim_zc_init ();
#if (MACHINE_PIC == 1)
	simulation_pic_init ();
#endif
	sim_watchdog_init ();
#if (MACHINE_DMD == 1)
	asciidmd_init ();
#endif

	/* Set the hardware registers to their initial values. */
	writeb (WPC_LAMP_COL_STROBE, 0);
#if !(MACHINE_PIC == 1)
	writeb (WPC_SW_COL_STROBE, 0);
#endif
#if (MACHINE_DMD == 1)
	writeb (WPC_DMD_LOW_PAGE, 0);
	writeb (WPC_DMD_HIGH_PAGE, 0);
	writeb (WPC_DMD_ACTIVE_PAGE, 0);
#endif

	/* Initialize the state of the switches; optos are backwards */
	sim_switch_init ();

	/* Force always closed */
	sim_switch_toggle (SW_ALWAYS_CLOSED);

	/* Close the coin door */
	sim_switch_toggle (SW_COIN_DOOR_CLOSED);

	/* Load the protected memory area */
	protected_memory_load ();

	/* Initialize the simulated ball tracker */
	sim_coil_init ();

	/* Invoke the machine-specific simulation function */
#ifdef CONFIG_MACHINE_SIMULATOR
	(*CONFIG_MACHINE_SIMULATOR) ();
#endif

	/* Jump to the reset function */
	while (sim_debug_init)
		usleep (10000);

	signal_update (SIGNO_RESET, 0);
	signal_update (SIGNO_BLANKING, 1);
	signal_update (SIGNO_5V, 1);
	signal_update (SIGNO_12V, 1);
	signal_update (SIGNO_18V, 1);
	signal_update (SIGNO_20V, 1);
	signal_update (SIGNO_50V, 1);

	/* Create more conf knobs */
	conf_add ("jumpers", &linux_jumpers);
	conf_add ("pic.gameno", &pic_machine_number);
	conf_add ("balls", &linux_installed_balls);
	conf_add ("sim.speed", &linux_irq_multiplier);

	/* Execute default script file.  First, load any global
	configuration in freewpc.conf.  Then, try to load a
	game-specific file, based on its shortname.  Last,
	execute any file provided on the command line. */
	exec_script_file ("conf/freewpc.conf");
	exec_script_file ("conf/" MACHINE_SHORTNAME ".conf");
	if (exec_file && !exec_late_flag)
		exec_script_file (exec_file);

	freewpc_init ();
	return 0;
}


