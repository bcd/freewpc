/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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

/* System includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* These definitions are required in order for the
 * target includes to parse correctly. */
#define MAX_LAMPS 64
typedef unsigned char U8, task_ticks_t;

/* Target includes */
#include <sys/lamp.h>
#include <mach/lamp.h>


enum lamp_direction {
	ASCENDING=1,
	DESCENDING=-1
};

enum lamp_color {
	ORANGE,
	YELLOW,
	GREEN,
	RED,
	WHITE,
	REDORANGE,
};


const char *color_names[] = {
	"orange", "yellow", "green", "red", "white", "redorange",
};


struct pf_config
{
	int min_x, min_y, max_x, max_y;
	int width, height;
};


struct lamp_config
{
	int id;
	int x;
	int y;
	int color;

	int criterion;
};

struct lamp_config lamp_info[] =
{
	{ .id = LM_PANEL_CAMERA, .color = ORANGE, },
	{ .id = LM_PANEL_HH, .color = ORANGE, },
	{ .id = LM_PANEL_CLOCK_CHAOS, .color = ORANGE, },
	{ .id = LM_PANEL_SUPER_SKILL, .color = ORANGE, },
	{ .id = LM_PANEL_FAST_LOCK, .color = ORANGE, },
	{ .id = LM_PANEL_GUMBALL, .color = ORANGE, },
	{ .id = LM_PANEL_TSM, .color = ORANGE, },
	{ .id = LM_PANEL_EB, .color = ORANGE, },
	{ .id = LM_LOCK2, .color = REDORANGE, },
	{ .id = LM_PANEL_GREED, .color = ORANGE, },
	{ .id = LM_PANEL_10M, .color = ORANGE, },
	{ .id = LM_PANEL_BATTLE_POWER, .color = ORANGE, },
	{ .id = LM_PANEL_SPIRAL, .color = ORANGE, },
	{ .id = LM_PANEL_CLOCK_MIL, .color = ORANGE, },
	{ .id = LM_PANEL_SUPER_SLOT, .color = ORANGE, },
	{ .id = LM_BALL, .color = REDORANGE, },
	{ .id = LM_LEFT_OUTLANE, .color = RED, },
	{ .id = LM_LOCK1, .color = REDORANGE, },
	{ .id = LM_LEFT_INLANE1, .color = WHITE, },
	{ .id = LM_PANEL_LITZ, .color = WHITE, },
	{ .id = LM_LEFT_INLANE2, .color = WHITE, },
	{ .id = LM_GUM, .color = REDORANGE, },
	{ .id = LM_LL_5M, .color = YELLOW, },
	{ .id = LM_DEAD_END, .color = WHITE, },
	{ .id = LM_SPIRAL_2M, .color = YELLOW, },
	{ .id = LM_SPIRAL_3M, .color = ORANGE, },
	{ .id = LM_SPIRAL_4M, .color = YELLOW, },
	{ .id = LM_SPIRAL_5M, .color = ORANGE, },
	{ .id = LM_SPIRAL_10M, .color = YELLOW, },
	{ .id = LM_SPIRAL_EB, .color = RED, },
	{ .id = LM_SHOOT_AGAIN, .color = RED, },
	{ .id = LM_RIGHT_INLANE, .color = WHITE, },
	{ .id = LM_BONUS_X, .color = WHITE, },
	{ .id = LM_MULTIBALL, .color = WHITE, },
	{ .id = LM_SUPER_SKILL, .color = ORANGE, },
	{ .id = LM_LEFT_POWERBALL, .color = WHITE, },
	{ .id = LM_CAMERA, .color = WHITE, },
	{ .id = LM_RAMP_BATTLE, .color = ORANGE },
	{ .id = LM_LOCK_EB, .color = RED, },
	{ .id = LM_LOCK_ARROW, .color = ORANGE },
	{ .id = LM_LEFT_JET, .color = ORANGE },
	{ .id = LM_LOWER_JET, .color = YELLOW },
	{ .id = LM_RIGHT_JET, .color = RED, },
	{ .id = LM_ML_5M, .color = YELLOW, },
	{ .id = LM_UL_5M, .color = YELLOW, },
	{ .id = LM_RIGHT_OUTLANE, .color = RED, },
	{ .id = LM_RIGHT_POWERBALL, .color = WHITE, },
	{ .id = LM_GUMBALL_LANE, .color = RED, },
	{ .id = LM_LR_5M, .color = YELLOW, },
	{ .id = LM_MR1_5M, .color = YELLOW, },
	{ .id = LM_MR2_5M, .color = YELLOW, },
	{ .id = LM_POWER_PAYOFF, .color = WHITE, },
	{ .id = LM_UR_5M, .color = YELLOW, },
	{ .id = LM_MPF_500K, .color = YELLOW, },
	{ .id = LM_MPF_750K, .color = ORANGE, },
	{ .id = LM_MPF_1M, .color = RED, },
	{ .id = LM_LEFT_SPIRAL, .color = YELLOW, },
	{ .id = LM_CLOCK_MILLIONS, .color = WHITE, },
	{ .id = LM_PIANO_PANEL, .color = YELLOW, },
	{ .id = LM_PIANO_JACKPOT, .color = RED, },
	{ .id = LM_SLOT_MACHINE, .color = YELLOW, },
	{ .id = LM_RIGHT_SPIRAL, .color = ORANGE, },
};

