/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

#ifndef _HWSIM_BALL_H
#define _HWSIM_BALL_H


/** The maximum number of balls that can be tracked in simulation */
#define SIM_MAX_BALLS 8

#define MAX_BALL_LOCATIONS 128

#define SIM_LOCATION_NONE 0
#define SIM_NO_BALL_HERE -1


struct ball;
struct ball_node;

/*	The node type is used to subclass a node's behavior. */
struct ball_node_type
{
	void (*insert) (struct ball_node *node, struct ball *ball);
	void (*remove) (struct ball_node *node, struct ball *ball);
};

#define MAX_BALLS_PER_NODE 8

/* A node reflects a position on the playfield where a pinball
	may rest indefinitely.  Each node implements a first-in
	first-out queue of ball objects, defined below. */
struct ball_node
{
	/* A pointer to the next node, which is the default location
	that a kick operation will move a ball to. */
	struct ball_node *next;

	/* Likewise, pointer to the previous node.  Note that now,
	there can only be 1 previous node. */
	struct ball_node *prev;

	/* The maximum queue depth here; how many balls can stack up.
	When this limit is reached, nodes that feed into this will
	begin to back up. */
	unsigned int size;

	/* The actual number of balls here now */
	unsigned int count;

	/* Pointers to the ball objects that are stored here */
	struct ball *ball_queue[MAX_BALLS_PER_NODE];

	/* The offset in the queue of the next ball to be kicked */
	unsigned int head;

	/* The type (subclass) structure for this node */
	struct ball_node_type *type;

	/* A type-dependent value */
	unsigned int index;

	/* For multiplexers only - a list of the downstream nodes.  This
	is used instead of 'next' */
	struct ball_node *mux_next[4];

	/* The time delay before a ball transitions to the next node.
	When a kick occurs, the ball is removed from the node immediately,
	but does not appear in the next node until this delay completes. */
	unsigned int delay;

	/* If nonzero, the node is 'unlocked', and balls will transition
	to the next node automatically */
	unsigned int unlocked;

	/* The name of the node used for debugging */
	const char *name;
};


/* A ball object represents a physical pinball, and tracks its
	whereabouts through the machine. */
struct ball
{
	struct ball_node *node;
	unsigned int pos;
	int timer;
	unsigned int flags;
	unsigned int index;
	char name[32];
};

extern struct ball_node open_node;
extern struct ball_node device_nodes[];
extern struct ball_node switch_nodes[];

/* Aliases for nodes present on most machines */
#define shooter_node switch_nodes[MACHINE_SHOOTER_SWITCH]
#define outhole_node switch_nodes[MACHINE_OUTHOLE_SWITCH]
#define trough_node device_nodes[DEVNO_TROUGH]

#ifdef MACHINE_OUTHOLE_SWITCH
#define drain_node outhole_node
#elif defined(DEVNO_TROUGH)
#define drain_node trough_node
#endif

extern struct ball_node_type switch_type_node;
extern struct ball_node_type mux_type_node;
extern struct ball_node_type open_type_node;

void node_insert (struct ball_node *node, struct ball *ball);
void node_kick (struct ball_node *node);
void node_move (struct ball_node *dst, struct ball_node *src);
void node_join (struct ball_node *source, struct ball_node *sink, unsigned int delay);
void node_init (void);
void node_insert_delay (struct ball_node *dst, struct ball *ball, unsigned int delay);

void mux_type_insert (struct ball_node *node, struct ball *ball);

#endif /* _HWSIM_BALL_H */
