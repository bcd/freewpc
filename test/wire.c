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

#define DEF_WIRE(main, stripe) ((main << 4) | stripe)
#define WIRE_MAIN(wire)  (wire >> 4)
#define WIRE_STRIPE(wire) (wire & 0x0F)
#define SOLID_WIRE(color)   DEF_WIRE(color, SOLID)

typedef U8 wire_color_t;
typedef U8 jack_t;
typedef U8 pin_t;
typedef U8 xistor_t;

typedef struct {
	jack_t jack;
	pin_t pin;
	wire_color_t color;
} wire_config_t;

typedef struct {
	wire_config_t *gate;
	wire_config_t *power;
	xistor_t xistor;
	U8 voltage;
} driver_config_t;

const char *wire_colors[] = {
	"WHT", "ORN", "GRN", "BRN", "YEL", "BLU", "BLK", "RED", "VIO", "GRY"
};

/* All of the following taken from Twilight Zone */

wire_config_t wire_pf_switch_rows[] = {
	{ 0, 0, DEF_WIRE(WHITE, BROWN) },
	{ 0, 0, DEF_WIRE(WHITE, RED) },
	{ 0, 0, DEF_WIRE(WHITE, ORANGE) },
	{ 0, 0, DEF_WIRE(WHITE, YELLOW) },
	{ 0, 0, DEF_WIRE(WHITE, GREEN) },
	{ 0, 0, DEF_WIRE(WHITE, BLUE) },
	{ 0, 0, DEF_WIRE(WHITE, VIOLET) },
	{ 0, 0, DEF_WIRE(WHITE, GREY) },
};

wire_config_t wire_flipper_switch_rows[] = {
	{ 0, 0, DEF_WIRE(BLACK, GREEN) },
	{ 0, 0, DEF_WIRE(BLUE, VIOLET) },
	{ 0, 0, DEF_WIRE(BLACK, BLUE) },
	{ 0, 0, DEF_WIRE(BLUE, GREY) },
	{ 0, 0, DEF_WIRE(BLACK, VIOLET) },
	{ 0, 0, DEF_WIRE(BLACK, YELLOW) },
	{ 0, 0, DEF_WIRE(BLACK, GREY) },
	{ 0, 0, DEF_WIRE(BLACK, BLUE) },
};

wire_config_t wire_direct_switch_rows[] = {
	{ 0, 0, DEF_WIRE(ORANGE, BROWN) },
	{ 0, 0, DEF_WIRE(ORANGE, RED) },
	{ 0, 0, DEF_WIRE(ORANGE, BLACK) },
	{ 0, 0, DEF_WIRE(ORANGE, YELLOW) },
	{ 0, 0, DEF_WIRE(ORANGE, GREEN) },
	{ 0, 0, DEF_WIRE(ORANGE, BLUE) },
	{ 0, 0, DEF_WIRE(ORANGE, VIOLET) },
	{ 0, 0, DEF_WIRE(ORANGE, GREY) },
};

wire_config_t wire_switch_cols[] = {
	{ 0, 0, SOLID_WIRE(BLACK) },
	{ 0, 0, DEF_WIRE(GREEN, BROWN) },
	{ 0, 0, DEF_WIRE(GREEN, RED) },
	{ 0, 0, DEF_WIRE(GREEN, ORANGE) },
	{ 0, 0, DEF_WIRE(GREEN, YELLOW) },
	{ 0, 0, DEF_WIRE(GREEN, BLACK) },
	{ 0, 0, DEF_WIRE(GREEN, BLUE) },
	{ 0, 0, DEF_WIRE(GREEN, VIOLET) },
	{ 0, 0, DEF_WIRE(GREEN, GREY) },
	{ 0, 0, SOLID_WIRE(ORANGE) },
};

wire_config_t wire_lamp_rows[] = {
	{ 134, 1, DEF_WIRE(RED, BROWN) },
	{ 134, 2, DEF_WIRE(RED, BLACK) },
	{ 134, 4, DEF_WIRE(RED, ORANGE) },
	{ 134, 5, DEF_WIRE(RED, YELLOW) },
	{ 134, 6, DEF_WIRE(RED, GREEN) },
	{ 134, 7, DEF_WIRE(RED, BLUE) },
	{ 134, 8, DEF_WIRE(RED, VIOLET) },
	{ 134, 9, DEF_WIRE(RED, GREY) },
};

wire_config_t wire_lamp_cols[] = {
	{ 137, 1, DEF_WIRE(YELLOW, BROWN) },
	{ 137, 2, DEF_WIRE(YELLOW, RED) },
	{ 137, 3, DEF_WIRE(YELLOW, ORANGE) },
	{ 137, 4, DEF_WIRE(YELLOW, BLACK) },
	{ 137, 5, DEF_WIRE(YELLOW, GREEN) },
	{ 137, 6, DEF_WIRE(YELLOW, BLUE) },
	{ 137, 7, DEF_WIRE(YELLOW, VIOLET) },
	{ 137, 9, DEF_WIRE(YELLOW, GREY) },
};

