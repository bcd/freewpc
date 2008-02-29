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

#include <freewpc.h>

/**
 * \file
 * \brief A display effect -- or deff -- is a task responsible for drawing to
 * the display (DMD or alphanumeric).
 *
 * At any given time, there may be multiple deffs which have been
 * "started", or queued to run.  However, the display can only be granted
 * to one deff at a time, so only the one with the highest "priority"
 * actually executes.
 *
 * The single running deff always has the same task group ID,
 * GID_DEFF.  Whenever priorities change, the running deff can be
 * stopped just by killing GID_DEFF and then restarting whatever
 * deff needs to be running.
 *
 * There are two kinds of deffs: normal, and running.  Running deffs
 * are special in that they are long-lived and continue to run
 * until explicitly stopped by a call to deff_stop().  Normal deffs
 * are short-lived and stop on their own once the effect completes.
 * If a normal deff can't get the display immediately, it is
 * discarded.  If a running deff can't get the display, it is queued
 * and subject to get it later if priorities change.
 */

#ifdef DEFF_DEBUG
#define deff_debug(fmt, rest...) dbprintf(fmt, ## rest)
#else
#define deff_debug(fmt, rest...)
#endif


/** A deff entry.  These are created dynamically as display effects
need to be started. */
typedef struct
{
	struct dll_header dll;
	U8 id;
	U8 prio;
	U8 flags;
	U8 timeout;
} deff_entry_t;


/** Optional feature for use by deffs that need to manage different
parts of the display independently. */
void (*deff_component_table[4]) (void);


/* The list of all deffs that want to run, but can't */
deff_entry_t *deff_waitqueue;

/* The single deff that is currently driving the display */
deff_entry_t *deff_runqueue;

/** The number of elapsed ticks since the last reschedule */
U8 deff_time_last_idle;

U8 deff_timeout_disabled;


deff_entry_t *deff_entry_create (deffnum_t id)
{
	const deff_t *deff = &deff_table[id];
	deff_entry_t *entry;
	
	entry = malloc (sizeof (deff_entry_t));
	if (entry == NULL)
		return NULL;

	dll_init_element (entry);
	entry->prio = deff->prio;
	entry->id = id;
	entry->flags = deff->flags;
	return entry;
}


static inline void deff_initqueue (deff_entry_t **queue)
{
	dll_init (queue);
}


static inline void deff_enqueue (deff_entry_t **queue, deff_entry_t *entry)
{
	dll_add_front (queue, entry);
}


static inline void deff_dequeue (deff_entry_t **queue, deff_entry_t *entry)
{
	dll_remove (queue, entry);
}


deff_entry_t *deff_entry_find_waiting (deffnum_t id)
{
	deff_entry_t *entry = deff_waitqueue;
	if (entry)
	{
		do {
			deff_debug ("find_waiting: %p\n", entry);
			if (entry->id == id)
				return entry;
			else
				entry = (deff_entry_t *)entry->dll.next;
		} while (entry != deff_waitqueue);
	}
	return NULL;
}


static inline void deff_entry_free (deff_entry_t *entry)
{
	free (entry);
}


void dump_deffs (void)
{
	if (deff_runqueue)
		dbprintf ("Running: %d\n", deff_runqueue->id);
	else
		dbprintf ("No Running\n");

	if (deff_waitqueue)
	{
		dbprintf ("Waiting:\n");
	}
	else
		dbprintf ("No waiting\n");
}


/** The default running deff that runs when no other deff exists.
It simply keeps the display blank. */
void deff_default (void)
{
	dmd_alloc_low_clean ();
	dmd_show_low ();
	for (;;)
	{
		task_sleep_sec (10);
	}
}


/** Returns the ID of the currently active display effect. */
U8 deff_get_active (void)
{
	return deff_runqueue ? deff_runqueue->id : DEFF_NULL;
}


/** Returns non-NULL if the specific display effect is started.  It is
not necessarily 'active', i.e. it may be queued but a higher
priority deff actually has the display. */
deff_entry_t *deff_entry_find (deffnum_t id)
{
	deff_entry_t *entry;

	/* Search the runqueue */
	if (id == deff_get_active ())
		return deff_runqueue;

	/* Search the waitqueue */
	if ((entry = deff_entry_find_waiting (id)) != NULL)
		return entry;

	/* Not found on either queue */
	return NULL;
}


/** For compatibility with the old deff system */
bool deff_is_running (deffnum_t id)
{
	return deff_entry_find (id) != NULL;
}


/**
 * Common processing that must occur when stopping a running deff.
 * This function is called both when a deff exits and when
 * other code stops the deff.
 * In both cases kickout locks should be cleared.
 * The DMD transition should only be cancelled when a deff
 * is killed externally.  If a deff exits cleanly, it may
 * schedule a transition to be used when the new deff renders
 * its first frame.
 */
