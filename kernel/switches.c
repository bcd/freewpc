/*
 * Copyright 2006, 2007, 2008 by Brian Dominy <brian@oddchange.com>
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


/* TODO : on real hardware, occasionally an entry seems to get
 * stuck in the switch queue, and that switch can no longer be
 * processed. */


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
	/* The switch number that is pending */
	U8 id;

	/* The amount of time left before the transition completes. */
	S8 timer;
} pending_switch_t;


/** 0/1 = the raw input value of the switch */
__fastram__ switch_bits_t sw_raw;

/** Nonzero for each switch whose last reading differs from the
most recent logical (debounced) reading */
__fastram__ switch_bits_t sw_edge;

__fastram__ switch_bits_t sw_stable;

__fastram__ switch_bits_t sw_unstable;

__fastram__ switch_bits_t sw_logical;

/** Nonzero for each switch that is in the switch queue. */
switch_bits_t sw_queued;

#define MAX_QUEUED_SWITCHES 16

pending_switch_t switch_queue[MAX_QUEUED_SWITCHES];

U8 switch_queue_head;

U8 switch_queue_tail;

U16 switch_last_service_time;


/** Return the switch table entry for a switch */
const switch_info_t *switch_lookup (const switchnum_t sw)
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



/** Detect row / column shorts */
void switch_short_detect (void)
{
	U8 n;
	U8 row = 0;


	n = sw_raw[0] & sw_raw[1] & sw_raw[2] & sw_raw[3] &
		sw_raw[4] & sw_raw[5] & sw_raw[6] & sw_raw[7];
	/* Each bit in 'n' represents a row that is shorted. */
	while (n != 0)
	{
		if (n & 1)
		{
			dbprintf ("Row %d short detected\n", row);
			/* TODO - ignore this row briefly */
		}
		n >>= 1;
		row++;
	}

	for (n = 0; n < 8; n++)
	{
		if (sw_raw[n] == ~mach_opto_mask[n])
		{
			/* The nth column is shorted. */
			dbprintf ("Column %d short detected\n", row);
			/* TODO - ignore this column briefly */
		}
	}
}


/* Before any switch data can be accessed on a WPC-S
 * or WPC95 machine, we need to poll the PIC and see
 * if the unlock code must be sent to it.   On pre-
 * security games, this function is a no-op. */
void pic_rtt (void)
{
#if (MACHINE_PIC == 1)
	U8 unlocked;

	/* Read the status to see if the matrix is still unlocked. */
	wpc_write_pic (WPC_PIC_COUNTER);
	null_function ();
	null_function ();
	null_function ();
	unlocked = wpc_read_pic ();
	if (!unlocked)
	{
		/* We need to unlock it again. */
		extern U8 pic_unlock_code[3];
		extern bool pic_unlock_ready;

		if (!pic_unlock_ready)
			return;

		wpc_write_pic (WPC_PIC_UNLOCK);
		null_function ();
		null_function ();
		wpc_write_pic (pic_unlock_code[0]);
		null_function ();
		null_function ();
		wpc_write_pic (pic_unlock_code[1]);
		null_function ();
		null_function ();
		wpc_write_pic (pic_unlock_code[2]);
		null_function ();
		null_function ();
	}
#endif /* MACHINE_PIC */
}


