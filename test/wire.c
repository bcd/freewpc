/*
 * Copyright 2009 by Brian Dominy <brian@oddchange.com>
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

/**
 * Integer IDs for the possible wire colors.
 */
enum wire_color {
	WHITE,
	ORANGE,
	GREEN,
	BROWN,
	YELLOW,
	BLUE,
	BLACK,
	RED,
	VIOLET,
	GREY,
	SOLID,
};

#define DEF_WIRE(solid, stripe) ((solid << 4) | stripe)
#define WIRE_SOLID(wire)  (wire >> 4)
#define WIRE_STRIPE(wire) (wire & 0x0F)
#define SOLID_WIRE(color)   DEF_WIRE(color, SOLID)
#define Q(n) n

typedef U8 wire_color_t;
typedef U16 jack_t;
typedef U8 pin_t;
typedef U8 xistor_t;

typedef struct {
	jack_t jack;
	pin_t pin;
	wire_color_t color;
	xistor_t xistor;
} wire_config_t;

typedef struct {
	wire_config_t *gate;
	wire_config_t *power;
} driver_config_t;

const char *wire_colors[] = {
	"WHT", "ORN", "GRN", "BRN", "YEL", "BLU", "BLK", "RED", "VIO", "GRY"
};

/* All of the following taken from Twilight Zone */

wire_config_t wire_pf_switch_rows[] = {
	{ 209, 1, DEF_WIRE(WHITE, BROWN) },
	{ 209, 2, DEF_WIRE(WHITE, RED) },
	{ 209, 3, DEF_WIRE(WHITE, ORANGE) },
	{ 209, 4, DEF_WIRE(WHITE, YELLOW) },
	{ 209, 5, DEF_WIRE(WHITE, GREEN) },
	{ 209, 7, DEF_WIRE(WHITE, BLUE) },
	{ 209, 8, DEF_WIRE(WHITE, VIOLET) },
	{ 209, 9, DEF_WIRE(WHITE, GREY) },
};

/* TODO : Not sure about all of the colors in this one ... */
wire_config_t wire_flipper_switch_rows[] = {
	{ 906, 1, DEF_WIRE(BLACK, GREEN) },
	{ 905, 1, DEF_WIRE(BLUE, VIOLET) },
	{ 906, 3, DEF_WIRE(BLACK, BLUE) },
	{ 905, 2, DEF_WIRE(BLUE, GREY) },
	{ 906, 4, DEF_WIRE(BLACK, VIOLET) },
	{ 905, 3, DEF_WIRE(BLACK, YELLOW) },
	{ 906, 5, DEF_WIRE(BLACK, GREY) },
	{ 905, 5, DEF_WIRE(BLACK, BLUE) },
};

wire_config_t wire_direct_switch_rows[] = {
	{ 205, 1, DEF_WIRE(ORANGE, BROWN) },
	{ 205, 2, DEF_WIRE(ORANGE, RED) },
	{ 205, 3, DEF_WIRE(ORANGE, BLACK) },
	{ 205, 4, DEF_WIRE(ORANGE, YELLOW) },
	{ 205, 6, DEF_WIRE(ORANGE, GREEN) },
	{ 205, 7, DEF_WIRE(ORANGE, BLUE) },
	{ 205, 8, DEF_WIRE(ORANGE, VIOLET) },
	{ 205, 9, DEF_WIRE(ORANGE, GREY) },
};

wire_config_t wire_switch_cols[] = {
	{ 0, 0, SOLID_WIRE(BLACK) },
	{ 207, 1, DEF_WIRE(GREEN, BROWN) },
	{ 207, 2, DEF_WIRE(GREEN, RED) },
	{ 207, 3, DEF_WIRE(GREEN, ORANGE) },
	{ 207, 4, DEF_WIRE(GREEN, YELLOW) },
	{ 207, 5, DEF_WIRE(GREEN, BLACK) },
	{ 207, 6, DEF_WIRE(GREEN, BLUE) },
	{ 207, 7, DEF_WIRE(GREEN, VIOLET) },
	{ 207, 9, DEF_WIRE(GREEN, GREY) },
	{ 0, 0, SOLID_WIRE(ORANGE) },
};

wire_config_t wire_lamp_rows[] = {
	{ 134, 1, DEF_WIRE(RED, BROWN), Q(90) },
	{ 134, 2, DEF_WIRE(RED, BLACK), Q(89) },
	{ 134, 4, DEF_WIRE(RED, ORANGE), Q(88) },
	{ 134, 5, DEF_WIRE(RED, YELLOW), Q(87) },
	{ 134, 6, DEF_WIRE(RED, GREEN), Q(86) },
	{ 134, 7, DEF_WIRE(RED, BLUE), Q(85) },
	{ 134, 8, DEF_WIRE(RED, VIOLET), Q(84) },
	{ 134, 9, DEF_WIRE(RED, GREY), Q(83) },
};

wire_config_t wire_lamp_cols[] = {
	{ 137, 1, DEF_WIRE(YELLOW, BROWN), Q(98), },
	{ 137, 2, DEF_WIRE(YELLOW, RED), Q(97), },
	{ 137, 3, DEF_WIRE(YELLOW, ORANGE), Q(96), },
	{ 137, 4, DEF_WIRE(YELLOW, BLACK), Q(95), },
	{ 137, 5, DEF_WIRE(YELLOW, GREEN), Q(94), },
	{ 137, 6, DEF_WIRE(YELLOW, BLUE), Q(93), },
	{ 137, 7, DEF_WIRE(YELLOW, VIOLET), Q(92), },
	{ 137, 9, DEF_WIRE(YELLOW, GREY), Q(91), },
};

