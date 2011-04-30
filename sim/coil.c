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

	/* Nonzero if this I/O line is disabled for simulation */
	int disabled;

	/* For divertor coils only, this is the multiplexer node that
		switches between the two possible destinations.
		For ball device coils, this is the device node which holds
		the balls. */
	struct ball_node *node;

	/* Next coil in the chain */
	struct sim_coil_state *chain;
};


/**
 * The global array of coil states
 */
struct sim_coil_state coil_states[PINIO_NUM_SOLS+4];

void coil_clone (unsigned int parent_id, unsigned int child_id)
{
	struct sim_coil_state *parent = coil_states + parent_id;
	struct sim_coil_state *child = coil_states + child_id;
	parent->chain = child;
	memset (child, 0, sizeof (struct sim_coil_state));
}


void device_coil_at_max (struct sim_coil_state *c)
{
	node_kick (c->node);
}


struct sim_coil_type device_type_coil = {
	.max_pos = 120,
	.on_step = 50,
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

#ifdef SOL_OUTHOLE
	if (solno == SOL_OUTHOLE)
		node_kick (&switch_nodes[MACHINE_OUTHOLE_SWITCH]);
#endif

#ifdef MACHINE_LAUNCH_SOLENOID
	if (solno == MACHINE_LAUNCH_SOLENOID)
		node_kick (&switch_nodes[MACHINE_SHOOTER_SWITCH]);
#endif

#ifdef MACHINE_KNOCKER_SOLENOID
	if (solno == MACHINE_KNOCKER_SOLENOID)
		simlog (SLC_DEBUG, "Thwack!");
#endif
}

struct sim_coil_type generic_type_coil = {
	.at_max = generic_coil_at_max,
	.max_pos = 40,
	.on_step = 4,
	.off_step = -1,
};

struct sim_coil_type flipper_power_type_coil = {
	/* TODO - trigger EOS when coil reaches peak */
	.max_pos = 32,
	.on_step = 2,
	.off_step = -1,
};

struct sim_coil_type flipper_hold_type_coil = {
	.max_pos = 32,
	.on_step = 0,
	.off_step = -1,
};

struct sim_coil_type outhole_type_coil = {
	.max_pos = 40,
	.on_step = 4,
	.off_step = -1,
};

void diverter_coil_0 (struct sim_coil_state *c)
{
	c->node->index = 0;
	c->node->next = c->node->mux_next[0];
}

void diverter_coil_1 (struct sim_coil_state *c)
{
	c->node->index = 1;
	c->node->next = c->node->mux_next[1];
}

struct sim_coil_type diverter_type_coil = {
	.at_rest = diverter_coil_0,
	.at_max = diverter_coil_1,
	.max_pos = 40,
	.on_step = 4,
	.off_step = -1,
};

void diverter_coil_init (unsigned int id, struct ball_node *node)
{
	struct sim_coil_state *c = coil_states + id;
	c->node = node;
	c->type = &diverter_type_coil;
}

void device_coil_init (unsigned int id, struct ball_node *node)
{
	struct sim_coil_state *c = coil_states + id;
	c->node = node;
	c->type = &device_type_coil;
}


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
	.max_pos = 16,
	.on_step = 1,
	.off_step = 0,
	//.unmonitored = 1,
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
static void sim_coil_update (struct sim_coil_state *c)
{
	struct sim_coil_state *m = c->master;

	if (c->master == NULL)
		return;

	/* If the IO is on and the coil has not reached its maximum,
	move it forward. */
	if (c->on && m->pos < m->type->max_pos)
	{
		m->pos += c->type->on_step;
		if (m->pos >= m->type->max_pos && !m->at_max)
		{
			m->pos = m->type->max_pos;
			m->at_max = 1;
			simlog (SLC_DEBUG, "Coil %d on", m - coil_states);
			if (m->type->at_max)
				m->type->at_max (c);
		}
	}
	/* Else, if the IO is off and the coil has not reached its rest state,
	move it backward. */
	else if (!c->on && m->pos > 0)
	{
		m->pos += c->type->off_step;
		if (m->pos <= 0)
		{
			m->pos = 0;
			m->at_max = 0;
			simlog (SLC_DEBUG, "Coil %d off", m - coil_states);
			if (m->type->at_rest)
				m->type->at_rest (c);
		}
	}

	if (c->chain)
		sim_coil_update (c->chain);

#if 0
	simlog (SLC_DEBUG, "Coil %d on=%d pos=%d of %d", m - coil_states,
		c->on, m->pos, m->type->max_pos);
#endif
	/* If the coil requires monitoring, and it is not at rest, then reschedule.
	Else, we are done until the CPU modifies it again. */
	if (!c->type->unmonitored && m->pos != 0)
		sim_time_register (1, FALSE, (time_handler_t)sim_coil_update, c);
	else
	{
		c->scheduled = 0;
	}
}