#if defined(CONFIG_PLATFORM_WPC) && defined(__m6809__)
extern inline void switch_rowpoll (const U8 col)
{
	/* Load the raw switch value from the hardware. */
	if (col == 0)
	{
		/* Column 0 = Dedicated Switches */
		__asm__ volatile ("ldb\t" C_STRING (WPC_SW_CABINET_INPUT));
	}
	else if (col <= 8)
	{
		/* Columns 1-8 = Switch Matrix
		Load method is different on PIC vs. non-PIC games. */
#if (MACHINE_PIC == 1)
		__asm__ volatile ("ldb\t" C_STRING (WPCS_PIC_READ));
#else
		__asm__ volatile ("ldb\t" C_STRING (WPC_SW_ROW_INPUT));
#endif
	}
	else if (col == 9)
	{
		/* Column 9 = Flipper Inputs
		Load method is different on WPC-95 vs. older Fliptronic games */
#if (MACHINE_WPC95 == 1)
		__asm__ volatile ("ldb\t" C_STRING (WPC95_FLIPPER_SWITCH_INPUT));
#else
		__asm__ volatile ("ldb\t" C_STRING (WPC_FLIPTRONIC_PORT_A));
#endif
	}

	/* Save the raw switch */
	__asm__ volatile ("stb\t%0" :: "m" (sw_raw[col]) );

	/* Set up the column strobe for the next read (on the next
	 * iteration).  Don't do this if the next read will be the dedicated
	 * switches.
	 *
	 * PIC vs. non-PIC games set up the column strobe differently. */
	if (col < 8)
	{
#if (MACHINE_PIC == 1)
		__asm__ volatile (
			"lda\t%0\n"
			"\tsta\t" C_STRING (WPCS_PIC_WRITE) :: "n" (WPC_PIC_COLUMN (col)) );
#else
		__asm__ volatile (
			"lda\t%0\n"
			"\tsta\t" C_STRING (WPC_SW_COL_STROBE) :: "n" (1 << col) );
#endif
	}

	/* Update stable/unstable states.  This is an optimized
	version of the C code below, which works around some GCC
	problems that are unlikely to ever be fixed.  I took the
	gcc output and optimized it by hand. */
	__asm__ volatile (
			"eorb	%0\n"
			"\ttfr	b,a\n"
			"\tandb	%1\n"
			"\tsta	%1\n"
			"\torb	%2\n"
			"\tstb	%2\n"
			"\tcoma\n"
			"\tanda	%2\n"
			"\tora	%3\n"
			"\tsta	%3" ::
				"m" (sw_logical[col]), "m" (sw_edge[col]),
				"m" (sw_stable[col]), "m" (sw_unstable[col]) );
}

#else /* !__m6809__ */

/** Poll a single switch column.
 * Column 0 corresponds to the cabinet switches.
 * Columns 1-8 refer to the playfield columns.
 * It is assumed that columns are polled in order, as
 * during the read of column N, the strobe is set so that
 * the next read will come from column N+1.
 */
extern inline void switch_rowpoll (const U8 col)
{
	U8 edge;

	/*
	 * Read the raw switch.
	 */
	if (col == 0)
		sw_raw[col] = pinio_read_dedicated_switches ();
	else if (col <= 8)
		sw_raw[col] = pinio_read_switch_rows ();
	else if (col == 9)
		sw_raw[col] = wpc_read_flippers ();

	/* Set up the column strobe for the next read (on the next
	 * iteration) */
	if (col < 8)
		pinio_write_switch_column (col);

	/* Update stable/unstable states. */
	edge = sw_raw[col] ^ sw_logical[col];
	sw_stable[col] |= edge & sw_edge[col];
	sw_unstable[col] |= ~edge & sw_stable[col];
	sw_edge[col] = edge;
}

#endif


/** Return TRUE if the given switch is CLOSED.
 * This scans the logical values calculated at periodic service time. */
bool switch_poll (const switchnum_t sw)
{
	return bitarray_test (sw_logical, sw);
}


/** Return TRUE if the given switch is an opto.  Optos
are defined in the opto mask array, which is auto generated from
the machine description. */
bool switch_is_opto (const switchnum_t sw)
{
	return bitarray_test (mach_opto_mask, sw);
}


/** Return TRUE if the given switch is ACTIVE.  This takes into
 * account whether or not the switch is an opto. */
bool switch_poll_logical (const switchnum_t sw)
{
	return switch_poll (sw) ^ switch_is_opto (sw);
}


void switch_rtt_0 (void)
{
	switch_rowpoll (0);
	switch_rowpoll (1);
	switch_rowpoll (2);
	switch_rowpoll (3);
	switch_rowpoll (4);
}


void switch_rtt_1 (void)
{
	switch_rowpoll (5);
	switch_rowpoll (6);
	switch_rowpoll (7);
	switch_rowpoll (8);

#if (MACHINE_FLIPTRONIC == 1)
	/* Poll the Fliptronic flipper switches */
	switch_rowpoll (9);
#endif
}


typedef struct
{
	const switch_info_t *swinfo;
	leff_data_t leffdata;
} lamp_pulse_data_t;


/** Task that performs a switch lamp pulse.
 * Some switches are inherently tied to a lamp.  When the switch
 * triggers, the lamp can be automatically flickered.  This is
 * implemented as a pseudo-lamp effect, so the true state of the
 * lamp is not disturbed. */
