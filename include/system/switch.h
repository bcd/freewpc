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

#ifndef _SYS_SWITCH_H
#define _SYS_SWITCH_H

/** Small integer typedef for a switch number */
typedef U8 switchnum_t;

/** Switch flags */
#define SW_OPTICAL	0x01 /* Switch is active when it is _open_ */
#define SW_EDGE		0x02 /* Switch is handled on any edge */
#define SW_IN_GAME	0x08 /* Only service switch during a game */
#define SW_PLAYFIELD	0x10 /* Declares that switch is 'on the playfield' */
#define SW_NOVALID   0x20 /* Switch does not mark playfield valid */
#define SW_IN_TEST	0x40 /* Service switch in test mode; default is no */

/** Switch handler prototype form */
typedef void (*switch_handler_t) (void);

/** A switch descriptor.  Contains all of the static information
 * about a particular switch */
typedef struct
{
	/** A function to call when the switch produces an event.
	 * All functions are assumed to be callsets, and located in the
	 * callset page of the ROM. */
	switch_handler_t fn;

	/** A set of flags that control when switch events are produced */
	U8 flags;

	/** If nonzero, indicates a lamp that is associated with the switch */
	U8 lamp;

	/** If nonzero, indicates a sound to be made on any switch event */
	sound_code_t sound;

	/** Indicates how long the switch must remain in the active
	 * state before an event is generated.   If zero, then only
	 * a quick 4ms debounce is done.  The value is given
	 * in 16ms ticks. */
	task_ticks_t debounce;

	/** If nonzero, indicates the device driver associated with this
	 *switch. */
	U8 devno;
} switch_info_t;


extern const U8 mach_opto_mask[];
extern const U8 mach_edge_switches[];

/** The maximum number of switches that can be queued at
 * a time.  Queueing is necessary only for switches that
 * require a long (more than 4ms) debounce interval.
 * Keeping this as a power of 2 generates more efficient code. */
#define MAX_QUEUED_SWITCHES 16

#define SW_DEVICE_DECL(real_devno)	((real_devno) + 1)

/** True if a switch is part of a ball container */
#define SW_HAS_DEVICE(sw)	(sw->devno != 0)

/** Returns the container ID that a switch belongs to */
#define SW_GET_DEVICE(sw)	(sw->devno - 1)

/** On a pre-Fliptronic game, the flipper button switches are in
the ordinary 8x8 switch matrix.  On Fliptronic games, these are
accessed separately and tracked in a "9th" switch column internally.
Define SW_LEFT_BUTTON and SW_RIGHT_BUTTON to the correct values
depending on the system type. */
#ifdef CONFIG_PLATFORM_WPC
#if (MACHINE_FLIPTRONIC == 1)
#define SW_LEFT_BUTTON SW_L_L_FLIPPER_BUTTON
#define SW_RIGHT_BUTTON SW_L_R_FLIPPER_BUTTON
#else
#define SW_LEFT_BUTTON SW_LEFT_FLIPPER
#define SW_RIGHT_BUTTON SW_RIGHT_FLIPPER
#endif
#endif

#define NUM_SWITCHES PINIO_NUM_SWITCHES

#define SWITCH_BITS_SIZE	(NUM_SWITCHES / 8)

#define SW_COL(x)			((x) >> 3)
#define SW_ROW(x)			((x) & 0x07)
#define SW_ROWMASK(x)	single_bit_set (SW_ROW(x))


/** The form for a matrix of bits, one per switch */
typedef U8 switch_bits_t[SWITCH_BITS_SIZE];

extern __fastram__ U8 sw_raw[SWITCH_BITS_SIZE];


/** Poll the raw state of a switch.  Returns zero if open, nonzero
if closed. */
extern inline U8 rt_switch_poll (const switchnum_t sw_num)
{
	/* TODO - for PIC games, this value is valid only when the PIC
	is known to be initialized properly. */
	return sw_raw[SW_COL(sw_num)] & SW_ROWMASK(sw_num);
}


