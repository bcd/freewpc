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
__fastram__ uint8_t switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];


/*
 * Entry for unused switches.
 */

void sw_unused_handler (void)
{
	dbprintf ("Unregistered switch\n");
}

const switch_info_t sw_unused =
{
	.fn = sw_unused_handler,
	.flags = 0,
};

/*
 * If any switch entries are still undefined, then fill
 * their slots with the "unused" switch.
 */
#ifndef MACHINE_SW11
#define MACHINE_SW11 sw_unused
#endif
#ifndef MACHINE_SW12
#define MACHINE_SW12 sw_unused
#endif
#ifndef MACHINE_SW13
#define MACHINE_SW13 sw_unused
#endif
#ifndef MACHINE_SW14
#define MACHINE_SW14 sw_unused
#endif
#ifndef MACHINE_SW15
#define MACHINE_SW15 sw_unused
#endif
#ifndef MACHINE_SW16
#define MACHINE_SW16 sw_unused
#endif
#ifndef MACHINE_SW17
#define MACHINE_SW17 sw_unused
#endif
#ifndef MACHINE_SW18
#define MACHINE_SW18 sw_unused
#endif

#ifndef MACHINE_SW21
#define MACHINE_SW21 sw_unused
#endif
#ifndef MACHINE_SW22
#define MACHINE_SW22 sw_unused
#endif
#ifndef MACHINE_SW23
#define MACHINE_SW23 sw_unused
#endif
#ifndef MACHINE_SW24
#define MACHINE_SW24 sw_unused
#endif
#ifndef MACHINE_SW25
#define MACHINE_SW25 sw_unused
#endif
#ifndef MACHINE_SW26
#define MACHINE_SW26 sw_unused
#endif
#ifndef MACHINE_SW27
#define MACHINE_SW27 sw_unused
#endif
#ifndef MACHINE_SW28
#define MACHINE_SW28 sw_unused
#endif

#ifndef MACHINE_SW31
#define MACHINE_SW31 sw_unused
#endif
#ifndef MACHINE_SW32
#define MACHINE_SW32 sw_unused
#endif
#ifndef MACHINE_SW33
#define MACHINE_SW33 sw_unused
#endif
#ifndef MACHINE_SW34
#define MACHINE_SW34 sw_unused
#endif
#ifndef MACHINE_SW35
#define MACHINE_SW35 sw_unused
#endif
#ifndef MACHINE_SW36
#define MACHINE_SW36 sw_unused
#endif
#ifndef MACHINE_SW37
#define MACHINE_SW37 sw_unused
#endif
#ifndef MACHINE_SW38
#define MACHINE_SW38 sw_unused
#endif
#ifndef MACHINE_SW41
#define MACHINE_SW41 sw_unused
#endif
#ifndef MACHINE_SW42
#define MACHINE_SW42 sw_unused
#endif
#ifndef MACHINE_SW43
#define MACHINE_SW43 sw_unused
#endif
#ifndef MACHINE_SW44
#define MACHINE_SW44 sw_unused
#endif
#ifndef MACHINE_SW45
#define MACHINE_SW45 sw_unused
#endif
#ifndef MACHINE_SW46
#define MACHINE_SW46 sw_unused
#endif
#ifndef MACHINE_SW47
#define MACHINE_SW47 sw_unused
#endif
#ifndef MACHINE_SW48
#define MACHINE_SW48 sw_unused
#endif

#ifndef MACHINE_SW51
#define MACHINE_SW51 sw_unused
#endif
#ifndef MACHINE_SW52
#define MACHINE_SW52 sw_unused
#endif
#ifndef MACHINE_SW53
#define MACHINE_SW53 sw_unused
#endif
#ifndef MACHINE_SW54
#define MACHINE_SW54 sw_unused
#endif
#ifndef MACHINE_SW55
#define MACHINE_SW55 sw_unused
#endif
#ifndef MACHINE_SW56
#define MACHINE_SW56 sw_unused
#endif
#ifndef MACHINE_SW57
#define MACHINE_SW57 sw_unused
#endif
#ifndef MACHINE_SW58
#define MACHINE_SW58 sw_unused
#endif
#ifndef MACHINE_SW61
#define MACHINE_SW61 sw_unused
#endif
#ifndef MACHINE_SW62
#define MACHINE_SW62 sw_unused
#endif
#ifndef MACHINE_SW63
#define MACHINE_SW63 sw_unused
#endif
#ifndef MACHINE_SW64
#define MACHINE_SW64 sw_unused
#endif
#ifndef MACHINE_SW65
#define MACHINE_SW65 sw_unused
#endif
#ifndef MACHINE_SW66
#define MACHINE_SW66 sw_unused
#endif
#ifndef MACHINE_SW67
#define MACHINE_SW67 sw_unused
#endif
#ifndef MACHINE_SW68
#define MACHINE_SW68 sw_unused
#endif

