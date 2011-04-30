/*
 * Copyright 2010, 2011 by Brian Dominy <brian@oddchange.com>
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

/* This module tracks the places where a pinball may be located.
	Each ball is associated with a node indicating where it is currently.
	Normally, balls can only move according to the rules of the node graph.
	Every node has a 'next' node which says where it is normally 'kicked'.

	The node graph is mostly constructed automatically based on the
	machine description, but each machine can modify it if necessary to
	describe unusual ball paths.
	*/

extern device_properties_t device_properties_table[];
extern int sim_installed_balls;


/* Node types for ball devices.  A single node tracks the entire device,
   regardless of how many balls it can hold. */

void device_type_insert (struct ball_node *node, struct ball *ball)
{
	const device_properties_t *props = &device_properties_table[node->index];
	simlog (SLC_DEBUG, "Device %d insert, count=%d", node->index, node->count);
	/* Set the switch that just closed */
	sim_switch_set (props->sw[node->size - node->count], 1);
}

void device_type_remove (struct ball_node *node, struct ball *ball)
{
	const device_properties_t *props = &device_properties_table[node->index];
	simlog (SLC_DEBUG, "Device %d remove, count=%d", node->index, node->count);
	/* Clear the switch that just opened */
	sim_switch_set (props->sw[node->size - node->count - 1], 0);
}

struct ball_node_type device_type_node = {
	.insert = device_type_insert,
	.remove = device_type_remove,
};


/* Nodes for the open playfield */

struct ball_node_type open_type_node =  {
};


/* Node types for single balls detected by a switch, but without a ball device */

void switch_type_insert_or_remove (struct ball_node *node, struct ball *ball)
{
	simlog (SLC_DEBUG, "Switch %d holds %d balls", node->index, node->count);
#ifdef CONFIG_TZ
	if (ball->flags & 0x01)
	{
		simlog (SLC_DEBUG, "Slot prox ignored by Powerball");
		sim_switch_set (node->index, 0);
	}
	else
#endif
	sim_switch_set (node->index, node->count);
}

struct ball_node_type switch_type_node = {
	.insert = switch_type_insert_or_remove,
	.remove = switch_type_insert_or_remove,
};



struct ball_node_type mux_type_node = {
	.insert = NULL,
	.remove = NULL,
};


struct ball_node open_node;
struct ball_node device_nodes[MAX_DEVICES];
struct ball_node switch_nodes[NUM_SWITCHES];

struct ball the_ball[6]; /* TBD - don't hardcode count */

/* Return true if a node is full (can hold no more pinballs) */
bool node_full_p (struct ball_node *node)
{
	return node->count == node->size;
}


void node_kick_delayed (struct ball_node *node)
{
	node_kick (node);
}


/* Insert an unbound ball into a node. */
void node_insert (struct ball_node *node, struct ball *ball)
{
	unsigned int offset;

	if (ball->node)
	{
		simlog (SLC_DEBUG, "node_insert: %s already at %s",
			ball->name, ball->node->name);
		return;
	}

	if (node_full_p (node) || !node->type)
	{
		simlog (SLC_DEBUG, "node_insert: %s not allowed", node->name);
		return;
	}

	offset = (node->head + node->count) % node->size;
	node->ball_queue[offset] = ball;
	node->count++;
	ball->node = node;
	ball->pos = offset;
	if (node->type->insert)
		node->type->insert (node, ball);
#ifdef CONFIG_UI
	ui_update_ball_tracker (ball->index, node->name);
#endif
	simlog (SLC_DEBUG, "node_insert: added %s to %s, count=%d", ball->name, node->name, node->count);

	if (node->unlocked && !node_full_p (node->next))
		sim_time_register (100, FALSE, (time_handler_t)node_kick_delayed, node);
}


/* Remove the head of the node queue. */
struct ball *node_remove (struct ball_node *node)
{
	unsigned int offset;
	struct ball *ball;

	if (node->count == 0)
	{
		simlog (SLC_DEBUG, "node_remove: no balls in %s", node->name);
		return NULL;
	}

	offset = node->head % node->size;
	ball = node->ball_queue[offset];
	if (!ball)
	{
		simlog (SLC_DEBUG, "node_remove: count=%d but ball is null?", node->count);
		return NULL;
	}

	node->head++;
	node->count--;
	ball->node = NULL;
	if (node->type->remove)
		node->type->remove (node, ball);
#ifdef CONFIG_UI
	ui_update_ball_tracker (ball->index, "Free");
#endif
	simlog (SLC_DEBUG, "node_remove: took %s from %s", ball->name, node->name);

	if (node->prev && node->prev->unlocked && node->prev->count != 0)
	{
		node_kick (node->prev);
	}

	return ball;
}

/* The minimum delay in milliseconds */
#define MIN_DELAY 100

void node_insert_delay_update (struct ball *ball)
{
	ball->timer -= MIN_DELAY;
	if (ball->timer <= 0)
	{
		struct ball_node *dst = ball->node;
		ball->node = NULL;
		node_insert (dst, ball);
	}
	else
		sim_time_register (MIN_DELAY, FALSE,
			(time_handler_t)node_insert_delay_update, ball);
}