bool sim_coil_is_active (unsigned int coil)
{
	struct sim_coil_state *c = coil_states + coil;
	return (c->at_max != 0);
}


/**
 * Called when the CPU board writes to a solenoid signal.
 * coil identifies the signal; on is nonzero for active voltage.
 */
void sim_coil_change (unsigned int coil, unsigned int on)
{
	struct sim_coil_state *c = coil_states + coil;

	if (c->disabled)
		return;

	if (c->on != on)
	{
		c->on = on;
		if (!c->scheduled)
		{
			sim_time_register (1, FALSE, (time_handler_t)sim_coil_update, c);
			c->scheduled = 1;
		}
	}
}


#ifdef MACHINE_TZ
static void mach_coil_init (void)
{
	coil_states[SOL_GUMBALL_RELEASE].type = &motor_type_coil;

	coil_states[SOL_CLOCK_FORWARD].type = &motor_type_coil;
	coil_states[SOL_CLOCK_REVERSE].type = &motor_type_coil;
	coil_states[SOL_CLOCK_REVERSE].master = &coil_states[SOL_CLOCK_FORWARD];

	coil_states[47].master = NULL;
}
#endif


#if (MACHINE_FLIPTRONIC == 1)
static void fliptronic_coil_init (U8 power_sol)
{
	struct sim_coil_state *power_coil = coil_states + power_sol;
	struct sim_coil_state *hold_coil = power_coil + 1;

	power_coil->type = &flipper_power_type_coil;
	hold_coil->type = &flipper_hold_type_coil;
	hold_coil->master = power_coil;
	simlog (SLC_DEBUG, "Hold coil %d linked to power coil %d",
		power_coil - coil_states, hold_coil - coil_states);
}
#endif

void sim_coil_init (void)
{
	int devno;
	int sol;

	/* Initialize everything to zero first */
	for (sol = 0; sol < PINIO_NUM_SOLS; sol++)
	{
		char item_name[32];
		struct sim_coil_state *c = coil_states + sol;
		memset (c, 0, sizeof (struct sim_coil_state));
		if (MACHINE_SOL_FLASHERP (sol))
			c->type = &flasher_type_coil;
		else
			c->type = &generic_type_coil;
		c->master = c;
		snprintf (item_name, sizeof (item_name), "coil.%d.disabled", sol);
		conf_add (item_name, &c->disabled);
	}

	/* Note coils which are attached to ball devices */
	for (devno = 0; devno < NUM_DEVICES; devno++)
	{
		const device_properties_t *props = &device_properties_table[devno];
		device_coil_init (props->sol, &device_nodes[devno]);
	}

	/* Initialize Fliptronic coils */
#if (MACHINE_FLIPTRONIC == 1)
	fliptronic_coil_init (SOL_LL_FLIP_POWER);
	fliptronic_coil_init (SOL_LR_FLIP_POWER);
#ifdef SOL_UL_FLIP_POWER
	fliptronic_coil_init (SOL_UL_FLIP_POWER);
#endif
#ifdef SOL_UR_FLIP_POWER
	fliptronic_coil_init (SOL_UR_FLIP_POWER);
#endif
#endif

	/* Initialize machine specific coils */
#ifdef CONFIG_MACHINE_SIM
	mach_coil_init ();
#endif
}
