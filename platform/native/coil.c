/*
 * Copyright 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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

extern device_properties_t device_properties_table[];

struct sim_coil_state;

/**
 * A type object stores properties specific to different types of
 * coil devices.
 */
struct sim_coil_type
{
	/* The maximum position for the coil */
	unsigned int max_pos;

	/* The coil offset applied when it is on */
	int on_step;

	/* The coil offset applied when it is off */
	int off_step;

	/* Nonzero if the coil output is unmonitored: it cannot
	change states unless the CPU touches it */
	int unmonitored;

	/* Called when the coil reaches its max position */
	void (*at_max) (struct sim_coil_state *c);

	/* Called when the coil returns to its rest position */
	void (*at_rest) (struct sim_coil_state *c);
};


/**
 * The state of each physical coil (or coil-like device).
 */
struct sim_coil_state
{
	/* The output of the line that drives the coil.  This
	corresponds to the last write from the CPU. */
	unsigned int on;

	/* Denotes the position of the coil.  POS is zero for
	a coil that is at rest, and at type->max_pos when it is fully
	engaged.  This interpretation is for true solenoids only.
	For motors or flashers, pos is not really used. */
	int pos;

	/* Nonzero if this coil has been scheduled: an update
	routine has been scheduled to modify the state as time
	progresses, even when the CPU is not actively writing it.
	We need this to avoid scheduling the same coil more than
	once, because we cannot cancel schedule entries already
	in progress. */
	unsigned int scheduled;

	/* The ball device associated with this coil.  Not used. */
	unsigned int devno;

	/* This smoothes out the values for 'on'; it is 1 when
	the coil is logically on, and 0 when logically off. */
	unsigned int at_max;

	/* The 'master' coil, used for flipper coils, where there are
	two physical coils that control a single solenoid arm.
	The POS field of the master is shared for both the power and
	the hold coil; the POS field for the hold is not used.
	For other coils, master points to itself. */
	struct sim_coil_state *master;

	/* The type object for this device */
	struct sim_coil_type *type;
};


/**
 * The global array of coil states
 */
struct sim_coil_state coil_states[SOL_COUNT];


void device_coil_at_max (struct sim_coil_state *c)
{
	unsigned int solno = c - coil_states;
	const device_properties_t *props = &device_properties_table[c->devno];
	int n;

	simlog (SLC_DEBUG, "Kick device solenoid %d (dev %d)", solno, c->devno);

	for (n = 0; n < props->sw_count; n++)
	{
		if (linux_switch_poll_logical (props->sw[n]))
		{
			simlog (SLC_DEBUG, "Device %d release", c->devno);
			sim_switch_toggle (props->sw[n]);

			/* Where does the ball go from here?
			Normally device kickout leads to unknown areas of
			the playfield.
			The shooter switch could be handled though. */
#if defined(DEVNO_TROUGH) && defined(MACHINE_SHOOTER_SWITCH)
			if (c->devno == DEVNO_TROUGH)
			{
				sim_switch_toggle (MACHINE_SHOOTER_SWITCH);
			}
#endif
			break;
		}
	}
}


struct sim_coil_type device_type_coil = {
	.max_pos = 5,
	.on_step = 1,
	.off_step = -1,
	.at_max = device_coil_at_max,
};


void flasher_coil_at_max (struct sim_coil_state *c)
{
	unsigned int solno = c - coil_states;
	simlog (SLC_DEBUG, "Flasher %d pulse", solno);
}

struct sim_coil_type flasher_type_coil = {
	.max_pos = 3,
	.at_max = flasher_coil_at_max,
};

void generic_coil_at_max (struct sim_coil_state *c)
{
	unsigned int solno = c - coil_states;

	simlog (SLC_DEBUG, "Kick generic solenoid %d", solno);

	/* If it's the outhole kicker, simulate the ball being
	moved off the outhole into the trough */
#if defined(MACHINE_OUTHOLE_SWITCH) && defined(DEVNO_TROUGH)
	if (solno == SOL_OUTHOLE &&
		linux_switch_poll_logical (MACHINE_OUTHOLE_SWITCH))
	{
		/* Simulate kicking the ball off the outhole into the trough */
		simlog (SLC_DEBUG, "Outhole kick");
		sim_switch_toggle (MACHINE_OUTHOLE_SWITCH);
		sim_switch_toggle (device_properties_table[DEVNO_TROUGH].sw[0]);
	}
#endif
}

struct sim_coil_type generic_type_coil = {
	.max_pos = 5,
	.on_step = 1,
	.off_step = -1,
};