wire_config_t wire_sol_power[] = {
	{ 0, 0, DEF_WIRE(VIOLET, YELLOW), 50 },
	{ 0, 0, DEF_WIRE(VIOLET, ORANGE), 50 },
	{ 0, 0, DEF_WIRE(VIOLET, GREEN), 20 },
	{ 0, 0, DEF_WIRE(VIOLET, GREEN), 20 },
};

wire_config_t wire_sol_drives[] = {
	{ 130, 1, DEF_WIRE(VIOLET, BROWN), Q(82) },
	{ 130, 2, DEF_WIRE(VIOLET, RED), Q(80) },
	{ 130, 4, DEF_WIRE(VIOLET, ORANGE), Q(78) },
	{ 130, 5, DEF_WIRE(VIOLET, YELLOW), Q(76) },
	{ 130, 6, DEF_WIRE(VIOLET, GREEN), Q(74) },
	{ 130, 7, DEF_WIRE(VIOLET, BLUE), Q(72) },
	{ 130, 8, DEF_WIRE(VIOLET, BLACK) },
	{ 130, 9, DEF_WIRE(VIOLET, GREY) },

	{ 127, 1, DEF_WIRE(BROWN, BLACK) },
	{ 127, 3, DEF_WIRE(BROWN, RED) },
	{ 127, 4, DEF_WIRE(BROWN, ORANGE) },
	{ 127, 5, DEF_WIRE(BROWN, YELLOW) },
	{ 127, 6, DEF_WIRE(BROWN, GREEN) },
	{ 127, 7, DEF_WIRE(BROWN, BLUE) },
	{ 127, 8, DEF_WIRE(BROWN, VIOLET) },
	{ 127, 9, DEF_WIRE(BROWN, GREY) },

	{ 126, 1, DEF_WIRE(BLACK, BROWN) },
	{ 126, 2, DEF_WIRE(BLACK, RED) },
	{ 126, 3, DEF_WIRE(BLACK, ORANGE) },
	{ 126, 4, DEF_WIRE(BLACK, YELLOW) },
	{ 126, 5, DEF_WIRE(BLUE, GREEN) },
	{ 126, 6, DEF_WIRE(BLUE, BLACK) },
	{ 126, 7, DEF_WIRE(BLUE, VIOLET) },
	{ 126, 8, DEF_WIRE(BLUE, GREY) },

	{ 122, 1, DEF_WIRE(BLUE, BROWN) },
	{ 122, 2, DEF_WIRE(BLUE, RED) },
	{ 122, 3, DEF_WIRE(BLUE, ORANGE) },
	{ 122, 4, DEF_WIRE(BLUE, YELLOW) },
};

wire_config_t wire_gi[] = {
	{ 121, 7, DEF_WIRE(WHITE, BROWN) },
	{ 121, 8, DEF_WIRE(WHITE, ORANGE) },
	{ 120, 9, DEF_WIRE(WHITE, YELLOW) },
	{ 120, 10, DEF_WIRE(WHITE, GREEN) },
	{ 119, 1, DEF_WIRE(WHITE, VIOLET) },
};

wire_config_t wire_fliptronic_sources[] = {
	{ 907, 1, DEF_WIRE(RED, GREEN) },
	{ 907, 2, DEF_WIRE(RED, GREEN) },
	{ 907, 4, DEF_WIRE(RED, BLUE) },
	{ 907, 5, DEF_WIRE(RED, BLUE) },
	{ 907, 6, DEF_WIRE(RED, VIOLET) },
	{ 907, 7, DEF_WIRE(RED, VIOLET) },
	{ 907, 8, DEF_WIRE(RED, GREY) },
	{ 907, 9, DEF_WIRE(RED, GREY) },
};

/* TODO - the colors are not right here */
wire_config_t wire_fliptronic_drives[] = {
	{ 902, 13, DEF_WIRE(YELLOW, GREEN) },
	{ 902, 11, DEF_WIRE(ORANGE, GREEN) },
	{ 902, 9, DEF_WIRE(BLUE, GREY) },
	{ 902, 7, DEF_WIRE(ORANGE, BLUE) },
	{ 902, 3, DEF_WIRE(BLACK, YELLOW) },
	{ 902, 4, DEF_WIRE(ORANGE, VIOLET) },
	{ 0, 0, DEF_WIRE(BLACK, BLUE) },
	{ 0, 0, DEF_WIRE(ORANGE, GREY) },
};


void render_wire_color (wire_config_t *wire)
{
	U8 solid = WIRE_SOLID (wire->color);
	U8 stripe = WIRE_STRIPE (wire->color);
	if (solid == stripe)
	{
		sprintf (wire_colors[solid]);
	}
	else
	{
		sprintf ("%s-%s", wire_colors[solid], wire_colors[stripe]);
	}
}


void render_wire_pin (wire_config_t *wire)
{
	sprintf ("J%d-%d", wire->jack, wire->pin);
}


wire_config_t *wire_get_sw_column (const switchnum_t sw)
{
	return NULL;
}

wire_config_t *wire_get_sw_row (const switchnum_t sw)
{
	return NULL;
}

wire_config_t *wire_get_lamp_column (const lampnum_t lamp)
{
	return NULL;
}

wire_config_t *wire_get_lamp_row (const lampnum_t lamp)
{
	return NULL;
}

