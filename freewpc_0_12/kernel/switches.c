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

/**
 * \file
 * \brief Handle all switch inputs
 */

#include <freewpc.h>
#include <sys/irq.h>

/*
 * An active switch is one which has transitioned and is eligible
 * for servicing, but is undergoing additional debounce logic.
 * This struct tracks the switch number, its state, and the
 * debounce timer.
 *
 * There are a finite number of these objects; when the hardware
 * detects a transition, it will either allocate a new slot or
 * re-use an existing slot for the same switch that did not
 * complete its debounce cycle.
 */
typedef struct
{
	U8 id;
	U8 timer;
} pending_switch_t;


/* Define shorthand names for the various arrays of switch bits */
#define switch_raw_bits			switch_bits[AR_RAW]
#define switch_changed_bits	switch_bits[AR_CHANGED]
#define switch_pending_bits	switch_bits[AR_PENDING]
#define switch_queued_bits		switch_bits[AR_QUEUED]
#define switch_running_bits	switch_bits[AR_RUNNING]

/** The global array of switch bits, all in one place */
__fastram__ U8 switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];


/** Return the switch table entry for a switch */
static const switch_info_t *switch_lookup (U8 sw)
{
	extern const switch_info_t switch_table[];
	return &switch_table[sw];
}


/** Return the lamp associated with a switch */
U8 switch_lookup_lamp (const switchnum_t sw)
{
	U8 lamp;
	const switch_info_t *swinfo = switch_lookup (sw);
	lamp = swinfo->lamp;
	return lamp;
}


#ifdef DEBUGGER
void switch_check_masks (void)
{
	U8 col;
	S8 row;
	U8 opto_mask;
	U8 edge_mask;
	const U8 *mach_optos = mach_opto_mask;
	const U8 *mach_edges = mach_edge_switches;

	for (col = 0; col < SWITCH_BITS_SIZE; col++)
	{
		opto_mask = 0;
		edge_mask = 0;
		for (row = 7; row >= 0; --row)
		{
			const switch_info_t *sw = switch_lookup (MAKE_SWITCH (col,row));

			opto_mask <<= 1;
			edge_mask <<= 1;

			if (sw->flags & SW_OPTICAL)
				opto_mask |= 1;
			if (sw->flags & SW_EDGE)
				edge_mask |= 1;
		}

		if (opto_mask != *mach_optos)
		{
			dbprintf ("Column %d optos: mach %02X driver %02X read %02X\n",
				col, *mach_optos, opto_mask, switch_bits[AR_RAW][col]);
		}
		else
		{
			dbprintf ("Column %d optos OK\n", col);
		}

		if (edge_mask != *mach_edges)
		{
			dbprintf ("Column %d edges: mach %02X driver %02X\n\n",
				col, *mach_edges, edge_mask);
		}
		else
		{
			dbprintf ("Column %d edges OK\n\n", col);
		}

		mach_optos++;
		mach_edges++;
	}
}
#endif


/** Initialize the switch subsystem */
void switch_init (void)
{
	memset ((U8 *)&switch_bits[0][0], 0, sizeof (switch_bits));
}


/* Inline assembler is great for the 6809, but it won't work in simulation.
 * These macros abstract the 6809 opcodes so they can be simulated. */
#ifdef __m6809__
#define SW_LDA_IO(var)		asm __volatile__ ("lda\t" C_STRING(var))
#define SW_LDA_MEM(var)		asm __volatile__ ("lda\t%0" :: "m" (var))
#define SW_ORA_MEM(var)		asm __volatile__ ("ora\t%0" :: "m" (var))
#define SW_STA_MEM(var)		asm __volatile__ ("sta\t%0" :: "m" (var))
#define SW_COMA()          asm __volatile__ ("coma\t")

#define SW_LDB_MEM(var)		asm __volatile__ ("ldb\t%0" :: "m" (var))
#define SW_EORB_MEM(var)	asm __volatile__ ("eorb\t%0" :: "m" (var))
#define SW_STB_IO(var)		asm __volatile__ ("stb\t" C_STRING(var))
#define SW_STB_MEM(var)		asm __volatile__ ("stb\t%0" :: "m" (var))
#define SW_LDB_CONST(n)		asm __volatile__ ("ldb\t%0" :: "g" (n))
#else

