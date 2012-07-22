/*
 * Copyright 2006-2011 by Brian Dominy <brian@oddchange.com>
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
#include <diag.h>
#include <search.h>

/*
 * A pending switch is one which has changed levels for 2
 * consecutive readings at interrupt time, but needs to
 * be debounced further.
 *
 * This struct tracks the switch number, its state, and the
 * debounce timer.
 *
 * There are a finite number of these objects.  When a change
 * is detected, we will either allocate a new slot or
 * re-use an existing slot for the same switch that did not
 * complete its previous debounce cycle.
 */
typedef struct
{
	/* The switch number that is pending */
	U8 id;

	/* The amount of time left before the transition completes. */
	S8 timer;
} pending_switch_t;


/** The raw input values of the switch.  These values are
 * updated every 2ms, and are only used as inputs into the
 * debounce procedure.  Higher layer software never looks at
 * these values. */
__fastram__ switch_bits_t sw_raw;

/** Nonzero for each switch whose last raw reading differs from the
most recent logical (debounced) reading */
__fastram__ switch_bits_t sw_edge;

/** Nonzero for each switch that has been stable at the IRQ level.
 * This means two consecutive readings, 2ms apart, returned the
 * same value, which differed from the last stable reading. */
__fastram__ switch_bits_t sw_stable;

/** Nonzero for each stable switch (according to sw_stable)
 * that went back to its previous reading.  This means
 * the switch did not remain stable long enough for non-interrupt
 * switch scanning to recognize it. */
__fastram__ switch_bits_t sw_unstable;

/** The current logical (i.e. debounced) readings for each
 * switch.  These are still based on voltage levels and do not
 * consider inverting necessary for processing optos.  These
 * are the levels which should be read outside of interrupts to
 * see what the current state of a switch is. */
__fastram__ switch_bits_t sw_logical;

/** Nonzero for each switch that is in the switch queue.
 * This is not strictly needed, but it provides a fast way to
 * see if a switch is already in the queue without having to
 * scan the entire array. */
switch_bits_t sw_queued;

/* An array of pending switches which have not fully debounced yet.
 * The array is kept compact, meaning that if there are N entries
 * used, they will always be in slots [0] to [N-1]. */
pending_switch_t switch_queue[MAX_QUEUED_SWITCHES];

/* A pointer to the first free entry in the switch queue.
 * When this is equal to 'switch_queue', it means the entire
 * queue is empty. */
__fastram__ pending_switch_t *switch_queue_top;

/** The last time that switch scanning occurred.  It is used
 * to determine by how much to decrement debounce timers. */
U16 switch_last_service_time;

/** The switch number of the last switch to be scheduled.
 * Provided as a convenience for test mode. */
U8 sw_last_scheduled;

/** Nonzero if a switch short was detected and switches need to be
 * ignored for some time.  The value indicates the number of
 * seconds to ignore switches. */
U8 sw_short_timer;


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

	n = sw_raw[0] & sw_raw[1] & sw_raw[2] & sw_raw[3] &
		sw_raw[4] & sw_raw[5] & sw_raw[6] & sw_raw[7];
	if (n != 0)
	{
		dbprintf ("Row short\n", n);
		sw_short_timer = 3;
	}

	for (n = 0; n < 8; n++)
	{
		if (sw_raw[n] == ~mach_opto_mask[n])
		{
			/* The nth column is shorted. */
			dbprintf ("Column short\n");
			sw_short_timer = 3;
		}
	}
}


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

	/* Ignore any switch that doesn't have a processing function.
	   This shouldn't ever happen if things are working correctly, but it
		was observed on PIC games when the PIC code is broken and reporting
		bad switch returns.  genmachine ensures that all defined switches
		have a non-null value (null_function if nothing needs to be done) */
	if (swinfo->fn == 0)
		goto cleanup;

	/* For test mode : this lets it see what was the last switch
	 * to be scheduled.  Used by the Switch Edges test. */
	sw_last_scheduled = sw;

	log_event (SEV_INFO, MOD_SWITCH, EV_SW_SCHEDULE, sw);

	/* Don't service switches marked SW_IN_GAME if we're
	 * not presently in a game */
	if ((swinfo->flags & SW_IN_GAME) && !in_game)
		goto cleanup;

	/* Don't service switches not marked SW_IN_TEST, unless we're
	 * actually in test mode */
	if (!(swinfo->flags & SW_IN_TEST) && in_test)
		goto cleanup;

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