static void deff_stop_task (void)
{
	/* if (!task_find_gid (GID_DEFF_EXITING)) -- not working yet */
		dmd_reset_transition ();
	kickout_unlock (KLOCK_DEFF);
}


/** Starts the thread for the currently running display effect. */
static void deff_start_task (const deff_t *deff)
{
	task_pid_t tp;

	/* Stop whatever deff is running now */
	task_kill_gid (GID_DEFF);
	deff_stop_task ();

	/* Create a task for the new deff */
	tp = task_create_gid (GID_DEFF, deff->fn);
	if (deff->page != 0xFF)
		task_set_rom_page (tp, deff->page);
}


/** Start a task for the running deff that ought to be running */
void deff_reschedule (void)
{
	deff_entry_t *entry;
	deff_entry_t *best = NULL;

	/* Clean up before starting a new task */
	deff_initqueue (&deff_runqueue);

	/* Select a deff from the waitqueue */
	if (deff_waitqueue)
	{
		deff_debug ("Scanning waitqueue\n");
		entry = deff_waitqueue;
		do {
			deff_debug ("Checking %p, id %d, prio %d\n",
				entry, entry->id, entry->prio);

			if (!best || entry->prio > best->prio)
				best = entry;

			entry = (deff_entry_t *)entry->dll.next;
		} while (entry != deff_waitqueue);
	}

	/* If there's something to run, move it to the runqueue and
	start it */
	if (best != NULL)
	{
		deff_debug ("Best is %d\n", best->id);
		deff_dequeue (&deff_waitqueue, best);
		deff_enqueue (&deff_runqueue, best);
		deff_start_task (&deff_table[best->id]);
	}
	else
	{
		/* Start the default deff.  Note that it does not
		have an entry for it */
		deff_debug ("Nothing to run\n");
		deff_stop_task ();
		task_recreate_gid (GID_DEFF, deff_default);
	}
}


/** Try to start a display effect described by entry.
 * If it can't be started, it is either queued or freed.
 * If it preempts another deff, that one will be queued or freed
 * as well. */
static void deff_entry_start (deff_entry_t *entry)
{
	if (!deff_runqueue || entry->prio > deff_runqueue->prio)
	{
		/* This is the new active running deff */
		/* If something else is running, it must be stopped */
		if (deff_runqueue)
		{
			deff_entry_t *oldentry = deff_runqueue;
			deff_dequeue (&deff_runqueue, oldentry);

			/* Move the running deff onto the wait list if
			it wants to be queued.  Otherwise it's a goner. */
			if (oldentry->flags & D_QUEUED)
			{
				deff_debug ("Moving deff %d to waitqueue\n", oldentry->id);
				deff_enqueue (&deff_waitqueue, oldentry);
			}
			else
			{
				deff_debug ("Removing low pri deff %d\n", oldentry->id);
				deff_entry_free (oldentry);
			}
		}
		deff_debug ("Adding deff %d to runqueue now\n", dn);
		deff_enqueue (&deff_runqueue, entry);
		deff_start_task (&deff_table[entry->id]);
	}
	else if (entry->flags & D_QUEUED)
	{
		/* This deff cannot run now, but it wants to wait. */
		deff_debug ("Can't run because higher priority active\n");
		deff_enqueue (&deff_waitqueue, entry);

		/* If it wants to timeout automatically, initialize the timeout value. */
		if (entry->flags & D_TIMEOUT)
			entry->timeout = 5; /* wait 5 secs. */
	}
	else
	{
		deff_debug ("Quick deff lacks priority\n");
		deff_entry_free (entry);
	}
}


/** Start a statically defined display effect */
void deff_start (deffnum_t dn)
{
	deff_entry_t *entry;

	dbprintf ("Deff %d start\n", dn);

	/* See if an entry is already tracking this deff.
	 * If so, just return.  To truly restart the deff,
	 * you need to call deff_restart(). */
	entry = deff_entry_find (dn);
	if (entry == NULL)
	{
		entry = deff_entry_create (dn);
		if (entry == NULL)
		{
			dbprintf ("Failed to alloc deff entry %d\n", dn);
			return;
		}
	}
	else
	{
		return;
	}

	deff_entry_start (entry);
}


/** Stop a running or waiting deff */
void deff_stop (deffnum_t dn)
{
	deff_entry_t *entry;

	dbprintf ("Stopping deff #%d\n", dn);
	if (deff_runqueue && deff_runqueue->id == dn)
	{
		deff_debug ("Dequeueing running deff %d\n", dn);
		deff_entry_free (deff_runqueue);
 		deff_reschedule ();
	}
	else if ((entry = deff_entry_find_waiting (dn)) != NULL)
	{
		deff_debug ("Dequeueing waiting deff %d\n", dn);
		deff_dequeue (&deff_waitqueue, entry);
		deff_entry_free (entry);
	}
	else
	{
		deff_debug ("Deff not started %d\n", dn);
	}
}