/**
 * Declare that another switch (or event) can follow the one that just closed.
 *
 * FIRST and SECOND indicate the switches/events; they do not have
 * to match the event name exactly.
 *
 * TIMEOUT is a TIME constant that says how long at most it will take for
 * the second event to occur after the first.
 *
 * event_should_follow() and event_can_follow() work identically.
 */
#define event_can_follow(first,second,timeout) \
	timer_restart_free (GID_ ## first ## _FOLLOWED_BY_ ## second, timeout)

#define event_should_follow(f,s,t) event_can_follow(f,s,t)


/**
 * Like event_can_follow(), but used when the second event is associated
 * with a ball container.
 *
 * With this version, timers are paused, since the ball is "en route" to
 * a ball device where it will be held up and timers will be paused anyway.
 */
#define device_switch_can_follow(first, second, timeout) \
	do { \
		event_can_follow (first, second, timeout); \
		timer_restart_free (GID_DEVICE_SWITCH_WILL_FOLLOW, timeout); \
	} while (0)


/**
 * Indicate that the second event did indeed follow.  This should be called
 * from the second event handler.
 *
 * If this returns TRUE, it means that the second event did indeed happen
 * after the first one, within the timeout period.  If FALSE, then the
 * first event did not occur earlier.
 *
 * event_did_follow() assumes that the events are 'ball events' related to
 * a ball moving on the playfield.  Because of this, event_did_follow()
 * will always return FALSE in multiball, because the logic can be fooled
 * by multiple balls on the table.  If dealing with non-ball events
 * where this check should not be done, use nonball_event_did_follow().
 */
#define nonball_event_did_follow(first,second) \
	timer_kill_gid (GID_ ## first ## _FOLLOWED_BY_ ## second)

#define ball_event_did_follow(first,second) \
	(single_ball_play () && nonball_event_did_follow(first, second))

/* The default is to assume a playfield ball event */
#define event_did_follow(f,s)		ball_event_did_follow(f,s)


/** A macro for implementing button event handlers, which need to be called
when a button is first pressed and continually as it is held.
	SW is the id of the switch.
	EVENT is the name of an event that is thrown to do the actual processing.
	DELAY is the amount of time that the button must be held before
	it is considered to repeat.  Only one call to EVENT happens during
	this initial time period.
	REPEAT is the frequency at which the event is rethrown while held.
	After the initial delay, the EVENT is called every REPEAT.
 */
#define button_invoke(sw,event,delay,repeat) \
	do { \
		U8 i; \
		callset_invoke (event); \
		for (i=0; i < 8; i++) \
			if (!switch_poll (sw)) \
				goto done; \
			else \
				task_sleep (delay / 8); \
	 \
		while (switch_poll (sw)) \
		{ \
			callset_invoke (event); \
			task_sleep (repeat); \
		} \
	done:; \
	} while (0)


#if (MACHINE_PIC == 1)
extern inline void pic_rtt_start (void)
{
	/* Write the command to retrieve the unlock counter */
	wpc_write_pic (WPC_PIC_COUNTER);
}

extern inline void pic_rtt_finish (void)
{
	/* Read back the unlock counter.  If nonzero, nothing
	to do. */
	if (likely (wpc_read_pic ()))
		return;

	/* Stop if the PIC did not initialize correctly. */
	extern U8 pic_unlock_ready;
	if (unlikely (!pic_unlock_ready))
		return;

	/* Unlock is required */
	VOIDCALL (pic_rtt_unlock);
}
#endif /* MACHINE_PIC */


void switch_init (void);
void switch_rtt (void);
void switch_periodic (void);
void switch_sched_task (void);
void switch_idle (void);
bool switch_poll (const switchnum_t sw);
bool switch_is_opto (const switchnum_t sw);
bool switch_poll_logical (const switchnum_t sw);
const switch_info_t *switch_lookup (const switchnum_t sw) __pure__;
U8 switch_lookup_lamp (const switchnum_t sw) __pure__;
void switch_queue_dump (void);

#if (MACHINE_PIC == 1)
__init__ void pic_init (void);
__init__ void pic_render_serial_number (void);
extern bool pic_invalid;
#define switch_scanning_ok() (!pic_invalid)
#else
#define switch_scanning_ok() TRUE
#endif

#endif /* _SYS_SWITCH_H */