#ifdef DEBUGGER
	if (swinfo->fn != null_function && sw < 72)
	{
		dbprintf ("SW: ");
		sprintf_far_string (names_of_switches + sw);
		dbprintf1 ();
#ifdef DEBUG_SWITCH_NUMBER
		dbprintf (" (%d) ", sw);
#endif
		dbprintf ("\n");
	}
#endif

	/* If the switch declares a processing function, call it. */
	if (swinfo->fn)
		callset_pointer_invoke (swinfo->fn);

	/* Declare this as a hardware event that affects the random number
	generator. */
	random_hw_event ();

	/* If a switch is marked SW_PLAYFIELD and we're in a game,
	 * then call the global playfield switch handler and check for
	 * valid playfield.  Also, reset the ball search timer so that
	 * it doesn't expire.
	 */
	if ((swinfo->flags & SW_PLAYFIELD) && in_game)
	{
		callset_invoke (any_pf_switch);

		/* If valid playfield was not asserted yet, then see if this
		 * switch validates it.  Most playfield switches do this right
		 * away, but for some switches, like special solenoids (jets
		 * or slings), which could repeatedly trigger if misaligned,
		 * count the activations and validate only when some number
		 * of different switches have triggered.  Device counting
		 * switches are ignored here, but an 'enter' event will
		 * set it. */
		if (!valid_playfield)
		{
			if (swinfo->flags & SW_NOVALID)
			{
				if (!SW_HAS_DEVICE (swinfo))
					try_validate_playfield (sw);
			}
			else
				set_valid_playfield ();
		}
		ball_search_timer_reset ();
	}

cleanup:
	/* If the switch is part of a device, then let the device
	 * subsystem process the event.  Note this will always occur
	 * regardless of any of the above conditions checked. */
	if (SW_HAS_DEVICE (swinfo))
		device_sw_handler (SW_GET_DEVICE (swinfo));

	task_exit ();
}


/**
 * Process a switch that has transitioned states.  All debouncing
 * is fully completed prior to this call.
 *
 * The transition is first latched, so that polling the switch level
 * will return the new state.  At the same time, IRQ-level scanning
 * is restarted, so that further transitions can be detected.
 *
 * Second, if the transition requires scheduling, a task is started
 * to handle it.  Eligibility depends on whether or not the switch
 * is declared as an edge switch (meaning it is scheduled on both
 * types of transitions) and whether or not it is an opto (i.e.
 * do we schedule open-to-closed or closed-to-open?)
 *
 * The switch is also known not to be in the debounce queue prior to this
 * function being called.
 */
void switch_transitioned (const U8 sw)
{
	/* Latch the transition.  sw_logical is still an open/closed level.
	 * By clearing the stable/unstable bits, IRQ will begin scanning
	 * for new transitions at this point. */
	rtt_disable ();
	bit_toggle (sw_logical, sw);
	bit_off (sw_stable, sw);
	bit_off (sw_unstable, sw);
	bit_off (sw_edge, sw);
	rtt_enable ();

	/* See if the transition requires scheduling.  It does if the
	   switch is declared 'edge' (it schedules when becoming active
		or inactive), otherwise only becoming active.  Because most
		switches are not edge, check for the active state first. */
	if (switch_poll_logical (sw) || bit_test (mach_edge_switches, sw))
	{
#ifdef CONFIG_BPT
		/* One extra condition : do not schedule any switches when the
		system is paused */
		if (db_paused != 0)
			return;
#endif

		/* Start a task to process the switch event.
		This task may sleep if necessary, but it should be as fast as possible
		and push long-lived operations into separate background tasks.
		It is possible for more than one instance of a task to exist for the same
		switch, if valid debounced transitions occur quickly. */
		task_pid_t tp = task_create_gid (GID_SW_HANDLER, switch_sched_task);
		task_set_arg (tp, sw);
	}
}


