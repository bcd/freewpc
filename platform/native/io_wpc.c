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

#include <freewpc.h>
#include <simulation.h>
#include <hwsim/io.h>
#include <hwsim/matrix.h>


/** The simulated solenoid outputs */
static U8 sim_sols[SOL_COUNT / 8];

/** Pointer to the current switch matrix element */
static U8 *sim_switch_data_ptr;

/** The jumper settings */
static int sim_jumpers;

/** The triac outputs */
U8 linux_triac_latch, linux_triac_outputs;


struct wpc_debug_port
{
	U8 ctrl_reg;
	U8 in_reg;
	char outbuf[256];
	char *outptr;
};

struct wpc_debug_port wpc_debug_port;

static void wpc_io_debug_init (struct wpc_debug_port *port)
{
	port->ctrl_reg = WPC_DEBUG_WRITE_READY;
	port->in_reg = 0;
	port->outptr = port->outbuf;
}

/* Handle parallel/serial port writes */
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


/*	A generic I/O handler that plugs into a configuration variable.
	VALP points to an 'int' which has been passed to conf_add to
	allow it to be set using a conf file or the 'set' command.
	ADDR is not used.  Pass this as the reader argument to io_add()
	to allow the running CPU to get to the config data. */
static U8 io_conf_reader (int *valp, unsigned int addr)
{
	return (U8)*valp;
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

void io_matrix_writer (struct io_matrix *mx, U8 val)
{
	mx->rowlatch = val;
}

U8 io_matrix_reader (struct io_matrix *mx)
{
	return mx->rowlatch;
}



struct io_switch_matrix sim_switch_matrix;
struct io_lamp_matrix sim_lamp_matrix;


#if 0
/* TODO */
struct wpc_io_sound_board
{
};

struct wpc_io_dmd_board
{
};

struct wpc_io_alpha_board
{
};
#endif

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
void sim_sol_write (int index, U8 *memp, U8 val)
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

		case WPC_LAMP_ROW_OUTPUT:
			io_matrix_writer (&sim_lamp_matrix.header, val);
			break;

		case WPC_LAMP_COL_STROBE:
			io_matrix_strobe (&sim_lamp_matrix.header, val, ui_write_lamp, SIGNO_LAMP);
			break;

#if (MACHINE_PIC == 1)
		case WPCS_PIC_WRITE:
			simulation_pic_access (1, val);
#else
		case WPC_SW_COL_STROBE:
			if (val != 0)
				sim_switch_data_ptr = sim_switch_matrix_get () + 1 + scanbit (val);
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
	}
}


U8 wpc_read (void *unused, unsigned int addr)
{
	switch (addr)
	{
		case WPCS_DATA:
		case WPCS_CONTROL_STATUS:
			return 0;

		case WPC_DEBUG_CONTROL_PORT:
			return wpc_debug_port.ctrl_reg;

		case WPC_CLK_HOURS_DAYS:
		case WPC_CLK_MINS:
		{
			/* The time-of-day registers return the system time of the
			simulator itself. */
			int minutes_on = sim_get_wall_clock ();
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
			return *sim_switch_data_ptr;
#endif

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

		case WPC_ZEROCROSS_IRQ_CLEAR:
			return sim_zc_read () ? 0x80 : 0x0;

		default:
			return 0;
	}
}


/**
 * Initialize the WPC I/O.  Map WPC I/O addresses to functions that will
 * simulate them.
 */
void io_wpc_init (void)
{
	/* TODO: This could be much more efficient. The I/O system lets you
	map different handlers to different addresses.  The old I/O used a
	single callback.  The old handlers are being used as-is for now, so
	only a single io_add is done.  This should be broken up into multiple
	adds below.  Anything after this call will override. */
	io_add (MIN_IO_ADDR, MAX_IO_ADDR, wpc_read, wpc_write, NULL);

	/* Install solenoid I/O handlers */
	io_add (WPC_SOL_HIGHPOWER_OUTPUT, 1, io_null_reader, wpc_write_sol, &sim_sols[0]);
	io_add (WPC_SOL_LOWPOWER_OUTPUT, 1, io_null_reader, wpc_write_sol, &sim_sols[1]);
	io_add (WPC_SOL_FLASHER_OUTPUT, 1, io_null_reader, wpc_write_sol, &sim_sols[2]);
	io_add (WPC_SOL_GEN_OUTPUT, 1, io_null_reader, wpc_write_sol, &sim_sols[3]);
#ifdef MACHINE_SOL_EXTBOARD1
	io_add (WPC_EXTBOARD1, 1, io_null_reader, wpc_write_sol, &sim_sols[5]);
#endif

	/* Install parallel/serial port handlers */
	wpc_io_debug_init (&wpc_debug_port);
	io_add (WPC_PARALLEL_DATA_PORT, 1, io_null_reader, wpc_write_debug, &wpc_debug_port);
	io_add (WPC_DEBUG_DATA_PORT, 1, wpc_read_debug, wpc_write_debug, &wpc_debug_port);

	/* Install diagnostic LED handler */
	io_add (WPC_LEDS, 1, io_null_reader, wpc_write_led, NULL);

	/* Install jumper/DIP switch handler */
	sim_jumpers = LC_USA_CANADA << 2;
	conf_add ("jumpers", &sim_jumpers);
	io_add (WPC_SW_JUMPER_INPUT, 1, io_conf_reader, io_null_writer, &sim_jumpers);
}