static U8 areg, breg;
#define SW_LDA_MEM(var)		areg = var
#define SW_LDA_IO(var)		areg = wpc_asic_read (var)
#define SW_ORA_MEM(var)		areg |= var
#define SW_STA_MEM(var)		var = areg
#define SW_COMA()          areg = ~areg;

#define SW_LDB_MEM(var)		breg = var
#define SW_STB_MEM(var)		var = breg
#define SW_STB_IO(var)		wpc_asic_write (var, breg)
#define SW_EORB_MEM(var)	breg ^= var
#define SW_LDB_CONST(n)		breg = n
#endif


/** Macro to update the internal state of a particular switch column. */
extern inline void switch_rowpoll (const U8 col)
{
	/* Read the switch column from the hardware.
	 * Column 0 corresponds to the cabinet switches.
	 * Columns 1-8 refer to the playfield columns.
	 * It is assumed that columns are polled in order, as
	 * during the read of column N, the strobe is set so that
	 * the next read will come from column N+1.
	 */
	if (col == 0)
		SW_LDA_IO (WPC_SW_CABINET_INPUT);
	else if (col <= 8)
		SW_LDA_IO (WPC_SW_ROW_INPUT);
	else /* if (col == 9) */
#if (MACHINE_WPC95 == 1)
		SW_LDA_IO (WPC95_FLIPPER_SWITCH_INPUT);
#else
		SW_LDA_IO (WPC_FLIPTRONIC_PORT_A);
#endif

	/* Set up the column strobe for the next read (on the next
	 * iteration) */
	if (col < 8)
	{
#if defined (MACHINE_PIC) && (MACHINE_PIC == 1)
		SW_LDB_CONST (col + 0x16);
#else
		SW_LDB_CONST (1 << col);
#endif
		SW_STB_IO (WPC_SW_COL_STROBE);
	}

	/* Process the switch column.
	 * This code is written in assembler in order to use the 'a'
	 * register, which keeps this code fast.
	 * For each column, 3 different values are stored:
	 * AR_RAW - the last raw reading from the hardware
	 *
	 * AR_CHANGED - '1' means the raw reading changed since the last
	 * poll.  This is computed as the XOR of the previous two readings.
	 * The purpose of this is to perform a quick and dirty debounce.
	 *
	 * AR_PENDING - '1' means the switch changed and is waiting to be
	 * processed.  When AR_CHANGED becomes asserted, and does not
	 * assert on the following cycle, that indicates a stable change.
	 * This is how we do debouncing.  The switch bits are then
	 * ORed into this AR_PENDING.  Changes queue up here until the
	 * switch can be processed by the idle task.
	 */
	/* Load previous raw state of switch */
	SW_LDB_MEM (switch_bits[AR_RAW][col]);

	/* Save current raw state of switch */
	SW_STA_MEM (switch_bits[AR_RAW][col]);
	
	/* Did switch change? B=0: no change, B=1: change */
	SW_EORB_MEM (switch_bits[AR_RAW][col]);

	/* Did the switch change states the last time? */
	SW_LDA_MEM (switch_bits[AR_CHANGED][col]);

	/* Save current change state of switch */
	SW_STB_MEM (switch_bits[AR_CHANGED][col]);

	/* Is this a stable switch change?  This occurs when a change
	 * is followed by no-change; i.e. the current change value is 0
	 * and the previous change value is 1.  The following computes
	 * A=0: not a stable change, A=1: stable change */
	SW_COMA ();
	SW_ORA_MEM (switch_bits[AR_CHANGED][col]);
	SW_COMA ();

	/* Enqueue any stable changes into the pending array */
	SW_ORA_MEM (switch_bits[AR_PENDING][col]);
	SW_STA_MEM (switch_bits[AR_PENDING][col]);
}