/** Add a new entry to the switch queue. */
void switch_queue_add (const switchnum_t sw)
{
	if (switch_queue_top < switch_queue + MAX_QUEUED_SWITCHES)
	{
		dbprintf ("adding %d to queue\n", sw);
		switch_queue_top->id = sw;
		switch_queue_top->timer = switch_lookup(sw)->debounce;
		bit_on (sw_queued, sw);
		switch_queue_top++;
	}
}


/** Remove an entry from the switch queue */
void switch_queue_remove (pending_switch_t *entry)
{
	/* Copy the last entry in the queue into the slot being deleted. */
	dbprintf ("removing %d from queue\n", entry->id);
	bit_off (sw_queued, entry->id);
	entry->id = (switch_queue_top-1)->id;
	entry->timer = (switch_queue_top-1)->timer;
	switch_queue_top--;
}

/** Initialize the switch queue */
void switch_queue_init (void)
{
	switch_queue_top = switch_queue;
	memset (sw_stable, 0, sizeof (sw_stable));
	memset (sw_unstable, 0, sizeof (sw_unstable));
	memset (sw_queued, 0, sizeof (sw_queued));
}


/** Service the switch queue.  This function is called
 * periodically to see if any pending switch transitions have
 * completed their debounce time.  If any switch becomes
 * unstable before the period expires, it will be removed
 * from the queue prior to this function being called.  So
 * all that is needed here is to decrement the timers, and if
 * one reaches zero, consider that switch transitioned.
 */
void switch_service_queue (void)
{
	pending_switch_t *entry;
	U8 elapsed_time;

	/* See how long since the last time we serviced the queue.
	This is in 16ms ticks. */
	elapsed_time = get_elapsed_time (switch_last_service_time);

	entry = switch_queue;
	while (unlikely (entry < switch_queue_top))
	{
		entry->timer -= elapsed_time;
		if (entry->timer <= 0)
		{
			/* Debounce interval is complete. */

			/* See if the switch held its state during the debounce period */
			if (bit_test (sw_unstable, entry->id))
			{
				/* Debouncing failed, so don't process the switch.
				 * Restart IRQ-level scanning. */
				rtt_disable ();
				bit_off (sw_stable, entry->id);
				bit_off (sw_unstable, entry->id);
				rtt_enable ();
			}
			else
			{
				/* Debouncing succeeded, so process the switch */
				switch_transitioned (entry->id);
			}

			/* The queue entry can be removed now.
			   Note we do not advance the 'entry' pointer in
				this case, as we may have copied a legitimate
				entry here. */
			switch_queue_remove (entry);
		}
		else
		{
			entry++;
		}
	}

	switch_last_service_time = get_sys_time ();
}


#ifdef DEBUGGER
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
	pending_switch_t *entry = switch_queue;

	dbprintf ("Switch queue base: %p\n", switch_queue);
	dbprintf ("Switch queue top: %p\n", switch_queue_top);
	while (entry != switch_queue_top)
	{
		dbprintf ("Pending: SW%d  %d\n", entry->id, entry->timer);
		entry++;
	}
	switch_matrix_dump ("Raw     ", sw_raw);
	switch_matrix_dump ("Logical ", sw_logical);
	switch_matrix_dump ("Edge    ", sw_edge);
	task_runs_long ();
	switch_matrix_dump ("Stable  ", sw_stable);
	switch_matrix_dump ("Unstable", sw_unstable);
	switch_matrix_dump ("Queued  ", sw_queued);
}
#endif /* DEBUGGER */


/**
 * Handle a switch that has just become stable; i.e. it changed
 * and held steady for at least 2 consecutive readings.  It is
 * also known not to be in the heavy debounce queue already.
 *
 * If the switch requires a longer debounce period, then it is
 * put into a queue and we wait a little while to see if it
 * remains stable, then it will be scheduled.
 */
static void switch_update_stable (const U8 sw)
{
	/* Queue the switch if it requires further debouncing.
	 * Otherwise, it is eligible for scheduling. */
	if (switch_lookup (sw)->debounce != 0)
		switch_queue_add (sw);
	else
		switch_transitioned (sw);
}