void switch_lamp_pulse (void)
{
	lamp_pulse_data_t * const cdata = task_current_class_data (lamp_pulse_data_t);
	/* Although not a true leff, this fools the lamp draw to doing
	 * the right thing. */
	cdata->leffdata.flags = L_SHARED;

	/* If the lamp is already allocated by another lamp effect,
	then don't bother trying to do the pulse. */
	if (lamp_leff2_test_and_allocate (cdata->swinfo->lamp))
	{
		/* Change the state of the lamp */
		if (lamp_test (cdata->swinfo->lamp))
			leff_off (cdata->swinfo->lamp);
		else
			leff_on (cdata->swinfo->lamp);
		task_sleep (TIME_200MS);

		/* Change it back */
		leff_toggle (cdata->swinfo->lamp);
		task_sleep (TIME_200MS);

		/* Free the lamp */
		lamp_leff2_free (cdata->swinfo->lamp);
	}
	task_exit ();
}


/*
 * The entry point for processing a switch transition.  It performs
 * some of the common switch handling logic before calling all
 * event handlers.  Then it also performs some common post-processing.
 * This function runs in a separate task context for each switch that
 * needs to be processed.
 */
void switch_sched_task (void)
{
	const U8 sw = (U8)task_get_arg ();
	const switch_info_t * const swinfo = switch_lookup (sw);

#ifdef DEBUGGER
	if (sw < 72)
	{
	dbprintf ("Handling switch ");
	sprintf_far_string (names_of_switches + sw);
	dbprintf1 ();
#ifdef DEBUG_SWITCH_NUMBER
	dbprintf (" (%d) ", sw);
#endif
	dbprintf ("\n");
	}
#endif

#if 0 /* not working */
	/* In test mode, always queue switch closures into the
	 * special switch test queue. */
	if (in_test)
		switch_test_add_queue (sw);
#endif

	log_event (SEV_INFO, MOD_SWITCH, EV_SW_SCHEDULE, sw);

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

		lamp_pulse_data_t *cdata = task_init_class_data (tp, lamp_pulse_data_t);
		cdata->swinfo = swinfo;
	}

	/* If we're in a live game and the switch declares a standard
	 * sound, then make it happen. */
	if ((swinfo->sound != 0) && in_live_game)
		sound_send (swinfo->sound);

	/* If the switch declares a processing function, call it.
	 * All functions are in the EVENT_PAGE. */
	if (swinfo->fn)
		callset_pointer_invoke (swinfo->fn);

	/* If a switch is marked SW_PLAYFIELD and we're in a game,
	 * then call the global playfield switch handler and mark
	 * the ball 'in play'.  Don't do the last bit if the switch
	 * specifically doesn't want this to happen.  Also, kick
	 * the ball search timer so that it doesn't expire.
	 */
	if ((swinfo->flags & SW_PLAYFIELD) && in_game)
	{
		callset_invoke (any_pf_switch);

		if (!valid_playfield)
		{
			if (swinfo->flags & SW_NOVALID)
			{
				try_validate_playfield (sw);
			}
			else
			{
				/* Normally, mark valid playfield right away
				 * for most switches */
				set_valid_playfield ();
			}
		}
		ball_search_timer_reset ();
	}

cleanup:
	/* If the switch is part of a device, then let the device
	 * subsystem process the event */
	if (SW_HAS_DEVICE (swinfo))
		device_sw_handler (SW_GET_DEVICE (swinfo));

	task_exit ();
}


/**
 * Schedule a task to handle a switch event.
 */
void switch_schedule (const U8 sw)
{
	/* Start a task to process the switch right away */
	task_pid_t tp = task_create_gid (GID_SW_HANDLER, switch_sched_task);
	task_set_arg (tp, sw);
}


/** Add a new entry to the switch queue. */
pending_switch_t *switch_queue_add (const switchnum_t sw)
{
	pending_switch_t *entry = &switch_queue[switch_queue_tail];
	dbprintf ("adding sw%d to queue\n", sw);
	entry->id = sw;
	entry->timer = switch_lookup(sw)->prebounce * 16;
	value_rotate_up (switch_queue_tail, 0, MAX_QUEUED_SWITCHES-1);
	bit_on (sw_queued, sw);
	return entry;
}