/** Restart a deff.  If the deff is already running, its thread is stopped
and then restarted.  If it is queued but not active, nothing happens.
If it isn't even in the queue, then it is treated just like deff_start(). */
void deff_restart (deffnum_t dn)
{
	if (deff_runqueue && deff_runqueue->id == dn)
	{
		deff_start_task (&deff_table[dn]);
	}
	else
	{
		deff_entry_t *entry = deff_entry_find_waiting (dn);
		if (entry == NULL)
			deff_start (dn);
	}
}


/** Called directly from a deff when it wants to exit */
__noreturn__ void deff_exit (void)
{
	dbprintf ("Exiting deff %d\n", deff_runqueue->id);
	task_setgid (GID_DEFF_EXITING);
	deff_entry_free (deff_runqueue);
	deff_reschedule ();
	task_exit ();
}


/** Called from a deff when it wants to exit after a certain delay */
__noreturn__ void deff_delay_and_exit (task_ticks_t ticks)
{
	task_sleep (ticks);
	deff_exit ();
}


/** Called from a deff when it wants to toggle between two images
 * on the low and high mapped pages, both in mono mode.
 * COUNT is the number of times to toggle.
 * DELAY is how long to wait between each change. */
void deff_swap_low_high (S8 count, task_ticks_t delay)
{
	while (--count >= 0)
	{
		dmd_show_other ();
		task_sleep (delay);
	}
}


/** Lower the priority of the currently running display effect.
This may cause it to be preempted by something more important. */
void deff_nice (enum _priority prio)
{
	/* If increasing the priority, then accept the change but
	nothing else really happens at the moment. */
	if (prio >= deff_runqueue->prio)
	{
		deff_runqueue->prio = prio;
		return;
	}

	/* TODO */
	/* Find the highest priority effect that is queued, and
	see if its priority is greater than 'prio'.  If it is,
	make this the new effect; otherwise, the current effect
	stays and just lower its priority. */
	if (0)
	{
	}
	else
	{
		deff_runqueue->prio = prio;
	}
}


/** Initialize the display effect subsystem. */
void deff_init (void)
{
	deff_initqueue (&deff_runqueue);
	deff_initqueue (&deff_waitqueue);
	deff_time_last_idle = 0;
	deff_timeout_disabled = 0;
}


/** Stop all running deffs */
void deff_stop_all (void)
{
	task_kill_gid (GID_DEFF);
	deff_stop_task ();
	dmd_alloc_low_clean ();
	dmd_show_low ();

	dbprintf ("deff_stop_all\n");
	if (deff_runqueue)
		deff_entry_free (deff_runqueue);

	if (deff_waitqueue)
	{
		deff_entry_t *entry = deff_waitqueue;
		do {
			deff_entry_t *next = (deff_entry_t *)entry->dll.next;
			deff_entry_free (entry);
			entry = next;
		} while (entry != deff_waitqueue);
	}

	deff_init ();
}


CALLSET_ENTRY (deff, any_device_enter)
{
	/* TODO : If there are any pending, short-lived deffs with D_TIMEOUT set,
	 * and there are no balls live on the playfield, wait here and give
	 * the deffs a chance to dequeue before returning. */
}


/** At idle time, stop any queued deffs that have timed out.
 * This routine is called once per second when the CPU is idle. */
CALLSET_ENTRY (deff, idle_every_second)
{
	/* Yes, update all of the queued timer entries that have D_TIMEOUT set. */
	/* But if timers are disabled, then don't do this. */
	if (deff_timeout_disabled == 0)
	{
restart:
		if (deff_waitqueue)
		{
			deff_entry_t *entry = deff_waitqueue;
			do {
				if (entry->flags & D_TIMEOUT)
				{
					entry->timeout--;
					dbprintf ("Deff %d timer at %d\n", entry->id, entry->timeout);
					if (entry->timeout == 0)
					{
						/* Remove this entry from the list and restart */
						deff_dequeue (&deff_waitqueue, entry);
						goto restart;
					}
				}
				entry = (deff_entry_t *)entry->dll.next;
			} while (entry != deff_waitqueue);
		}
	}
}


/** If both flippers are pressed and the current running deff
 * is abortable, then stop it. */
CALLSET_ENTRY (deff, flipper_abort)
{
	if (deff_runqueue 
		&& deff_runqueue->flags & D_ABORTABLE)
	{
		dbprintf ("Deff %d aborted.\n", deff_runqueue->id);
		deff_stop (deff_runqueue->id);
	}
}