/** Periodic switch processing.  This function is called frequently
 * to scan pending switches and spawn new tasks to handle them.
 */
void switch_periodic (void)
{
	register U16 col = 0;
	extern U8 sys_init_complete;
	U8 rows;

	/* If there are row/column shorts, ignore the switch matrix. */
	if (unlikely (sw_short_timer))
		return;

	/* Prior to system initialization, switches are not serviced.
	Any switch closures during this time continue to be queued up.
	However, at the least, allow the coin door switches to be polled. */
	if (unlikely (sys_init_complete == 0))
	{
		sw_logical[0] = sw_raw[0];
		return;
	}

	/* Check for shorted switch rows/columns. */
	switch_short_detect ();

	/* Service the switch queue.  Note that this is done BEFORE
	 * polling switches; please do not change this!  In case
	 * idle is not invoked fast enough, it is possible that a
	 * switch might have legitimately completed its debounce period,
	 * but we just didn't see it in time before it transitioned
	 * back.  In that case, to be fair, consider the transition
	 * anyway.  The service function does not actually poll the
	 * current switch readings at all, so it is safe to do this
	 * even if there are hardware errors. */
	switch_service_queue ();

	/* Iterate over each switch column to see what needs to be done. */
	task_dispatching_ok = TRUE;
	for (col=0; col < SWITCH_BITS_SIZE; col++)
	{
		/* Each bit in sw_stable indicates a switch
		that just transitioned and may need to be processed */
		if (unlikely (rows = sw_stable[col]))
		{
			U8 sw = col * 8;
			do {
				if ((rows & 1) && !bit_test (sw_queued, sw))
					switch_update_stable (sw);
				rows >>= 1;
				sw++;
			} while (rows);
		}
		task_runs_long ();
	}
}

/** As part of startup diagnostics, check that the 12V
 * switch matrix power is present. */
CALLSET_ENTRY (switch, diagnostic_check)
{
#if (MACHINE_PIC == 1)
	if (!switch_scanning_ok ())
	{
		diag_post_error ("SECURITY PIC\nNOT INITIALIZED\n", SYS_PAGE);
		return;
	}
#endif

#ifdef SW_ALWAYS_CLOSED
	/* Make sure the ALWAYS CLOSED switch is really closed.
	 * If not, there's a serious problem
	 * and we can't do any switch processing. */
	if (unlikely (!rt_switch_poll (SW_ALWAYS_CLOSED)))
		diag_post_error ("12V SWITCH POWER\nIS NOT PRESENT\n", SYS_PAGE);
#endif

#ifdef MACHINE_SLAM_TILT_SWITCH
	if (unlikely (rt_switch_poll (MACHINE_SLAM_TILT_SWITCH)))
		diag_post_error ("SLAM TILT IS\nSTUCK CLOSED\n", SYS_PAGE);
#endif
}


/** Do final initializing once system init is complete. */
CALLSET_ENTRY (switch, init_complete)
{
	/* Initialize the service timer.  This needs to happen
	 * just before interrupts are enabled */
	switch_last_service_time = get_sys_time ();
}


/** Once per second, see if we had disabled switch scanning
 * due to a short, and it should be reenabled now.  This
 * is not accurately timed: it may range from 2.1 to 3s of
 * disabled time. */
CALLSET_ENTRY (switch, idle_every_second)
{
	if (sw_short_timer > 0)
	{
		sw_short_timer--;
	}
}


/** Initialize the switch subsystem */
void switch_init (void)
{
	/* Initialize the short timer so switch scanning is enabled */
	sw_short_timer = 0;

	/* Initialize the switch state buffers.  raw/logical are
	set to 'inactive' for all switches; for optos that means the
	bit is set, not clear.  edge is set to all zeroes, meaning that
	no change was seen since the last reading. */
	memcpy (sw_raw, mach_opto_mask, SWITCH_BITS_SIZE);
	memcpy (sw_logical, mach_opto_mask, SWITCH_BITS_SIZE);
	memset (sw_edge, 0, sizeof (switch_bits_t));

	/* Initialize the switch queue */
	switch_queue_init ();
}

