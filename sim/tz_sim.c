/*
 * Copyright 2011 by Brian Dominy <brian@oddchange.com>
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

#define SOL_BRIDGE_DIVERT (PINIO_NUM_SOLS+0)

/* Twilight Zone simulation */

/* Define nodes for playfield locations not directly associated with a
single switch or ball device */

struct ball_node left_ramp_exit_node;
struct ball_node left_ramp_exit_to_inlane;
struct ball_node left_ramp_exit_to_shooter;

struct ball_node right_ramp_exit_node;
struct ball_node right_ramp_trap_node;

struct ball_node powerfield_node;
struct ball_node powerfield_lower_exit_node;
struct ball_node powerfield_upper_exit_node;

struct ball_node left_loop_exit_node;
struct ball_node right_loop_exit_node;

struct ball_node gumball_machine_node;

struct ball_node autofire_node;

struct ball_node shooter_diverter_node;


void node_clone (struct ball_node *dst, struct ball_node *src)
{
	dst->name = src->name;
	dst->type = src->type;
	dst->index = src->index;
	dst->unlocked = src->unlocked;
	dst->size = src->size;
}


void locked_switch_node_init (struct ball_node *node, unsigned int swno)
{
	node->name = names_of_switches[swno];
	node->type = &switch_type_node;
	node->index = swno;
	node->unlocked = 0;
	node->size = 1;
}


void open_node_init (struct ball_node *node, const char *name)
{
	node->name = "Gumball";
	node->type = &open_type_node;
	node->size = MAX_BALLS_PER_NODE;
	node->unlocked = 0;
}


void diverting_node_init (struct ball_node *mux_node, unsigned int sol,
	struct ball_node *inactive_node, struct ball_node *active_node)
{
	diverter_coil_init (sol, mux_node);
	mux_node->name = "Diverter";
	mux_node->type = &mux_type_node;
	mux_node->index = 0;
	mux_node->unlocked = 1;
	mux_node->size = 1;
	mux_node->next = mux_node->mux_next[0] = inactive_node;
	mux_node->mux_next[1] = active_node;
	if (!inactive_node->name)
	{
		inactive_node->name = "Not Diverted";
		inactive_node->type = &open_type_node;
		inactive_node->size = MAX_BALLS_PER_NODE;
		inactive_node->unlocked = 1;
	}
	if (!active_node->name)
	{
		active_node->name = "Diverted";
		active_node->type = &open_type_node;
		active_node->size = MAX_BALLS_PER_NODE;
		active_node->unlocked = 1;
	}
}



void kicking_node_init (struct ball_node *node, unsigned int sol)
{
	device_coil_init (sol, node);
	node->name = names_of_drives[sol];
	node->type = &open_type_node;
	node->index = sol;
	node->unlocked = 0;
	node->size = MAX_BALLS_PER_NODE;
	node->next = &open_node;
}


void magnet_node_init (struct ball_node *magnet_node, unsigned int sol,
	struct ball_node *exit_node)
{
	diverting_node_init (magnet_node, sol, exit_node, magnet_node);
	magnet_node->delay = 100;
}


void mach_node_init (void)
{
	/* Fixup node graph for Twilight Zone */
	locked_switch_node_init (&autofire_node, SW_AUTOFIRE1);

	/* Connect nodes where the ball automatically transitions from one
	to the next */
	node_join (&switch_nodes[SW_LEFT_OUTLANE], &drain_node, 1500);
	node_join (&switch_nodes[SW_RIGHT_OUTLANE], &drain_node, 1500);
	node_join (&outhole_node, &switch_nodes[SW_FAR_LEFT_TROUGH], 100);
	node_join (&switch_nodes[SW_FAR_LEFT_TROUGH], &trough_node, 50);
	node_join (&switch_nodes[SW_SLOT_PROXIMITY], &device_nodes[DEVNO_SLOT], 500);
	node_join (&switch_nodes[SW_PIANO], &switch_nodes[SW_SLOT_PROXIMITY], 1500);
	node_join (&switch_nodes[SW_CAMERA], &switch_nodes[SW_SLOT_PROXIMITY], 1000);
	node_join (&switch_nodes[SW_DEAD_END], &switch_nodes[SW_CAMERA], 500);
	node_join (&shooter_node, &device_nodes[DEVNO_SLOT], 600);
	node_join (&switch_nodes[SW_GUMBALL_LANE], &device_nodes[DEVNO_POPPER], 1200);
	node_join (&device_nodes[DEVNO_ROCKET], &switch_nodes[SW_HITCHHIKER], 500);
	node_join (&device_nodes[DEVNO_LOCK], &switch_nodes[SW_LOWER_RIGHT_MAGNET], 300);
	node_join (&device_nodes[DEVNO_POPPER], &switch_nodes[SW_GUMBALL_ENTER], 500);
	node_join (&switch_nodes[SW_GUMBALL_ENTER], &gumball_machine_node, 100);

	/* Create multiplexor nodes, locations where a ball may take alternate paths
	depending on some external criteria. */

	/* 50ms after left ramp exit is seen, go towards the left inlane by default,
	or the shooter diverter if the ramp diverter is active. */
	node_join (&switch_nodes[SW_LEFT_RAMP_EXIT], &left_ramp_exit_node, 50);
	diverting_node_init (&left_ramp_exit_node, SOL_RAMP_DIVERTOR,
		&left_ramp_exit_to_inlane, &left_ramp_exit_to_shooter);
	node_join (&left_ramp_exit_to_inlane, &switch_nodes[SW_LEFT_INLANE_2], 1500);
	node_join (&left_ramp_exit_to_shooter, &shooter_diverter_node, 1200);

	/* When a ball reaches the shooter diverter, decide whether to go to the
	autofire switches or the manual shooter. */
	diverting_node_init (&shooter_diverter_node, SOL_SHOOTER_DIV,
		&switch_nodes[SW_SHOOTER], &autofire_node);

	/* right ramp divertor */
	coil_clone (SOL_RIGHT_RAMP_DIV, SOL_BRIDGE_DIVERT);
	node_join (&switch_nodes[SW_RIGHT_RAMP], &right_ramp_exit_node, 250);
	diverting_node_init (&right_ramp_exit_node, SOL_BRIDGE_DIVERT,
		&right_ramp_trap_node, &powerfield_node);
	kicking_node_init (&right_ramp_trap_node, SOL_RIGHT_RAMP_DIV);

	/* diverting_node_init (&gumball_diverter_node, SOL_GUMBALL_DIV,
		&???, &switch_nodes[SW_GUMBALL_LANE]); */

	open_node_init (&gumball_machine_node, "Gumball Mech.");
	open_node_init (&powerfield_node, "Powerfield");

	/* Mark the Powerball */
	//the_ball[0].flags |= 0x1;
}