struct sim_coil_type flipper_type_coil = {
	.max_pos = 5,
	.on_step = 1,
	.off_step = -1,
};

struct sim_coil_type outhole_type_coil = {
	.max_pos = 5,
	.on_step = 1,
	.off_step = -1,
};

void motor_coil_at_rest (struct sim_coil_state *c)
{
	unsigned int solno = c - coil_states;
	c->pos = c->type->max_pos;
	simlog (SLC_DEBUG, "Motor %d @ %d", solno, c->pos);
}

void motor_coil_at_max (struct sim_coil_state *c)
{
	unsigned int solno = c - coil_states;
	c->pos = 0;
	simlog (SLC_DEBUG, "Motor %d @ %d", solno, c->pos);
}

struct sim_coil_type motor_type_coil = {
	.at_rest = motor_coil_at_rest,
	.at_max = motor_coil_at_max,
	.max_pos = 32,
	.on_step = 4,
	.off_step = 0,
	.unmonitored = 1,
};

/**
 * Update the state machine for a coil.
 *
 * The complexity here is that software can pulse the solenoid rapidly
 * between on and off; the goal is to detect when a single 'kick' operation
 * has occurred.  That requires ignoring brief off-periods and only
 * considering that it is been on a majority of the time in the recent past.
 *
 * Also, once a kick has occurred, it is not considered to kick again until
 * it has returned to the resting position (in reality, to allow another ball
 * to enter the kicking area).
 *
 * This function uses periodic callbacks to keep the solenoid state updated
 * even when no writes are emitted from the CPU; the power driver board
 * latches state.
 */
void sim_coil_update (struct sim_coil_state *c)
{
	/* If the IO is on and the coil has not reached its maximum,
	move it forward. */
	if (c->on && c->pos < c->type->max_pos)
	{
		c->pos += c->type->on_step;
		if (c->pos >= c->type->max_pos && !c->at_max)
		{
			c->pos = c->type->max_pos;
			c->at_max = 1;
			if (c->type->at_max)
				c->type->at_max (c);
		}
	}
	/* Else, if the IO is off and the coil has not reached its rest state,
	move it backward. */
	else if (!c->on && c->pos > 0)
	{
		c->pos -= c->type->off_step;
		if (c->pos <= 0)
		{
			c->pos = 0;
			c->at_max = 0;
			if (c->type->at_rest)
				c->type->at_rest (c);
		}
	}

	/* If the coil requires monitoring, and it is not at rest, then reschedule.
	Else, we are done until the CPU modifies it again. */
	if (!c->type->unmonitored && c->pos != 0)
		sim_time_register (4, FALSE, (time_handler_t)sim_coil_update, c);
	else
	{
		c->scheduled = 0;
	}
}


/**
 * Called when the CPU board writes to a solenoid signal.
 * coil identifies the signal; on is nonzero for active voltage.
 */
void sim_coil_change (unsigned int coil, unsigned int on)
{
	struct sim_coil_state *c = coil_states + coil;

	on = !!on;
	if (c->on != on)
	{
		c->on = on;
		if (!c->scheduled)
		{
			sim_time_register (4, FALSE, (time_handler_t)sim_coil_update, c);
			c->scheduled = 1;
		}
	}
}


#ifdef MACHINE_TZ
void tz_sim_init (void)
{
	coil_states[SOL_GUMBALL_RELEASE].type = &motor_type_coil;

	coil_states[SOL_CLOCK_FORWARD].type = &motor_type_coil;
	coil_states[SOL_CLOCK_REVERSE].type = &motor_type_coil;
	coil_states[SOL_CLOCK_REVERSE].master = &coil_states[SOL_CLOCK_FORWARD];
}
#endif


void sim_coil_init (void)
{
	int devno;
	int sol;

	/* Initialize everything to zero first */
	for (sol = 0; sol < SOL_COUNT; sol++)
	{
		struct sim_coil_state *c = coil_states + sol;
		memset (c, 0, sizeof (struct sim_coil_state));
		if (MACHINE_SOL_FLASHERP (sol))
			c->type = &flasher_type_coil;
		else
			c->type = &generic_type_coil;
		c->master = c;
	}

	/* Note coils which are attached to ball devices */
	for (devno = 0; devno < NUM_DEVICES; devno++)
	{
		const device_properties_t *props = &device_properties_table[devno];
		struct sim_coil_state *c = coil_states + props->sol;
		c->type = &device_type_coil;
		c->devno = devno;
	}

#ifdef MACHINE_TZ
	tz_sim_init ();
#endif
}