#ifndef MACHINE_SW71
#define MACHINE_SW71 sw_unused
#endif
#ifndef MACHINE_SW72
#define MACHINE_SW72 sw_unused
#endif
#ifndef MACHINE_SW73
#define MACHINE_SW73 sw_unused
#endif
#ifndef MACHINE_SW74
#define MACHINE_SW74 sw_unused
#endif
#ifndef MACHINE_SW75
#define MACHINE_SW75 sw_unused
#endif
#ifndef MACHINE_SW76
#define MACHINE_SW76 sw_unused
#endif
#ifndef MACHINE_SW77
#define MACHINE_SW77 sw_unused
#endif
#ifndef MACHINE_SW78
#define MACHINE_SW78 sw_unused
#endif
#ifndef MACHINE_SW81
#define MACHINE_SW81 sw_unused
#endif
#ifndef MACHINE_SW82
#define MACHINE_SW82 sw_unused
#endif
#ifndef MACHINE_SW83
#define MACHINE_SW83 sw_unused
#endif
#ifndef MACHINE_SW84
#define MACHINE_SW84 sw_unused
#endif
#ifndef MACHINE_SW85
#define MACHINE_SW85 sw_unused
#endif
#ifndef MACHINE_SW86
#define MACHINE_SW86 sw_unused
#endif
#ifndef MACHINE_SW87
#define MACHINE_SW87 sw_unused
#endif
#ifndef MACHINE_SW88
#define MACHINE_SW88 sw_unused
#endif

/* Declare external references for each of the switch
 * table entries.
 */
extern const switch_info_t 
	/* Dedicated Switches */
	sw_left_coin, sw_center_coin, sw_right_coin, sw_fourth_coin,
	sw_escape_button, sw_down_button, sw_up_button, sw_enter_button,

	/* Playfield Switches */
	MACHINE_SW11, MACHINE_SW12, MACHINE_SW13, MACHINE_SW14,
	MACHINE_SW15, MACHINE_SW16, MACHINE_SW17, MACHINE_SW18,

	MACHINE_SW21, MACHINE_SW22, MACHINE_SW23, MACHINE_SW24,
	MACHINE_SW25, MACHINE_SW26, MACHINE_SW27, MACHINE_SW28,

	MACHINE_SW31, MACHINE_SW32, MACHINE_SW33, MACHINE_SW34,
	MACHINE_SW35, MACHINE_SW36, MACHINE_SW37, MACHINE_SW38,

	MACHINE_SW41, MACHINE_SW42, MACHINE_SW43, MACHINE_SW44,
	MACHINE_SW45, MACHINE_SW46, MACHINE_SW47, MACHINE_SW48,

	MACHINE_SW51, MACHINE_SW52, MACHINE_SW53, MACHINE_SW54,
	MACHINE_SW55, MACHINE_SW56, MACHINE_SW57, MACHINE_SW58,

	MACHINE_SW61, MACHINE_SW62, MACHINE_SW63, MACHINE_SW64,
	MACHINE_SW65, MACHINE_SW66, MACHINE_SW67, MACHINE_SW68,

	MACHINE_SW71, MACHINE_SW72, MACHINE_SW73, MACHINE_SW74,
	MACHINE_SW75, MACHINE_SW76, MACHINE_SW77, MACHINE_SW78,

	MACHINE_SW81, MACHINE_SW82, MACHINE_SW83, MACHINE_SW84,
	MACHINE_SW85, MACHINE_SW86, MACHINE_SW87, MACHINE_SW88,

	/* Flipper switches (EOS switches don't have handlers) */
	sw_unused, sw_right_flipper, sw_unused, sw_left_flipper,
	sw_unused, sw_upper_right_flipper, sw_unused, sw_upper_left_flipper;