static int cmp_generic (
	struct lamp_config *a, 
	struct lamp_config *b,
	int (*criterion) (struct lamp_config *a),
	int ascending_flag)
{
	a->criterion = criterion(a);
	b->criterion = criterion(b);

	if ( a->criterion < b->criterion) return -ascending_flag;
	else if (a->criterion > b->criterion) return ascending_flag;
	else return 0;
}


static int cmpelem_y (struct lamp_config *a) { return a->y; }
static int cmpelem_x (struct lamp_config *a) { return a->x; }


static int cmp_ascending_y (struct lamp_config *a, struct lamp_config *b)
{
	return cmp_generic (a, b, cmpelem_y, ASCENDING);
}

static int cmp_descending_y (struct lamp_config *a, struct lamp_config *b)
{
	return cmp_generic (a, b, cmpelem_y, DESCENDING);
}

static int cmp_ascending_x (struct lamp_config *a, struct lamp_config *b)
{
	return cmp_generic (a, b, cmpelem_x, ASCENDING);
}

static int cmp_descending_x (struct lamp_config *a, struct lamp_config *b)
{
	return cmp_generic (a, b, cmpelem_x, DESCENDING);
}

typedef int (*CMP) (const void *, const void *);


struct order_types
{
	const char *name;
	CMP cmp;
} order_data[] =
{
	{ "bottom-to-top", cmp_ascending_y },
	{ "top-to-bottom", cmp_descending_y },
	{ "left-to-right", cmp_ascending_x },
	{ "right-to-left", cmp_descending_x },
	{ "circle-in", NULL },
	{ "circle-out", NULL },
	{ "wiper-left", NULL },
	{ "wiper-right", NULL },
};



CMP
lookup_order_type (const char *name)
{
	int i;
	for (i=0; i < sizeof (order_data) / sizeof (struct order_types); i++)
		if (!strcmp (name, order_data[i].name))
			return order_data[i].cmp;
	return NULL;
}


void
compute_pf_config (int n_lamps, struct pf_config *pf)
{
	int i;

	pf->min_x = 0;
	pf->min_y = 0;
	pf->max_x = 1000;
	pf->max_y = 1000;

	for (i=0; i < n_lamps; i++)
	{
	}

	pf->width = pf->max_x - pf->min_x;
	pf->height = pf->max_y - pf->min_y;
}


int
prune_color (int n_lamps, const char *name)
{
	enum lamp_color color;
	int i;

	for (i=0; i < sizeof (color_names) / sizeof (char *); i++)
		if (!strcmp (color_names[i], name))
			color = i;

	/* Sort lamps so that lamps with the desired color appears
	 * before all others. */

	/* Return the number of lamps that match, effectively discarding
	 * all others. */
	return n_lamps;
}


void
write_lamps (int n_lamps, int speed)
{
	int i;
	double speed_diff;
	double buffered_delay = 0.0;

	/* Compute the difference between the first and last lamps
	in _units_ */
	int unit_duration = lamp_info[n_lamps-1].criterion - 
		lamp_info[0].criterion;

	/* Compute the speed differential */

	for (i=0 ; i < n_lamps; i++)
	{
		/* Output the next lamp in sequence */
		printf ("%d\n", lamp_info[i].id);

		/* Output a delay opcode for the time difference
		between this lamp and the next one */
		buffered_delay += speed_diff * 
			(lamp_info[i+1].criterion - lamp_info[i].criterion);
		if (buffered_delay >= 1.0)
		{
			int whole_delay = (int)buffered_delay;
			buffered_delay -= (whole_delay * 1.0);
			printf ("DELAY %d\n", whole_delay);
		}
	}
}


int
main (int argc, char *argv[])
{
	int n_lamps = sizeof (lamp_info) / sizeof (struct lamp_config);
	int (*cmp) (const void *, const void *) = cmp_descending_y;
	int speed = 1000;
	int argn = 1;
	struct pf_config playfield_config;

	while (argn < argc)
	{
		switch (argv[argn][0])
		{
			case '-':
				switch (argv[argn][1])
				{
					case 't':
						cmp = lookup_order_type (argv[++argn]);
						break;

					case 's':
						speed = strtoul (argv[++argn], NULL, 0);
						break;

					case 'c':
						/* Prune the lamps by color */
						n_lamps = prune_color (n_lamps, argv[++argn]);
						break;

					case 'h':
						printf ("lamporder [-t <order>] [-s <speed>] [-c]\n");
						printf ("-t   Specify the order to output the lamps\n");
						printf ("-s   Specify the output speed\n");
						printf ("-c   Only output certain color lamps\n");
						exit (0);
				}
				break;

			default:
				break;
		}
		argn++;
	}

	compute_pf_config (n_lamps, &playfield_config);

	qsort (lamp_info, 
		n_lamps,
		sizeof (struct lamp_config),
		cmp );

	write_lamps (n_lamps, speed);
}

