/**
 * kernel/switches.c
 *
 * (C) Copyright 2005 by Brian Dominy.
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
uint8_t switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];


/*
 * Entry for unused switches.
 */

void sw_unused_handler (void)
{
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
#ifndef MACHINE_SW01
#define MACHINE_SW01 sw_unused
#endif
#ifndef MACHINE_SW02
#define MACHINE_SW02 sw_unused
#endif
#ifndef MACHINE_SW03
#define MACHINE_SW03 sw_unused
#endif
#ifndef MACHINE_SW04
#define MACHINE_SW04 sw_unused
#endif
#ifndef MACHINE_SW05
#define MACHINE_SW05 sw_unused
#endif
#ifndef MACHINE_SW06
#define MACHINE_SW06 sw_unused
#endif
#ifndef MACHINE_SW07
#define MACHINE_SW07 sw_unused
#endif
#ifndef MACHINE_SW08
#define MACHINE_SW08 sw_unused
#endif

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
	MACHINE_SW01, MACHINE_SW02, MACHINE_SW03, MACHINE_SW04,
	MACHINE_SW05, MACHINE_SW06, MACHINE_SW07, MACHINE_SW08,

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

	sw_unused, sw_right_flipper, sw_unused, sw_left_flipper,
	sw_unused, sw_upper_right_flipper, sw_unused, sw_upper_left_flipper;


static const switch_info_t *switch_table[NUM_SWITCHES] = {
	&MACHINE_SW01, &MACHINE_SW02, &MACHINE_SW03, &MACHINE_SW04,
	&MACHINE_SW05, &MACHINE_SW06, &MACHINE_SW07, &MACHINE_SW08,

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



extern inline const switch_info_t *switch_lookup (uint8_t sw)
{
	return switch_table[sw];
}


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
		asm __volatile__ ("\tlda " STR(WPC_FLIPTRONIC_PORT_A));

	if (col < 8)
	{
		asm __volatile__ ("\tldb %0" :: "g" (1 << col));
		asm __volatile__ ("\tstb " STR(WPC_SW_COL_STROBE));
	}

	asm __volatile__ ("\tldb %0" 	:: "m" (switch_bits[AR_RAW][col]));
	asm __volatile__ ("\tsta %0"	:: "m" (switch_bits[AR_RAW][col]));
	asm __volatile__ ("\teorb %0"  :: "m" (switch_bits[AR_RAW][col]));
	asm __volatile__ ("\tstb %0"	:: "m" (switch_bits[AR_CHANGED][col]));
	asm __volatile__ ("\torb %0"	:: "m" (switch_bits[AR_PENDING][col]));
	asm __volatile__ ("\tstb %0"	:: "m" (switch_bits[AR_PENDING][col]));
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
	const switch_info_t * const swinfo = task_get_arg ();
	dbprintf ("Pulsing lamp %d\n", swinfo->lamp);

	lamp_leff_allocate (swinfo->lamp);
	leff_toggle (swinfo->lamp);
	task_sleep (TIME_100MS * 2);
	leff_toggle (swinfo->lamp);
	lamp_leff_free (swinfo->lamp);
	task_exit ();
}


#pragma long_branch
void switch_sched (void)
{
	const uint8_t sw = task_get_arg ();
	const switch_info_t * const swinfo = switch_lookup (sw);

	if ((swinfo->flags & SW_IN_GAME) && !in_game)
		return;

	if ((swinfo->flags & SW_PLAYFIELD) && in_game)
	{
		call_hook(any_pf_switch);
		if (!(swinfo->flags & SW_NOPLAY) && !ball_in_play)
			mark_ball_in_play ();
	}

	if ((swinfo->lamp != 0) && in_live_game)
	{
		task_t *tp = task_create_gid (GID_SWITCH_LAMP_PULSE, switch_lamp_pulse);
		task_set_arg (tp, (U16)swinfo);
	}

	if ((swinfo->sound != 0) && in_live_game)
		sound_send (swinfo->sound);

	if (swinfo->fn)
		(*swinfo->fn) ();

	if (SW_HAS_DEVICE (swinfo))
		device_sw_handler (SW_GET_DEVICE (swinfo));

	/* Debounce period after the switch has been handled */
	if (swinfo->inactive_time == 0)
		task_sleep (TIME_100MS * 1); /* was 300ms!!! */
	else
		task_sleep (swinfo->inactive_time);

	register bitset p = (bitset)switch_bits[AR_QUEUED];
	register uint8_t v = sw;
	__clearbit(p, v);
	task_exit ();
}
#pragma short_branch


void switch_idle_task (void)
{
	uint8_t rawbits, pendbits;
	uint8_t col;

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
					task_t *tp = task_create_gid (GID_SW_HANDLER, switch_sched);
					task_set_arg (tp, sw);
				}
			}
		}
	}
}