static const switch_info_t *switch_table[NUM_SWITCHES] = {
	&sw_left_coin, &sw_center_coin, &sw_right_coin, &sw_fourth_coin,
	&sw_escape_button, &sw_down_button, &sw_up_button, &sw_enter_button,

	&MACHINE_SW11, &MACHINE_SW12, &MACHINE_SW13, &MACHINE_SW14,
	&MACHINE_SW15, &MACHINE_SW16, &MACHINE_SW17, &MACHINE_SW18,

	&MACHINE_SW21, &MACHINE_SW22, &MACHINE_SW23, &MACHINE_SW24,
	&MACHINE_SW25, &MACHINE_SW26, &MACHINE_SW27, &MACHINE_SW28,

	&MACHINE_SW31, &MACHINE_SW32, &MACHINE_SW33, &MACHINE_SW34,
	&MACHINE_SW35, &MACHINE_SW36, &MACHINE_SW37, &MACHINE_SW38,

	&MACHINE_SW41, &MACHINE_SW42, &MACHINE_SW43, &MACHINE_SW44,
	&MACHINE_SW45, &MACHINE_SW46, &MACHINE_SW47, &MACHINE_SW48,

	&MACHINE_SW51, &MACHINE_SW52, &MACHINE_SW53, &MACHINE_SW54,
	&MACHINE_SW55, &MACHINE_SW56, &MACHINE_SW57, &MACHINE_SW58,

	&MACHINE_SW61, &MACHINE_SW62, &MACHINE_SW63, &MACHINE_SW64,
	&MACHINE_SW65, &MACHINE_SW66, &MACHINE_SW67, &MACHINE_SW68,

	&MACHINE_SW71, &MACHINE_SW72, &MACHINE_SW73, &MACHINE_SW74,
	&MACHINE_SW75, &MACHINE_SW76, &MACHINE_SW77, &MACHINE_SW78,

	&MACHINE_SW81, &MACHINE_SW82, &MACHINE_SW83, &MACHINE_SW84,
	&MACHINE_SW85, &MACHINE_SW86, &MACHINE_SW87, &MACHINE_SW88,

	&sw_unused, &sw_right_flipper, &sw_unused, &sw_left_flipper,
	&sw_unused, &sw_upper_right_flipper, &sw_unused, &sw_upper_left_flipper,
};