/* Insert a ball at a node after some number of milliseconds has expired.
Until then the ball is not attached to any node. */
void node_insert_delay (struct ball_node *dst, struct ball *ball,
	unsigned int delay)
{
	ball->node = dst;
	ball->timer = delay;
	sim_time_register (MIN_DELAY, FALSE,
		(time_handler_t)node_insert_delay_update, ball);
}


/* Move a ball from one location to another.  The two nodes do not
	have to be connected via the default topology. */
void node_move (struct ball_node *dst, struct ball_node *src)
{
	struct ball *ball;

	if (!dst || !src)
		return;

	/* If there are already too many balls in the destination, then
	don't allow the operation: it must remain where it is. */
	if (node_full_p (dst))
	{
		simlog (SLC_DEBUG, "node_kick %s: destination %s is full", src->name, dst->name);
		return;
	}

	ball = node_remove (src);
	if (!ball)
	{
		simlog (SLC_DEBUG, "node_kick: no balls in %s", src->name);
		return;
	}

	simlog (SLC_DEBUG, "node_kick: %s -> %s", src->name, dst->name);
	/* If no delay is associated with a movement from the source, then
	the move is instantaneous.  Otherwise, it will be performed later; in
	the meantime the ball is not associated with any node. */
	if (src->delay == 0)
		node_insert (dst, ball);
	else
	{
#ifdef CONFIG_UI
	ui_update_ball_tracker (ball->index, src->name);
#endif
		node_insert_delay (dst, ball, src->delay);
	}
}


/* To kick a node means to force a pinball that is there to move to
	the next node. */
void node_kick (struct ball_node *node)
{
	node_move (node->next, node);
}


void node_join (struct ball_node *source, struct ball_node *sink, unsigned int delay)
{
	source->next = sink;
	source->delay = delay;
	sink->prev = source;
}


/* Initialize the node graph for this machine.
	This creates the nodes that match the topology of the game, using
	some of the machine-specific parameters to guide things.
	Last, the ball trough is populated with all of the pinballs. */
void node_init (void)
{
	unsigned int i;

	/* Create nodes for all playfield switches.  Not all of these will
	be used necessarily.  The default is for all switches to drain to the
	open playfield.  The switch will remain active for 100ms before it moves. */
	for (i=0; i < NUM_SWITCHES; i++)
	{
		struct ball_node *node = switch_nodes + i;
		node->name = names_of_switches[i];
		node->type = &switch_type_node;
		node->index = i;
		node->unlocked = 1;
		node->size = 1;
		node_join (node, &open_node, 100);
	}

	/* Create nodes for the ball devices.  Trough leads to shooter;
	everything else leads to the open playfield as for the switches. */
	for (i=0; i < MAX_DEVICES; i++)
	{
		device_nodes[i].type = &device_type_node;
		device_nodes[i].index = i;
		device_nodes[i].size = device_properties_table[i].sw_count;
		device_nodes[i].name = device_properties_table[i].name;
		device_nodes[i].unlocked = 0;
#if defined(DEVNO_TROUGH) && defined(MACHINE_SHOOTER_SWITCH)
		if (i == DEVNO_TROUGH)
			node_join (&device_nodes[i], &shooter_node, 50);
		else
#endif
			node_join (&device_nodes[i], &open_node, 0);
	}

	/* The outhole and the shooter switches, initialized above, can
	actually hold more pinballs than 1; they just queue up undetected.
	They are also unlocked, meaning that they stay there until something
	forces them to move on. */
#ifdef MACHINE_OUTHOLE_SWITCH
	outhole_node.size = MAX_BALLS_PER_NODE;
	outhole_node.unlocked = 0;
	node_join (&outhole_node, &trough_node, 100);
#endif
#ifdef MACHINE_SHOOTER_SWITCH
	shooter_node.size = MAX_BALLS_PER_NODE;
	shooter_node.unlocked = 0;
	node_join (&shooter_node, &open_node, 0);
#endif

	/* Initialize the open playfield node, which feeds into the trough
	(or outhole if present). */
	open_node.name = "Playfield";
	open_node.type = &open_type_node;
	open_node.size = MAX_BALLS_PER_NODE;
	open_node.unlocked = 0;
#ifdef drain_node
	node_join (&open_node, &drain_node, 0);
#endif

	/* Fixup the graph in a machine-specific way */
#ifdef CONFIG_MACHINE_SIM
	mach_node_init ();
#endif

#ifdef DEVNO_TROUGH
	/* Create the pinballs and dump them into the trough.
		Actually, we dump them onto the playfield and force them to drain.
		This lets us install more balls than the trough can hold, as if
		you just dropped them onto the playfield. */
	for (i=0; i < sim_installed_balls; i++)
	{
		the_ball[i].node = NULL;
		strcpy (the_ball[i].name, "Ball X");
		the_ball[i].name[5] = i + '0';
		the_ball[i].index = i;
		the_ball[i].flags = 0;

		node_insert (&open_node, &the_ball[i]);
		node_kick (&open_node);
	}
#endif
}

