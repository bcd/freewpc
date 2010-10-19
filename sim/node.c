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

#include <freewpc.h>
#include <simulation.h>

/* This module tracks the places where a pinball may be located.
	Each ball is associated with a node indicating where it is currently.
	Normally, balls can only move according to the rules of the node graph.
	Every node has a 'next' node which says where it is normally 'kicked'.
	*/

extern device_properties_t device_properties_table[];
extern int sim_installed_balls;


/* Nodes for positions within a ball device */

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

/* Nodes for single balls detected by a switch, but without a ball device */

void switch_type_insert_or_remove (struct ball_node *node, struct ball *ball)
{
	simlog (SLC_DEBUG, "Switch %d holds %d balls", node->index, node->count);
	sim_switch_set (node->index, node->count);
}

struct ball_node_type switch_type_node = {
	.insert = switch_type_insert_or_remove,
	.remove = switch_type_insert_or_remove,
};


struct ball_node_type mux_type_node = {
};


struct ball_node open_node;
struct ball_node device_nodes[MAX_DEVICES];
struct ball_node shooter_node;
struct ball_node outhole_node;
#ifdef MACHINE_TZ
struct ball_node far_left_trough_node;
struct ball_node autofire_node;
struct ball_node right_spiral_node;
struct ball_node gumball_machine_node;
struct ball_node gumball_exit_node;
struct ball_node camera_node;
struct ball_node piano_node;
struct ball_node slot_prox_node;
#endif

struct ball the_ball[6];

/* Return true if a node is full (can hold no more pinballs) */
bool node_full_p (struct ball_node *node)
{
	return node->count == node->size;
}


/* Insert an unbound ball into a node. */
void node_insert (struct ball_node *node, struct ball *ball)
{
	unsigned int offset;

	if (node_full_p (node) || ball->node || !node->type)
	{
		simlog (SLC_DEBUG, "node_insert: not allowed");
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
	simlog (SLC_DEBUG, "node_insert: added %s to %s", ball->name, node->name);

	if (node->unlocked && !node_full_p (node->next))
	{
		node_kick (node);
	}
}


/* Remove the head of the node queue. */
struct ball *node_remove (struct ball_node *node)
{
	unsigned int offset;
	struct ball *ball;

	if (node->count == 0)
	{
		simlog (SLC_DEBUG, "node_remove: no balls here");
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


void node_insert_after_delay (struct ball *ball)
{
	struct ball_node *dst = ball->node;
	ball->node = NULL;
	node_insert (dst, ball);
}


/* Kick a node, which will force the ball at the head of
	the node queue into the next node. */
void node_move (struct ball_node *dst, struct ball_node *src)
{
	struct ball *ball;

	if (!dst || !src)
		return;

	if (node_full_p (dst))
	{
		simlog (SLC_DEBUG, "node_kick %s: destination %s is full", src->name, dst->name);
		return;
	}

	ball = node_remove (src);
	if (!ball)
	{
		simlog (SLC_DEBUG, "node_kick: no ball present here");
		return;
	}

	if (src->delay == 0)
		node_insert (dst, ball);
	else
	{
		ball->node = dst;
		sim_time_register (src->delay, FALSE,
			(time_handler_t)node_insert_after_delay, ball);
	}
}


void node_kick (struct ball_node *node)
{
	node_move (node->next, node);
}


void device_node_kick (unsigned int devno)
{
	node_kick (&device_nodes[devno]);
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

	/* Create nodes for the ball devices.  Trough leads to shooter;
	everything else leads to playfield */
	for (i=0; i < MAX_DEVICES; i++)
	{
		device_nodes[i].type = &device_type_node;
		device_nodes[i].index = i;
		device_nodes[i].size = device_properties_table[i].sw_count;
		device_nodes[i].name = device_properties_table[i].name;
#ifdef DEVNO_TROUGH
		if (i == DEVNO_TROUGH)
			node_join (&device_nodes[i], &shooter_node, 50);
		else
#endif
			node_join (&device_nodes[i], &open_node, 0);
	}

	/* Create a node for the outhole, which leads to the trough */
#ifdef MACHINE_OUTHOLE_SWITCH
	outhole_node.name = "Outhole";
	outhole_node.type = &switch_type_node;
	outhole_node.index = MACHINE_OUTHOLE_SWITCH;
	outhole_node.size = MAX_BALLS_PER_NODE;
	node_join (&outhole_node, &trough_node, 100);
#endif

	/* Create a node for the shooter, which leads to the playfield */
#ifdef MACHINE_SHOOTER_SWITCH
	shooter_node.name = "Shooter";
	shooter_node.type = &switch_type_node;
	shooter_node.index = MACHINE_SHOOTER_SWITCH;
	shooter_node.size = MAX_BALLS_PER_NODE;
	node_join (&shooter_node, &open_node, 0);
#endif

	/* Initialize the open playfield node, which feeds into the trough
	or outhole */
	open_node.name = "Playfield";
	open_node.type = &open_type_node;
	open_node.size = MAX_BALLS_PER_NODE;
#ifdef MACHINE_OUTHOLE_SWITCH
	node_join (&open_node, &outhole_node, 0);
#elif defined(DEVNO_TROUGH)
	node_join (&open_node, &trough_node, 0);
#endif

#ifdef DEVNO_TROUGH
	/* Create the pinballs and dump them into the trough.
		Actually, we dump them onto the playfield and force them to drain,
		so that they proceed through the outhole and TZ's far left trough.
		This lets us install more balls than the trough can hold, as if
		you just dropped them onto the playfield. */
	for (i=0; i < sim_installed_balls; i++)
	{
		the_ball[i].node = NULL;
		strcpy (the_ball[i].name, "Ball X");
		the_ball[i].name[5] = i + '0';
		the_ball[i].index = i;

		node_insert (&open_node, &the_ball[i]);
		node_kick (&open_node);
	}
#endif

#ifdef MACHINE_TZ
	/* Fixup node graph for Twilight Zone */

	/* Insert the far left trough switch between the outhole
	and the trough */
	node_join (&outhole_node, &far_left_trough_node, 100);
	far_left_trough_node.name = "Far Left Trough";
	far_left_trough_node.type = &switch_type_node;
	far_left_trough_node.index = SW_FAR_LEFT_TROUGH;
	far_left_trough_node.unlocked = 1;
	far_left_trough_node.size = 1;
	node_join (&far_left_trough_node, &trough_node, 50);

	/* Create the autofire node. */
#endif /* MACHINE_TZ */
}

