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
 *
 * @TODO integration with test report
 * @TODO initialise ZR1 on startup
 */

#include <freewpc.h>
#include <window.h>
#include <test.h>

enum {
	FIRST_TEST = 0,
	CALIBRATE = FIRST_TEST,
	SHAKE,
	IDLE,
	STOP,
	ENABLE_SOLENOIDS,  // XXX
	DISABLE_SOLENOIDS, // XXX
	LAST_TEST = DISABLE_SOLENOIDS
} zr1_test_command;

char *short_names[] = {
	"CALIBRATE",
	"SHAKE",
	"IDLE",
	"STOP",
	"ENABLE SOL.", // XXX
	"DISABLE SOL." // XXX
};

extern U8 calibration_running;
extern U8 zr1_pos_center;
extern U8 zr1_pos_full_left_opto_off;
extern U8 zr1_pos_full_right_opto_off;
extern U8 zr1_engine_position;

void zr1_test_init (void)
{
	zr1_test_command = CALIBRATE;
}

void draw_test_title(void) {
	font_render_string_center (&font_mono5, 64, 2, "ZR1 ENGINE TEST");
	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 5);
}

void zr1_test_draw (void)
{
	dmd_alloc_low_clean ();

	draw_test_title();

	sprintf ("POS %d", zr1_engine_position);
	font_render_string_left (&font_mono5, 0, 6, sprintf_buffer);

	sprintf ("POWER %s",
		global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED) ? "ON" : "OFF");
	font_render_string_right (&font_mono5, 0, 6, sprintf_buffer);

	sprintf ("LEFT %s %d",
		(switch_poll_logical (SW_ZR_1_FULL_LEFT) ? "X" : "-"), zr1_pos_full_left_opto_off);
	font_render_string_left (&font_mono5, 0, 12, sprintf_buffer);

	sprintf ("%d %s RIGHT",
		zr1_pos_full_right_opto_off, (switch_poll_logical (SW_ZR_1_FULL_RIGHT) ? "X" : "-"));
	font_render_string_right (&font_mono5, 0, 12, sprintf_buffer);

	sprintf ("CENTER %d", zr1_pos_center);
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);

	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 25);

	sprintf(short_names[zr1_test_command]);
	font_render_string_left (&font_mono5, 0, 27, sprintf_buffer);

	dmd_show_low ();
}


void zr1_test_thread (void)
{
	for (;;)
	{

		task_sleep (TIME_100MS);

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

/**
 * Ensures user can exit test menu when calibration still running
 */
void zr1_test_escape (void) {
	if (calibration_running) {
		return;
	}

	window_pop();
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
			if (zr1_is_shaking()) {
				zr1_stop();
				break;
			}
			zr1_shake();
		break;

		case IDLE:
			if (zr1_is_idle()) {
				zr1_stop();
				break;
			}
			zr1_idle();
		break;

		case STOP:
			zr1_stop();
		break;

		// TODO remove when real-machine testing is complete - begin

		case ENABLE_SOLENOIDS:
			zr1_enable_solenoids();
		break;

		case DISABLE_SOLENOIDS:
			zr1_disable_solenoids();
		break;

		// TODO remove when tseting complete - end
	}
}


struct window_ops corvette_zr1_test_window = {
	DEFAULT_WINDOW,
	.init = zr1_test_init,
	.draw = zr1_test_draw,
	.up = zr1_test_up,
	.down = zr1_test_down,
	.enter = zr1_test_enter,
	.escape = zr1_test_escape,
	.thread = zr1_test_thread,
};


struct menu corvette_zr1_test_item = {
	.name = "ZR1 ENGINE TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &corvette_zr1_test_window, NULL } },
};