/** Return TRUE if the given switch is CLOSED. */
bool switch_poll (const switchnum_t sw)
{
	register bitset p = (bitset)switch_raw_bits;
	register U8 v = sw;
	__testbit(p, v);
	return v;
}


/** Return TRUE if the given switch is an opto.  Optos
are defined in the opto mask array, which is auto generated from
the machine description. */
bool switch_is_opto (const switchnum_t sw)
{
	register bitset p = (bitset)mach_opto_mask;
	register U8 v = sw;
	__testbit(p, v);
	return v;
}


/** Return TRUE if the given switch is ACTIVE.  This takes into
 * account whether or not the switch is an opto. */
bool switch_poll_logical (const switchnum_t sw)
{
	return switch_poll (sw) ^ switch_is_opto (sw);
}


/** The realtime switch processing.
 * All that is done is to poll all of the switches and store the
 * updated values in memory.  The idle function will come along
 * later, scan them, and do any real processing. */
void switch_rtt (void)
{
	/* Poll the cabinet switches */
	switch_rowpoll (0);

	/* Poll the playfield switches */
	switch_rowpoll (1);
	switch_rowpoll (2);
	switch_rowpoll (3);
	switch_rowpoll (4);
	switch_rowpoll (5);
	switch_rowpoll (6);
	switch_rowpoll (7);
	switch_rowpoll (8);

	/* Poll the flipper switches */
	switch_rowpoll (9);
}


/** Task that performs a switch lamp pulse.
 * Some switches are inherently tied to a lamp.  When the switch
 * triggers, the lamp can be automatically flickered.  This is
 * implemented as a pseudo-lamp effect, so the true state of the
 * lamp is not disturbed. */
void switch_lamp_pulse (void)
{
	const switch_info_t * const swinfo = (switch_info_t *)task_get_arg ();

	/* Although not a true leff, this fools the lamp draw to doing
	 * the right thing. */
	task_set_thread_data (task_getpid (), L_PRIV_FLAGS, L_SHARED);

	/* If the lamp is already allocated by another lamp effect,
	then don't bother trying to do the pulse. */
	if (!lamp_leff2_test_allocated (swinfo->lamp))
	{
		/* Allocate the lamp */
		lamp_leff2_allocate (swinfo->lamp);

		/* Change the state of the lamp */
		if (lamp_test (swinfo->lamp))
			leff_off (swinfo->lamp);
		else
			leff_on (swinfo->lamp);
		task_sleep (TIME_100MS * 2);

		/* Change it back */
		leff_toggle (swinfo->lamp);
		task_sleep (TIME_100MS * 2);

		/* Free the lamp */
		lamp_leff2_free (swinfo->lamp);
	}
	task_exit ();
}


/*
 * Any switch transition is handled by this routine.  It performs
 * some of the common switch handling logic before calling the
 * switch-specific function.
 */
