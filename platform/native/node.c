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

/* A path denotes a sequence of 'places' where a pinball may be located.
	Each ball is associated with a node indicating where it is currently.
	Normally, balls can only move according to the rules of the node graph.
	*/

extern device_properties_t device_properties_table[];
extern int linux_installed_balls;


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
	return ball;
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

	/* TODO : add small delay */
	node_insert (dst, ball);
}


void node_kick (struct ball_node *node)
{
	node_move (node->next, node);
}


void device_node_kick (unsigned int devno)
{
	node_kick (&device_nodes[devno]);
}


/* Initialize the node graph for this machine. */
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
			device_nodes[i].next = &shooter_node;
		else
#endif
			device_nodes[i].next = &open_node;
	}

	/* Create a node for the outhole, which leads to the trough */
#ifdef MACHINE_OUTHOLE_SWITCH
	outhole_node.name = "Outhole";
	outhole_node.type = &switch_type_node;
	outhole_node.index = MACHINE_OUTHOLE_SWITCH;
	outhole_node.next = &trough_node;
	outhole_node.size = 1;
#endif

	/* Create a node for the shooter, which leads to the playfield */
#ifdef MACHINE_SHOOTER_SWITCH
	shooter_node.name = "Shooter";
	shooter_node.type = &switch_type_node;
	shooter_node.index = MACHINE_SHOOTER_SWITCH;
	shooter_node.next = &open_node;
	shooter_node.size = MAX_BALLS_PER_NODE;
#endif

	/* Initialize the open playfield node, which feeds into the trough
	or outhole */
	open_node.name = "Playfield";
	open_node.type = &open_type_node;
#ifdef MACHINE_OUTHOLE_SWITCH
	open_node.next = &outhole_node;
#elif defined(DEVNO_TROUGH)
	open_node.next = &trough_node;
#endif
	open_node.size = MAX_BALLS_PER_NODE;

#ifdef DEVNO_TROUGH
	/* Create the pinballs and dump them into the trough */
	for (i=0; i < linux_installed_balls; i++)
	{
		the_ball[i].node = NULL;
		strcpy (the_ball[i].name, "Ball X");
		the_ball[i].name[5] = i + '0';
		the_ball[i].index = i;
		node_insert (&trough_node, &the_ball[i]);
	}
#endif
}

