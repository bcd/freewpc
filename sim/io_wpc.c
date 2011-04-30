/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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


/* This file describes the mapping of WPC I/O addresses to the logic
 * that simulates them. */

#include <freewpc.h>
#include <simulation.h>
#include <hwsim/io.h>
#include <hwsim/matrix.h>
#include <hwsim/triac.h>
#include <hwsim/sound-ext.h>

#define MINS_PER_HOUR 60

/*	The state of the debug port.
	Values written to the port are buffered in 'outbuf' until a newline is seen,
	then the entire buffer is written out to the UI.
	Values read from the port are served by the keyboard module. */

struct wpc_debug_port
{
	U8 ctrl_reg;
	U8 in_reg;
	char outbuf[256];
	char *outptr;
};


/** The simulated solenoid outputs */
static U8 sim_sols[PINIO_NUM_SOLS / 8];

/** Pointer to the current switch matrix element */
static U8 *sim_switch_data_ptr;

/** The jumper settings */
static int sim_jumpers;

/** The triac outputs */
U8 linux_triac_latch, linux_triac_outputs;

/** The simulated debug port */
struct wpc_debug_port wpc_debug_port;

/** The triac that fronts the GI strings */
struct sim_triac wpc_triac;

/** The flipper relay on non-Fliptronic games */
#ifdef CONFIG_FLIPTRONIC
static U8 wpc_flipper_relay;
#endif



static void wpc_io_debug_init (struct wpc_debug_port *port)
{
	port->ctrl_reg = WPC_DEBUG_WRITE_READY;
	port->in_reg = 0;
	port->outptr = port->outbuf;
}

/* Handle parallel/serial port reads and writes */
static void wpc_write_debug (struct wpc_debug_port *port, unsigned int addr, U8 val)
{
	*(port->outptr)++ = val;
	if (val == '\n')
	{
		*--port->outptr = '\0';
		simlog (SLC_DEBUG_PORT, port->outbuf);
		port->outptr = port->outbuf;
	}
}

static U8 wpc_read_debug (struct wpc_debug_port *port, unsigned int addr)
{
	port->ctrl_reg &= ~WPC_DEBUG_READ_READY;
	return port->in_reg;
}

static U8 wpc_read_debug_status (struct wpc_debug_port *port, unsigned int addr)
{
	return port->ctrl_reg;
}



/*	A generic I/O handler that plugs into a configuration variable.
	VALP points to an 'int' which has been passed to conf_add to
	allow it to be set using a conf file or the 'set' command.
	ADDR is not used.  Pass this as the reader argument to io_add()
	to allow the running CPU to get to the config data. */
static U8 io_conf_reader (int *valp, unsigned int addr)
{
	return (U8)*valp;
}


/* A generic I/O handlers that plugs into a simple byte variable.
	The CPU "reads" and "writes" the value of the memory directly. */

static U8 io_mem_reader (U8 *valp, unsigned int addr)
{
	return valp[addr];
}

static void io_mem_writer (U8 *valp, unsigned int addr, U8 val)
{
	valp[addr] = val;
}


/* Generic lamp and switch matrix handling */

/** Write to a multiplexed output; i.e. a register in which distinct
 * outputs are multiplexed together into a single 8-bit I/O location.
 * UI_UPDATE provides a function for displaying the contents of a single
 * output; it takes the output number and a zero(off)/non-zero(on) state.
 * INDEX gives the output number of the first bit of the byte of data.
 * MEMP points to the data byte, containing 8 outputs.
 * NEWVAL is the value to be written; it is assigned to *MEMP.
 */
void mux_write (mux_ui ui_update, int index, U8 *memp, U8 newval, unsigned int sigbase)
{
	U8 oldval = *memp;
	int n;
	for (n = 0; n < 8; n++)
	{
		if ((newval & (1 << n)) != (oldval & (1 << n)))
		{
			/* Update the user interface to reflect the change in output */
#ifdef CONFIG_UI
			if (ui_update)
				ui_update (index + n, newval & (1 << n));
#endif

			/* Notify the signal tracker that the output changed */
			signal_update (sigbase+index+n, newval & (1 << n));
		}
	}

	/* Latch the write; save the value written */
	*memp = newval;
}


void io_matrix_strobe (struct io_matrix *mx, U8 val, mux_ui ui_update, unsigned int signo)
{
	if (val == 0)
		mx->rowptr = NULL;
	else
	{
		mx->rowptr = mx->rowdata + scanbit (val);
		mux_write (ui_update, 8 * (mx->rowptr - mx->rowdata), mx->rowptr, mx->rowlatch, SIGNO_LAMP);
	}
}

void io_lamp_matrix_strobe (struct io_matrix *mx, unsigned int addr, U8 val)
{
	io_matrix_strobe (mx, val, ui_write_lamp, SIGNO_LAMP);
}