inline const switch_info_t *switch_lookup (uint8_t sw)
{
	return switch_table[sw];
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
			const switch_info_t *sw = switch_table[MAKE_SWITCH (col,row)];

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

void switch_init (void)
{
	memset ((uint8_t *)&switch_bits[0][0], 0, sizeof (switch_bits));
}


extern inline void switch_rowpoll (const uint8_t col)
{
	/* Read the switch column from the hardware.
	 * Column 0 corresponds to the cabinet switches.
	 * Columns 1-8 refer to the playfield columns.
	 * It is assumed that columns are polled in order, as
	 * during the read of column N, the strobe is set so that
	 * the next read will come from column N+1.
	 */
	if (col == 0)
		asm __volatile__ ("\tlda " STR(WPC_SW_CABINET_INPUT));
	else if (col <= 8)
		asm __volatile__ ("\tlda " STR(WPC_SW_ROW_INPUT));
	else /* if (col == 9) */
#if (MACHINE_WPC95 == 1)
		asm __volatile__ ("\tlda " STR(WPC95_FLIPPER_SWITCH_INPUT));
#else
		asm __volatile__ ("\tlda " STR(WPC_FLIPTRONIC_PORT_A));
#endif

	/* Set up the column strobe for the next read (on the next
	 * iteration) */
	if (col < 8)
	{
#if defined (MACHINE_PIC) && (MACHINE_PIC == 1)
		asm __volatile__ ("\tldb\t%0" :: "g" (col + 0x16));
#else
		asm __volatile__ ("\tldb\t%0" :: "g" (1 << col));
#endif
		asm __volatile__ ("\tstb\t" STR(WPC_SW_COL_STROBE));
	}

	/* Process the switch column.
	 * This code is written in assembler in order to use the 'a'
	 * register, which keeps this code fast.
	 * For each column, 3 different values are stored:
	 * AR_RAW - the last raw reading from the hardware
	 *
	 * AR_CHANGED - '1' means the raw reading changed since the last
	 * poll.  This is computed as the XOR of the previous two readings.
	 *
	 * AR_PENDING - '1' means the switch changed and is waiting to be
	 * processed.  When AR_CHANGED becomes asserted, and does not
	 * assert on the following cycle, that indicates a stable change.
	 * This is how we do debouncing.  The switch bits are then
	 * ORed into this AR_PENDING.  Changes queue up here until the
	 * switch can be processed by the idle task.
	 */
	/* Load previous raw state of switch */
	asm __volatile__ ("\tldb\t%0" 	:: "m" (switch_bits[AR_RAW][col]));

	/* Save current raw state of switch */
	asm __volatile__ ("\tsta\t%0"		:: "m" (switch_bits[AR_RAW][col]));
	
	/* Did switch change? B=0: no change, B=1: change */
	asm __volatile__ ("\teorb\t%0"  	:: "m" (switch_bits[AR_RAW][col]));

	/* Did the switch change states the last time? */
	asm __volatile__ ("\tlda\t%0"		:: "m" (switch_bits[AR_CHANGED][col]));

	/* Save current change state of switch */
	asm __volatile__ ("\tstb\t%0"		:: "m" (switch_bits[AR_CHANGED][col]));

	/* Is this a stable switch change?  This occurs when a change
	 * is followed by no-change; i.e. the current change value is 0
	 * and the previous change value is 1.  The following computes
	 * A=0: not a stable change, A=1: stable change */
	asm __volatile__ ("\tcoma");
	asm __volatile__ ("\tora\t%0"  	:: "m" (switch_bits[AR_CHANGED][col]));
	asm __volatile__ ("\tcoma");

	/* Enqueue any stable changes into the pending array */
	asm __volatile__ ("\tora\t%0"		:: "m" (switch_bits[AR_PENDING][col]));
	asm __volatile__ ("\tsta\t%0"		:: "m" (switch_bits[AR_PENDING][col]));
}


bool switch_poll (const switchnum_t sw)
{
	register bitset p = (bitset)switch_raw_bits;
	register uint8_t v = sw;
	__testbit(p, v);
	return v;
}

bool switch_is_opto (const switchnum_t sw)
{
	register bitset p = (bitset)mach_opto_mask;
	register uint8_t v = sw;
	__testbit(p, v);
	return v;
}

bool switch_poll_logical (const switchnum_t sw)
{
	return switch_poll (sw) ^ switch_is_opto (sw);
}


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


void switch_lamp_pulse (void)
{
	const switch_info_t * const swinfo = (switch_info_t *)task_get_arg ();
	dbprintf ("Pulsing lamp %d\n", swinfo->lamp);

	lamp_leff_allocate (swinfo->lamp);
	leff_toggle (swinfo->lamp);
	task_sleep (TIME_100MS * 2);
	leff_toggle (swinfo->lamp);
	lamp_leff_free (swinfo->lamp);
	task_exit ();
}


/*
 * Any switch transition is handled by this routine.  It performs
 * some of the common switch handling logic before calling the
 * switch-specific function.
 */
void switch_sched (void)
{
	const uint8_t sw = task_get_arg ();
	const switch_info_t * const swinfo = switch_lookup (sw);

	dbprintf ("Handling switch #%d\n", sw);

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

	/* If a switch is marked SW_PLAYFIELD and we're in a game,
	 * then call the global playfield switch handler and mark
	 * the ball 'in play'.  Don't do the last bit if the switch
	 * specifically doesn't want this to happen.  Also, kick
	 * the ball search timer so that it doesn't expire.
	 */
	if ((swinfo->flags & SW_PLAYFIELD) && in_game)
	{
		call_hook(any_pf_switch);
		if (!(swinfo->flags & SW_NOPLAY) && !ball_in_play)
			mark_ball_in_play ();
		ball_search_timer_reset ();
	}

	/* TODO : not working */
	/* If the switch has an associated lamp, then flicker the lamp when
	 * the switch triggers. */
	if ((swinfo->lamp != 0) && in_live_game)
	{
		task_t *tp = task_create_gid (GID_SWITCH_LAMP_PULSE, switch_lamp_pulse);
		task_set_arg (tp, (U16)swinfo);
	}

	/* If we're in a live game and the switch declares a standard
	 * sound, then make it happen. */
	if ((swinfo->sound != 0) && in_live_game)
		sound_send (swinfo->sound);

	/* If the switch declares a processing function, call it.
	 * Note: processing functions used to be full-fledged tasks a long
	 * time ago, but not anymore.  These functions should 'return' and
	 * not do a 'task_exit'. */
	if (swinfo->fn)
		(*swinfo->fn) ();

cleanup:
	/* If the switch is part of a device, then let the device
	 * subsystem process the event */
	if (SW_HAS_DEVICE (swinfo))
		device_sw_handler (SW_GET_DEVICE (swinfo));

	/* Debounce period after the switch has been handled */
	if (swinfo->inactive_time == 0)
		task_sleep (TIME_100MS * 1); /* was 300ms!!! */
	else
		task_sleep (swinfo->inactive_time);

#if 0
	/* This code isn't needed at the moment */
	register bitset p = (bitset)switch_bits[AR_QUEUED];
	register uint8_t v = sw;
	__clearbit(p, v);
#endif

	task_exit ();
}


void switch_idle_task (void)
{
	uint8_t rawbits, pendbits;
	uint8_t col;
	extern U8 sys_init_complete;

	if (sys_init_complete == 0)
		return;

	for (col = 0; col <= 9; col++)
	{
		/* Disable interrupts while fiddling with the bits */
		disable_irq ();

		/* Grab the raw bits */
		rawbits = switch_bits[AR_RAW][col];

		/* Invert for optos */
		rawbits ^= mach_opto_mask[col];

		/* Convert to active level */
		rawbits |= mach_edge_switches[col];

		/* Grab the current set of pending bits */
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
			uint8_t sw = col << 3;

			/* Iterate over all rows -- all switches on this column */
			uint8_t row;
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
					task_t *tp = task_create_gid (GID_SW_HANDLER, switch_sched);
					task_set_arg (tp, sw);
				}
			}
		}
	}
}