/** Find an entry in the switch queue by switch number. */
pending_switch_t *switch_queue_find (const switchnum_t sw)
{
	U8 i = switch_queue_head;
	while (i != switch_queue_tail)
	{
		if (switch_queue[i].id == sw)
			return &switch_queue[i];
		value_rotate_up (i, 0, MAX_QUEUED_SWITCHES-1);
	}
	return NULL;
}

/** Remove an entry from the switch queue */
void switch_queue_remove (pending_switch_t *entry)
{
	bit_off (sw_queued, entry->id);
	entry->id = 0xFF;
	if (entry == &switch_queue[switch_queue_head])
		value_rotate_up (switch_queue_head, 0, MAX_QUEUED_SWITCHES-1);
}

/** Initialize the switch queue */
void switch_queue_init (void)
{
	switch_queue_head = switch_queue_tail = 0;
	memset (sw_stable, 0, sizeof (sw_stable));
	memset (sw_unstable, 0, sizeof (sw_unstable));
	memset (sw_queued, 0, sizeof (sw_queued));
}


void switch_service_queue (void)
{
	U8 i;
	U8 elapsed_time;

	/* See how long since the last time we serviced the queue */
	elapsed_time = get_elapsed_time (switch_last_service_time);
	if (elapsed_time < 5)
		return;
	if (elapsed_time > 100)
	{
		dbprintf ("Switch queue did not reschedule for a while.\n");
		nonfatal (ERR_SWITCH_SLOW_SERVICE);
		elapsed_time = 100;
	}

	i = switch_queue_head;
	while (unlikely (i != switch_queue_tail))
	{
		pending_switch_t *entry;

		entry = &switch_queue[i];
		if (likely (entry->id != 0xFF))
		{
			dbprintf ("Servicing queued SW%d: ", entry->id);
			entry->timer -= elapsed_time;
			if (entry->timer <= 0)
			{
				/* Debounce is complete.  Schedule the switch handler. */
				dbprintf ("debounce complete\n");
				switch_queue_remove (entry);
				switch_schedule (entry->id);
				/* TODO - reset stable/unstable here */
			}
			else
			{
				dbprintf ("%d down, %d to go\n", elapsed_time, entry->timer);
			}
		}
		value_rotate_up (i, 0, MAX_QUEUED_SWITCHES-1);
	}

	switch_last_service_time = get_sys_time ();
}


static inline void switch_matrix_dump (const char *name, U8 *matrix)
{
	U8 i;
	dbprintf ("%s:", name);
	for (i=0; i < SWITCH_BITS_SIZE; i++)
		dbprintf ("%02X ", matrix[i]);
	dbprintf ("\n");
}


void switch_queue_dump (void)
{
	U8 i;

	dbprintf ("Queue: [%d, %d]\n", switch_queue_head, switch_queue_tail);
	for (i=0; i < MAX_QUEUED_SWITCHES; i++)
	{
		pending_switch_t *entry = switch_queue + i;
		if (entry->id != 0)
			dbprintf ("Entry %d:  SW %d  %d\n", i, entry->id, entry->timer);
	}

	switch_matrix_dump ("Raw     ", sw_raw);
	switch_matrix_dump ("Logical ", sw_logical);
	switch_matrix_dump ("Edge    ", sw_edge);
	task_sleep (TIME_16MS);
	switch_matrix_dump ("Stable  ", sw_stable);
	switch_matrix_dump ("Unstable", sw_unstable);
	switch_matrix_dump ("Queued  ", sw_queued);
}


/**
 * Handle a switch that has just become stable; i.e. it changed
 * and held steady for at least 2 consecutive readings.  It is
 * also known not to be in the heavy debounce queue already.
 */
void switch_update_stable (const U8 sw)
{
	dbprintf ("Switch stable: %d\n", sw);

	/* Queue the switch if it requires further debouncing.
	 * Otherwise, it is eligible for scheduling. */
	if (switch_lookup (sw)->prebounce != 0)
	{
		switch_queue_add (sw);
		return;
	}

	/* Latch the transition.  sw_logical is still an open/closed level.
	 * By clearing the stable/unstable bits, IRQ will begin scanning
	 * for new transitions at this point. */
	disable_irq ();
	bit_toggle (sw_logical, sw);
	bit_off (sw_stable, sw);
	bit_off (sw_unstable, sw);
	enable_irq ();

	/* See if the transition requires scheduling.  It does if:
	 a) the switch is bidirectional, or
	 b) the switch is an opto and it is now open, or
	 c) the switch is not an opto and it is now closed.
	*/
	if (bit_test (mach_edge_switches, sw)
		|| (!bit_test (sw_logical, sw) && bit_test (mach_opto_mask, sw))
		|| (bit_test (sw_logical, sw) && !bit_test (mach_opto_mask, sw)))
	{
		switch_schedule (sw);
	}
}