void io_matrix_writer (struct io_matrix *mx, unsigned int addr, U8 val)
{
	mx->rowlatch = val;
}

U8 io_matrix_reader (struct io_matrix *mx, unsigned int addr)
{
	return mx->rowlatch;
}



struct io_switch_matrix sim_switch_matrix;

struct io_lamp_matrix sim_lamp_matrix;



/*	Handle a key press event to be sent as input to the target.
	This is called by the simulator to queue a key event for the
	target CPU to read later. */
void wpc_key_press (char val)
{
	struct wpc_debug_port *port = &wpc_debug_port;
	if ((port->ctrl_reg & WPC_DEBUG_READ_READY) == 0)
	{
		port->ctrl_reg |= WPC_DEBUG_READ_READY;
		port->in_reg = val;
	}
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

		if (solno < PINIO_NUM_SOLS)
			sim_coil_change (solno, val & (1 << n));
	}

	/* Commit the new state */
	mux_write (ui_write_solenoid, index, memp, val, SIGNO_SOL);
}


/* Handle solenoid writes */
static void wpc_write_sol (U8 *memp, unsigned int addr, U8 val)
{
	int bankno = memp - sim_sols;
	sim_sol_write (bankno * 8, memp, val);
}

/* Handle the diagnostic LED */
static void wpc_write_led (void *unused1, unsigned int unused2, U8 val)
{
	signal_update (SIGNO_DIAG_LED, (val & 0x80) ? 1 : 0);
}

/* Handle the miscellaneous I/O */

static U8 wpc_misc_read (void *unused1, unsigned int unused2)
{
	return sim_zc_read () ? 0x80 : 0x0;
}

static void wpc_misc_write (void *unused1, unsigned int unused2, U8 val)
{
	if ((val & 0x0F) == 6)
		sim_watchdog_reset ();
}

/* Handle I/O the old way */

void wpc_write (void *unused, unsigned int addr, U8 val)
{
	switch (addr)
	{
#if (MACHINE_WPC95 == 1)
		case WPC95_FLIPPER_COIL_OUTPUT:
			sim_sol_write (32, &sim_sols[4], val);
#elif (MACHINE_FLIPTRONIC == 1)
		case WPC_FLIPTRONIC_PORT_A:
			sim_sol_write (32, &sim_sols[4], ~val);
#endif
			break;

		case WPC_GI_TRIAC:
			/* The input side of the triac has a latch; store only the G.I.
			related bits there */
			linux_triac_latch = val & PINIO_GI_STRINGS;

			/* The outputs are comprised of whatever GI strings are already
			on, plus whatever outputs (GIs and relays) were just written. */
			val |= linux_triac_outputs;
			sim_triac_update (val);
			break;

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


#if (MACHINE_PIC == 1)
		case WPCS_PIC_WRITE:
			simulation_pic_access (1, val);
#else
		case WPC_SW_COL_STROBE:
			if (val != 0)
				sim_switch_data_ptr = sim_switch_matrix_get () + 1 + scanbit (val);
#endif
			break;

	}
}


U8 wpc_clock_reader (void *unused, unsigned int reg)
{
	unsigned int minutes_on = sim_get_wall_clock ();
	/* The time-of-day registers return the system time of the
	simulator itself. */
	if (reg == 0)
		return minutes_on / MINS_PER_HOUR;
	else
		return minutes_on % MINS_PER_HOUR;
}


U8 wpc_timer_reader (void *unused, unsigned int reg)
{
	return 0;
}


U8 wpc_read (void *unused, unsigned int addr)
{
	switch (addr)
	{
#if (MACHINE_PIC == 1)
		case WPCS_PIC_READ:
			return simulation_pic_access (0, 0);
#else
		case WPC_SW_ROW_INPUT:
			return *sim_switch_data_ptr;
#endif

#if (MACHINE_WPC95 == 1)
		case WPC95_FLIPPER_SWITCH_INPUT:
			return ~sim_switch_matrix_get ()[9];
#elif (MACHINE_FLIPTRONIC == 1)
		case WPC_FLIPTRONIC_PORT_A:
			return ~sim_switch_matrix_get ()[9];
#endif

		default:
			return 0;
	}
}



static void io_add_direct_switches (IOPTR addr, U8 switchno)
{
	io_add_ro (addr, io_mem_reader, sim_switch_matrix_get () + (switchno / 8));
}

static void io_add_switch_matrix (IOPTR addr_strobe, IOPTR addr_input, U8 switchno)
{
}

static void io_add_lamp_matrix (IOPTR addr_strobe, IOPTR addr_output, U8 lampno)
{
	io_add_wo (addr_output, io_matrix_writer, &sim_lamp_matrix.header);
	io_add_wo (addr_strobe, io_lamp_matrix_strobe, &sim_lamp_matrix.header);
}


/**
 * Map a range of solenoid sets.
 * ADDR gives the CPU's register that it uses to write to the set.
 * SOLNO gives the solenoid number of the first solenoid in the set.
 */
