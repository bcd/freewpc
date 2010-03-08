/*
 * Copyright 2010 by Dominic Clifton <me@dominicclifton.name>
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

/* ZR1 Test :
 * - Enable/Disable solenoids.
 * - Calibrate the left right and center positions.
 * - Shake engine.
 */

#include <freewpc.h>
#include <window.h>
#include <test.h>

#define ZR1_ENGINE_POS WPC_EXTBOARD2
#define ZR1_ENGINE_CONTROL WPC_EXTBOARD3

enum {
	FIRST_TEST = 0,
	CALIBRATE = FIRST_TEST,
	SHAKE,
	ENABLE_SOLENOIDS,
	DISABLE_SOLENOIDS,
	LAST_TEST = DISABLE_SOLENOIDS
} zr1_test_command;

char *short_names[] = {
	"CALIBRATE",
	"SHAKE",
	"ENABLE SOL.",
	"DISABLE SOL."
};


extern U8 zr1_pos_center;
extern U8 zr1_pos_full_left_opto_on;
extern U8 zr1_pos_full_left_opto_off;
extern U8 zr1_pos_full_right_opto_on;
extern U8 zr1_pos_full_right_opto_off;

U8 position;

void zr1_calibrate(void) {

	// TODO write a real calibration routine as per the TODO file for corvette

	zr1_enable_solenoids();

	for (position = 64; position < 192; position++) {
		writeb (ZR1_ENGINE_POS, position);
		task_sleep (TIME_50MS);
	}

	for (position = 192; position > 64; position--) {
		writeb (ZR1_ENGINE_POS, position);
		task_sleep (TIME_50MS);
	}

	position = zr1_pos_center; // show the position value

	zr1_disable_solenoids();
}

void zr1_shake(void) {
	// TODO
}

void zr1_enable_solenoids(void) {
	if (global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED)) {
		return; // already on
	}
	writeb (ZR1_ENGINE_POS, zr1_pos_center);
	writeb (ZR1_ENGINE_CONTROL, 0);
	global_flag_on(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
}

void zr1_disable_solenoids(void) {
	if (!global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED)) {
		return; // already off
	}
	writeb (ZR1_ENGINE_POS, zr1_pos_center); // leave it in the middle when we turn it off
	writeb (ZR1_ENGINE_CONTROL, 1);
	global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
}

void zr1_test_init (void)
{
	zr1_test_command = CALIBRATE;
}

void zr1_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 2, "ZR1 TEST");

	sprintf ("POS: %d", position);
	font_render_string_left (&font_mono5, 0, 6, sprintf_buffer);

	sprintf ("POWER: %s",
		global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED) ? "ON" : "OFF");
	font_render_string_right (&font_mono5, 0, 6, sprintf_buffer);

	// TODO verify switches show closed when closed - they're optos.
	sprintf ("LEFT: %s",
		switch_poll_logical (SW_ZR_1_FULL_LEFT) ? "X" : "-");
	font_render_string_left (&font_mono5, 0, 12, sprintf_buffer);

	sprintf ("%s :RIGHT",
		switch_poll_logical (SW_ZR_1_FULL_RIGHT) ? "X" : "-");
	font_render_string_right (&font_mono5, 0, 12, sprintf_buffer);


	sprintf(short_names[zr1_test_command]);
	font_render_string_left (&font_mono5, 0, 24, sprintf_buffer);

	dmd_show_low ();
}


void zr1_test_thread (void)
{
	for (;;)
	{
		switch (zr1_test_command) {
			case CALIBRATE:
				task_sleep (TIME_100MS);
			break;

			default:
				task_sleep (TIME_500MS);
		}

		zr1_test_draw ();
	}
}


void zr1_test_up (void)
{
	if (zr1_test_command < LAST_TEST)
		zr1_test_command++;
}


void zr1_test_down (void)
{
	if (zr1_test_command > FIRST_TEST)
		zr1_test_command--;
}


void zr1_test_enter (void)
{
	sound_send (SND_TEST_ENTER);
	switch (zr1_test_command)
	{
		case CALIBRATE:
			zr1_calibrate();
			break;
		case SHAKE:
			zr1_shake();
			break;
		case ENABLE_SOLENOIDS:
			zr1_enable_solenoids();
			break;
		case DISABLE_SOLENOIDS:
			zr1_disable_solenoids();
			break;
	}
}


struct window_ops corvette_zr1_test_window = {
	DEFAULT_WINDOW,
	.init = zr1_test_init,
	.draw = zr1_test_draw,
	.up = zr1_test_up,
	.down = zr1_test_down,
	.enter = zr1_test_enter,
	.thread = zr1_test_thread,
};


struct menu corvette_zr1_test_item = {
	.name = "ZR1 ENGINE TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &corvette_zr1_test_window, NULL } },
};

