
#include <freewpc.h>
#include <sys/irq.h>

#define switch_raw_bits			switch_bits[AR_RAW]
#define switch_changed_bits	switch_bits[AR_CHANGED]
#define switch_pending_bits	switch_bits[AR_PENDING]
#define switch_queued_bits		switch_bits[AR_QUEUED]

uint8_t switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];


extern void sw_left_coin (void);
extern void sw_right_coin (void);
extern void sw_center_coin (void);
extern void sw_fourth_coin (void);
extern void sw_escape_button (void);
extern void sw_down_button (void);
extern void sw_up_button (void);
extern void sw_enter_button (void);

extern void sw_start_button (void);
extern void sw_trough (void);
extern void sw_outhole (void);

extern void sw_rocket (void);
extern void sw_slot (void);
extern void sw_lock (void);

extern void sw_tilt (void);
extern void sw_slam_tilt (void);

static const switch_info_t switch_info[NUM_SWITCHES] = {
	[SW_LEFT_COIN] = { .fn = sw_left_coin, },
	[SW_CENTER_COIN] = { .fn = sw_center_coin, },
	[SW_RIGHT_COIN] = { .fn = sw_right_coin, },
	[SW_FOURTH_COIN] = { .fn = sw_fourth_coin, },
	[SW_ESCAPE] = { .fn = sw_escape_button, },
	[SW_DOWN] = { .fn = sw_down_button, },
	[SW_UP] = { .fn = sw_up_button, },
	[SW_ENTER] = { .fn = sw_enter_button, },

	[SW_START_BUTTON] = { .fn = sw_start_button },
	[SW_TILT] = { .fn = sw_tilt },
	[SW_RIGHT_TROUGH] = { .fn = sw_trough },
	[SW_CENTER_TROUGH] = { .fn = sw_trough },
	[SW_LEFT_TROUGH] = { .fn = sw_trough },
	[SW_OUTHOLE] = { .fn = sw_outhole },

	[SW_SLAM_TILT] = { .fn = sw_slam_tilt },

	[SW_ROCKET] = { .fn = sw_rocket },

	[SW_SLOT] = { .fn = sw_slot },
	[SW_LOCK_CENTER] = { .fn = sw_lock },
	[SW_LOCK_UPPER] = { .fn = sw_lock },
	[SW_LOCK_LOWER] = { .fn = sw_lock },
};


extern inline switch_info_t *switch_lookup (uint8_t sw)
{
	return (switch_info_t *)((uint8_t *)switch_info + (sw << 2));
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
	else
		asm __volatile__ ("\tlda " STR(WPC_SW_ROW_INPUT));

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
}


void switch_sched (void)
{
	const uint8_t sw = task_get_arg ();
	const switch_info_t * const swinfo = switch_lookup (sw);

	if (swinfo->fn)
		(*swinfo->fn) ();

	/* Debounce period after the switch has been handled */
	task_sleep (TIME_100MS * 3);

	register bitset p = (bitset)switch_bits[AR_QUEUED];
	register uint8_t v = sw;
	__clearbit(p, v);
	task_exit ();
}


void switch_idle_task (void)
{
	uint8_t rawbits, pendbits;
	uint8_t col;

	for (col = 0; col < 9; col++)
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