static void io_add_sol_bank (IOPTR addr, U8 solno)
{
	io_add_wo (addr, wpc_write_sol, &sim_sols[solno / 8]);
}

/* Handle writes to map a page of DMD memory into a CPU window.
	ADDR identifies the mapping number.  VAL is the physical page. */

static void io_dmd_write_map (void *window, unsigned int addr, U8 val)
{
	asciidmd_map_page ((int)window, val);
}

static void io_dmd_write_visible (void *unused1, unsigned int unused2, U8 val)
{
	asciidmd_set_visible (val);
}

static void io_add_dmd_mapping_reg (IOPTR addr, unsigned int window)
{
	io_add_wo (addr, io_dmd_write_map, (void *)window);
}

static void io_add_dmd_visible_reg (IOPTR addr)
{
	io_add_wo (addr, io_dmd_write_visible, NULL);
}



/**
 * Initialize the WPC I/O.  Map WPC I/O addresses to functions that will
 * simulate them.
 */
void io_wpc_init (void)
{
	/* TODO: Continue breaking this up into multiple adds below */
	io_add (MIN_IO_ADDR, MAX_IO_ADDR, wpc_read, wpc_write, NULL);

	/* Install miscellaneous I/O handlers */
	io_add_rw (WPC_ZEROCROSS_IRQ_CLEAR, wpc_misc_read, wpc_misc_write, NULL);

	/* Install switch handlers */
	io_add_direct_switches (WPC_SW_CABINET_INPUT, SW_LEFT_COIN);

	/* Install lamp handlers */
	io_add_lamp_matrix (WPC_LAMP_COL_STROBE, WPC_LAMP_ROW_OUTPUT, 0);

	/* Install solenoid I/O handlers */
	io_add_sol_bank (WPC_SOL_HIGHPOWER_OUTPUT, SOL_BASE_HIGH);
	io_add_sol_bank (WPC_SOL_LOWPOWER_OUTPUT, SOL_BASE_LOW);
	io_add_sol_bank (WPC_SOL_FLASHER_OUTPUT, SOL_BASE_GENERAL);
	io_add_sol_bank (WPC_SOL_GEN_OUTPUT, SOL_BASE_AUXILIARY);
#ifdef MACHINE_SOL_EXTBOARD1
	io_add_sol_bank (WPC_EXTBOARD1, SOL_BASE_EXTENDED);
#endif

	/* Install dot matrix register handlers */
#if (MACHINE_DMD == 1)
	io_add_dmd_visible_reg (WPC_DMD_ACTIVE_PAGE);
	io_add_dmd_mapping_reg (WPC_DMD_LOW_PAGE, 0);
	io_add_dmd_mapping_reg (WPC_DMD_HIGH_PAGE, 1);
#if (MACHINE_WPC95 == 1)
	io_add_dmd_mapping_reg (WPC95_DMD_3000_PAGE, 2);
	io_add_dmd_mapping_reg (WPC95_DMD_3200_PAGE, 3);
	io_add_dmd_mapping_reg (WPC95_DMD_3400_PAGE, 4);
	io_add_dmd_mapping_reg (WPC95_DMD_3600_PAGE, 5);
#endif
	/* WPC_DMD_FIRQ_ROW_VALUE is not handled in simulation */
#endif

	/* Install parallel/serial port handlers */
	wpc_io_debug_init (&wpc_debug_port);
	io_add_wo (WPC_PARALLEL_DATA_PORT, wpc_write_debug, &wpc_debug_port);
	io_add_rw (WPC_DEBUG_DATA_PORT, wpc_read_debug, wpc_write_debug, &wpc_debug_port);
	io_add_rw (WPC_DEBUG_CONTROL_PORT, wpc_read_debug_status, io_null_writer, &wpc_debug_port);

	/* Install diagnostic LED handler */
	io_add_wo (WPC_LEDS, wpc_write_led, NULL);

	/* Install jumper/DIP switch handler */
	sim_jumpers = LC_USA_CANADA << 2;
	conf_add ("jumpers", &sim_jumpers);
	io_add_ro (WPC_SW_JUMPER_INPUT, io_conf_reader, &sim_jumpers);

	/* TODO - install hwtimer read/write */

	/* Install sound board read/write */
	io_add (WPCS_DATA, 2, sound_ext_read, sound_ext_write, NULL);

	/* Install clock handler.  Since clock time comes from the native OS,
	it cannot be changed and so these are read-only registers */
	io_add (WPC_CLK_HOURS_DAYS, 2, wpc_clock_reader, io_null_writer, NULL);

	/* Install the internal timer handler */
	io_add_ro (WPC_PERIPHERAL_TIMER_FIRQ_CLEAR, wpc_timer_reader, NULL);

	/* TODO : If a ribbon cable is disconnected, then that I/O will not work. */
}