void switch_sched (void)
{
	const U8 sw = (U8)task_get_arg ();
	const switch_info_t * const swinfo = switch_lookup (sw);

#if 1 /* TODO */
	/* Under the newest version of gcc6809, this statement
	 * causes the compiler to crash...????  */
	dbprintf ("Handling switch #%d\n", sw);
#else
	dbprintf ("Handling switch %p\n", swinfo);
#endif

#if 0 /* not working */
	/* In test mode, always queue switch closures into the
	 * special switch test queue. */
	if (in_test)
		switch_test_add_queue (sw);
#endif

	/* Don't service switches marked SW_IN_GAME at all, if we're
	 * not presently in a game */
	if ((swinfo->flags & SW_IN_GAME) && !in_game)
	{
		dbprintf ("Not handling switch because not in game\n");
		goto cleanup;
	}

	/* Don't service switches not marked SW_IN_TEST, unless we're
	 * actually in test mode */
	if (!(swinfo->flags & SW_IN_TEST) && in_test)
	{
		dbprintf ("Not handling switch because in test mode\n");
		goto cleanup;
	}

	/* If the switch has an associated lamp, then flicker the lamp when
	 * the switch triggers. */
	if ((swinfo->lamp != 0) && in_live_game)
	{
		task_pid_t tp = task_create_gid (GID_SWITCH_LAMP_PULSE, 
			switch_lamp_pulse);
		task_set_arg (tp, (U16)swinfo);
	}

	/* If we're in a live game and the switch declares a standard
	 * sound, then make it happen. */
	if ((swinfo->sound != 0) && in_live_game)
		sound_send (swinfo->sound);

	/* If the switch declares a processing function, call it.
	 * All functions are in the EVENT_PAGE. */
	if (swinfo->fn)
	{
		call_far (EVENT_PAGE, (*swinfo->fn) ());
	}

	/* If a switch is marked SW_PLAYFIELD and we're in a game,
	 * then call the global playfield switch handler and mark
	 * the ball 'in play'.  Don't do the last bit if the switch
	 * specifically doesn't want this to happen.  Also, kick
	 * the ball search timer so that it doesn't expire.
	 */
	if ((swinfo->flags & SW_PLAYFIELD) && in_game)
	{
		callset_invoke (any_pf_switch);
		if (!(swinfo->flags & SW_NOPLAY) && !ball_in_play)
			mark_ball_in_play ();
		ball_search_timer_reset ();
	}

cleanup:
	/* If the switch is part of a device, then let the device
	 * subsystem process the event */
	if (SW_HAS_DEVICE (swinfo))
		device_sw_handler (SW_GET_DEVICE (swinfo));

	/* Debounce period after the switch has been handled.
	TODO : this is not really working now. */
	if (swinfo->inactive_time == 0)
		task_sleep (TIME_100MS * 1);
	else
		task_sleep (swinfo->inactive_time);

#if 0
	/* This code isn't needed at the moment */
	register bitset p = (bitset)switch_bits[AR_QUEUED];
	register U8 v = sw;
	__clearbit(p, v);
#endif

	task_exit ();
}


/** Idle time switch processing.
 * 'Pending switches' are scanned and handlers are spawned for each
 * of them (each is a separate task).
 */
CALLSET_ENTRY (switch, idle)
{
	U8 rawbits, pendbits;
	U8 col;
	extern U8 sys_init_complete;

	/* Prior to system initialization, switches are not serviced.
	Any switch closures during this time continued to be queued up. */
	if (sys_init_complete == 0)
		return;

	for (col = 0; col <= 9; col++)
	{
		/* Disable interrupts while fiddling with the bits */
		disable_irq ();

		/* Grab the raw bits : 0=open, 1=closed */
		rawbits = switch_bits[AR_RAW][col];

		/* Invert for optos: 0=inactive, 1=active */
		rawbits ^= mach_opto_mask[col];

		/* Convert to active level: 0=inactive, 1=active or edge */
		rawbits |= mach_edge_switches[col];

		/* Grab the current set of pending bits, masked with rawbits.
		 * pendbits is only 1 if the switch is marked pending and it
		 * is currently active.  For edge-triggered switches, it is
		 * invoked active or inactive.
		 */
		pendbits = switch_bits[AR_PENDING][col] & rawbits;

		/* Only service the active bits */
		pendbits &= rawbits;

		/* Clear the pending bits */
		switch_bits[AR_PENDING][col] = 0;

		/* Enable interrupts */
		enable_irq ();

		if (pendbits) /* Anything to be done on this column? */
		{
			/* Yes, calculate the switch number */
			U8 sw = col * 8;

			/* Iterate over all rows -- all switches on this column */
			U8 row;
			for (row=0; row < 8; row++, sw++, pendbits >>= 1)
			{
				if (pendbits & 1)
				{
					/* TODO : rather than handling the switch right away,
					 * we should queue it up, and then process the switches
					 * later in queue order.  This would solve the problem
					 * of some switch handlers taking longer to execute
					 * than others.
					 */
					task_pid_t tp = task_create_gid (GID_SW_HANDLER, switch_sched);
					task_set_arg (tp, sw);
				}
			}
		}
	}
}