wire_config_t wire_sol_power[] = {
	{ 0, 0, DEF_WIRE(VIOLET, YELLOW), 50 },
	{ 0, 0, DEF_WIRE(VIOLET, ORANGE), 50 },
	{ 0, 0, DEF_WIRE(VIOLET, GREEN), 20 },
	{ 0, 0, DEF_WIRE(VIOLET, GREEN), 20 },
};

wire_config_t wire_sol_drives[] = {
	{ 0, 0, DEF_WIRE(VIOLET, BROWN) },
	{ 0, 0, DEF_WIRE(VIOLET, RED) },
	{ 0, 0, DEF_WIRE(VIOLET, ORANGE) },
	{ 0, 0, DEF_WIRE(VIOLET, YELLOW) },
	{ 0, 0, DEF_WIRE(VIOLET, GREEN) },
	{ 0, 0, DEF_WIRE(VIOLET, BLUE) },
	{ 0, 0, DEF_WIRE(VIOLET, BLACK) },
	{ 0, 0, DEF_WIRE(VIOLET, GREY) },

	{ 0, 0, DEF_WIRE(BROWN, BLACK) },
	{ 0, 0, DEF_WIRE(BROWN, RED) },
	{ 0, 0, DEF_WIRE(BROWN, ORANGE) },
	{ 0, 0, DEF_WIRE(BROWN, YELLOW) },
	{ 0, 0, DEF_WIRE(BROWN, GREEN) },
	{ 0, 0, DEF_WIRE(BROWN, BLUE) },
	{ 0, 0, DEF_WIRE(BROWN, VIOLET) },
	{ 0, 0, DEF_WIRE(BROWN, GREY) },

	{ 0, 0, DEF_WIRE(BLACK, BROWN) },
	{ 0, 0, DEF_WIRE(BLACK, RED) },
	{ 0, 0, DEF_WIRE(BLACK, ORANGE) },
	{ 0, 0, DEF_WIRE(BLACK, YELLOW) },
	{ 0, 0, DEF_WIRE(BLUE, GREEN) },
	{ 0, 0, DEF_WIRE(BLUE, BLACK) },
	{ 0, 0, DEF_WIRE(BLUE, VIOLET) },
	{ 0, 0, DEF_WIRE(BLUE, GREY) },

	{ 0, 0, DEF_WIRE(BLUE, BROWN) },
	{ 0, 0, DEF_WIRE(BLUE, RED) },
	{ 0, 0, DEF_WIRE(BLUE, ORANGE) },
	{ 0, 0, DEF_WIRE(BLUE, YELLOW) },
};

wire_config_t wire_gi[] = {
	{ 0, 0, DEF_WIRE(WHITE, BROWN) },
	{ 0, 0, DEF_WIRE(WHITE, ORANGE) },
	{ 0, 0, DEF_WIRE(WHITE, YELLOW) },
	{ 0, 0, DEF_WIRE(WHITE, GREEN) },
	{ 0, 0, DEF_WIRE(WHITE, VIOLET) },
};

wire_config_t wire_fliptronic_sources[] = {
	{ 0, 0, DEF_WIRE(RED, GREEN) },
	{ 0, 0, DEF_WIRE(RED, GREEN) },
	{ 0, 0, DEF_WIRE(RED, BLUE) },
	{ 0, 0, DEF_WIRE(RED, BLUE) },
	{ 0, 0, DEF_WIRE(RED, VIOLET) },
	{ 0, 0, DEF_WIRE(RED, VIOLET) },
	{ 0, 0, DEF_WIRE(RED, GREY) },
	{ 0, 0, DEF_WIRE(RED, GREY) },
};

wire_config_t wire_fliptronic_drives[] = {
	{ 0, 0, DEF_WIRE(BLUE, VIOLET) },
	{ 0, 0, DEF_WIRE(ORANGE, GREEN) },
	{ 0, 0, DEF_WIRE(BLUE, GREY) },
	{ 0, 0, DEF_WIRE(ORANGE, BLUE) },
	{ 0, 0, DEF_WIRE(BLACK, YELLOW) },
	{ 0, 0, DEF_WIRE(ORANGE, VIOLET) },
	{ 0, 0, DEF_WIRE(BLACK, BLUE) },
	{ 0, 0, DEF_WIRE(ORANGE, GREY) },
};


void render_wire_color (wire_config_t *wire)
{
	U8 main = WIRE_MAIN (wire->color);
	U8 stripe = WIRE_STRIPE (wire->color);
	if (main == stripe)
	{
		sprintf (wire_colors[main]);
	}
	else
	{
		sprintf ("%s-%s", wire_colors[main], wire_colors[stripe]);
	}
}


void render_wire_pin (wire_config_t *wire)
{
	sprintf ("J%d-%d", wire->jack, wire->pin);
}


wire_config_t *wire_get_sw_column (const switchnum_t sw)
{
}

wire_config_t *wire_get_sw_row (const switchnum_t sw)
{
}

wire_config_t *wire_get_lamp_column (const lampnum_t lamp)
{
}

wire_config_t *wire_get_lamp_row (const lampnum_t lamp)
{
}

