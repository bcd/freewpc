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

#ifndef _SYS_DEFF_H
#define _SYS_DEFF_H


/** Type for a display effect ID */
typedef U8 deffnum_t;

/** Type for a display effect function */
typedef void (*deff_function_t) (void);

#ifdef CONFIG_NO_DEFFS
#define DEFF_FUNCTION(_func,_page) .fn=deff_exit, .page=SYS_PAGE
#else
#define DEFF_FUNCTION(_func,_page) .fn=_func, .page=_page
#endif

/** The flags for a normal display effect with no special requirements */
#define D_NORMAL	0x0

/** Set for a deff that wants to run indefinitely, regardless of whether
or not it can get the display.  When this bit is clear, if the deff cannot 
be started right away, it is immediately dropped.  Otherwise, it is placed 
onto the deff queue.  Likewise, the deff may be preempted by a higher-
priority deff later, and this bit will ensure that the deff is requeued
and can be restarted later.  A deff marked D_QUEUED may need to be
explicitly stopped via deff_stop() when conditions change, or the deff
itself may exit on its own if it decides it no longer needs to run. */
#define D_QUEUED 0x1

/** Set for a deff that should not be queued for a long period of time,
should it fail to get the display.  This setting also requires that
D_QUEUED be set.  When a deff is queued, it is timestamped and if the
deff remains on the queue for longer than some predetermined amount
of time, it is automatically stopped.  Note that the timeout varies
depending on what is going on; if a ball becomes locked up in a device,
we can hold the ball and service more deffs than if the ball is
on the playfield. */
#define D_TIMEOUT 0x2

/** Set for a deff that is abortable (by pressing both flipper buttons). */
#define D_ABORTABLE 0x4

/** Set for a deff that pauses kickouts.  While this deff is active
 * (not just queued -- it must actually have the display), all kickouts
 * are postponed.  When the deff finishes, is stopped, or is aborted,
 * kickouts can resume.  It would be better if trough kickouts were
 * not affected by this (it's OK to park a ball at the plunger while
 * the deff runs.) */
#define D_PAUSE 0x8

/** Set for a deff that wants to display the last score that was
 * earned by the player.  This requires that the score value be
 * copied to a safe location where it remain available throughout
 * the deff, even if other score switches are tripped in the
 * meantime. */
#define D_SCORE 0x10

/** Set for a deff that is allowed to preempt itself */
#define D_RESTARTABLE 0x20

/** A constant descriptor for a display effect. */
typedef struct
{
	/** Various flags (the D_ values given above) */
	U8 flags;

	/** The priority of the effect */
	U8 prio;

	/** The function to be spawned as a task to implement the effect */
	deff_function_t fn;

	/** The ROM page in which the function resides */
	U8 page;
} deff_t;

enum _priority;

extern const deff_t deff_table[];

extern void (*deff_component_table[4]) (void);

deffnum_t deff_get_active (void);
void deff_start (deffnum_t dn);
void deff_stop (deffnum_t dn);
void deff_restart (deffnum_t dn);
void deff_start_highest_priority (void);
__noreturn__ void deff_exit (void);
__noreturn__ void deff_delay_and_exit (task_ticks_t ticks);
void deff_start_bg (deffnum_t dn, enum _priority prio);
void deff_update (void);
void deff_start_sync (deffnum_t dn);
void deff_init (void);
void deff_stop_all (void);
void deff_queue_add (deffnum_t id, U16 timeout);


/* The deff components module offers inline functions for
display effects that want to update multiple parts of the display
independently. */

extern inline void deff_init_components (void)
{
	deff_component_table[0] = null_function;
	deff_component_table[1] = null_function;
	deff_component_table[2] = null_function;
	deff_component_table[3] = null_function;
}


extern inline void deff_set_component (U8 cnum, void (*function) (void))
{
	deff_component_table[cnum] = function;
}


extern inline void deff_call_components (void)
{
	(*deff_component_table[0]) ();
	(*deff_component_table[1]) ();
	(*deff_component_table[2]) ();
	(*deff_component_table[3]) ();
}


#ifdef CONFIG_DMD_OR_ALPHA
extern inline __noreturn__ void generic_deff (
	const char *line1, const char *line2)
{
	dmd_alloc_low_clean ();
	if (line1 && line2)
	{
		font_render_string_center (&font_fixed6, 64, 10, line1);
		font_render_string_center (&font_fixed6, 64, 21, line2);
	}
	else
	{
#if (MACHINE_DMD == 1)
		font_render_string_center (&font_fixed6, 64, 16, line1);
#else
		font_render_string_center (&font_fixed6, 64, 10, line1);
#endif
	}
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}
#endif

#ifndef MACHINE_CUSTOM_AMODE
void default_amode_deff (void);
#endif

#ifndef DEFF_AMODE
#define DEFF_AMODE 1
#endif

#endif /* _SYS_DEFF_H */