/**
 * Handle a switch that is changing faster than its debounce period.
 * Ignore the recent transitions and restart IRQ-level switch
 * scanning again.
 */
void switch_update_unstable (const U8 sw)
{
	dbprintf ("Switch unstable: %d\n", sw);

	if (bit_test (sw_queued, sw))
	{
		/* The switch was already seen and queued, but it did not
		 * complete its full debounce.  TODO - remove the queue
		 * entry here */
	}

	/* Restart IRQ-level scanning. */
	disable_irq ();
	bit_off (sw_stable, sw);
	bit_off (sw_unstable, sw);
	enable_irq ();
}



/** Idle time switch processing.  This function is called whenever there
 * are no round robin tasks to run.
 *
 * 'Pending switches' are scanned and handlers are spawned for each
 * of them (each is a separate task).
 */
CALLSET_ENTRY (switch, idle)
{
	register U16 col = 0;
	extern U8 sys_init_complete;
	U8 rows;

	/* Prior to system initialization, switches are not serviced.
	Any switch closures during this time continue to be queued up.
	However, at the least, allow the coin door switches to be polled. */
	if (unlikely (sys_init_complete == 0))
	{
		sw_logical[0] = sw_raw[0];
		return;
	}

	/* TODO - before doing anything else, make sure the ALWAYS CLOSED
	 * switch is really closed.  If not, there's a serious problem
	 * and we can't do any switch processing. */

	/* Check for shorted switch rows/columns.  TODO : this should return a
	 * value, and we skip rest of processing if there are problems. */
	switch_short_detect ();

	/* Iterate over each switch column to see what needs to be done. */
	for (col=0; col < SWITCH_BITS_SIZE; col++)
	{
		/* Each bit set in sw_unstable indicates a switch that had transitioned
		briefly, but not before the full debounce period expired.  These
		switches might be in the switch queue, or might not.  We need
		to check anyway. */
		if (unlikely (rows = sw_unstable[col]))
		{
			U8 sw = col * 8;
			do {
				if (rows & 1)
					switch_update_unstable (sw);
				rows >>= 1;
				sw++;
			} while (rows);
		}

		/* Each bit in sw_stable, but not in sw_unstable, indicates a switch
		that just transitioned and needs to be put into the debounce queue. */
		/* TODO - also verify that not already queued? */
		if (unlikely (rows = (sw_stable[col] & ~sw_unstable[col])))
		{
			U8 sw = col * 8;
			do {
				if (rows & 1)
					switch_update_stable (sw);
				rows >>= 1;
				sw++;
			} while (rows);
		}

		/* Because it can take awhile to scan the entire matrix, keep
		the software watchdog from expiring. */
		task_dispatching_ok = TRUE;
	}

	/* Service the switch queue. */
	switch_service_queue ();
}

#if 0 /* merge above */
		if (prebounce_bits & 1)
		{
			/* The switch is already in prebounce state, and a change
			 * occurred.  The switch is not stable, so the
			 * queue entry needs to be reset: restart the
			 * prebounce timer.  The entry is not removed/added
			 * to preserve the order??? */
			pending_switch_t *entry = switch_queue_find (sw);
			dbprintf ("Restarting prebounce for SW%d\n", sw);
			entry->timer = switch_lookup(sw)->prebounce * 16;
		}
#endif


/** Do final initializing once system init is complete. */
CALLSET_ENTRY (switch, init_complete)
{
	/* Initialize the service timer.  This needs to happen
	 * just before interrupts are enabled */
	switch_last_service_time = get_sys_time ();
}


/** Initialize the switch subsystem */
void switch_init (void)
{
	/* Initialize the switch state buffers */
	memcpy (sw_logical, mach_opto_mask, SWITCH_BITS_SIZE);
	memset (sw_edge, 0, sizeof (switch_bits_t));

	/* Initialize the switch queue */
	switch_queue_init ();
}

