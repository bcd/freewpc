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
#include <corvette/zr1.h>

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

// Calibration errors
enum {
	ERROR_CHECK_F111 = 0,
	ERROR_CHECK_FULL_LEFT_OPTO,
	ERROR_CHECK_FULL_RIGHT_OPTO,

};

char *errors[] = {
	"CHECK F111",
	"CHECK ZR1 LEFT OPTO",
	"CHECK ZR1 RIGHT OPTO"
};


extern U8 zr1_pos_center;
extern U8 zr1_pos_full_left_opto_on;
extern U8 zr1_pos_full_left_opto_off;
extern U8 zr1_pos_full_right_opto_on;
extern U8 zr1_pos_full_right_opto_off;

U8 foundPos;
U8 position;
U8 displaying_message;
char *calibration_error_message;

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
	// TODO wait a bit for solenoids to react to new position value
	writeb (ZR1_ENGINE_CONTROL, 1);
	global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
}

void draw_test_title( void ) {

	font_render_string_center (&font_mono5, 64, 2, "ZR1 TEST");
	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 5);

}

void display_calibration_error() {
	displaying_message = TRUE;

	dmd_alloc_low_clean ();

	draw_test_title();


	sprintf ("CALIBRATION FAILED");
	font_render_string_center (&font_mono5, 64, 8, sprintf_buffer);

	font_render_string_center (&font_mono5, 64, 14, calibration_error_message);

	dmd_show_low ();

	task_sleep (TIME_2S);

	displaying_message = FALSE;
}

void zr1_calibration_failed(U8 code) {
	// TODO appropriate display message
	calibration_error_message =  errors[code];
	display_calibration_error();
}

void zr1_calculate_center_pos( void ) {
	zr1_pos_center = (zr1_pos_full_right_opto_off + zr1_pos_full_left_opto_off ) / 2;
}


void zr1_calibrate(void) {

	// TODO write a real calibration routine as per the TODO file for corvette

	zr1_reset();

	zr1_disable_solenoids();

	zr1_enable_solenoids();
	position = zr1_pos_center;

	// wait till the engine is in the center.
	task_sleep(TIME_1S);

	// check for opto still active
	if (switch_poll_logical (SW_ZR_1_FULL_LEFT) || switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
		// engine not in center
		zr1_calibration_failed(ERROR_CHECK_F111);
	}

	// engine positioned in the center


	// move from center to the left until either the limit is hit or the left opto activates

	for (position = zr1_pos_center; position > ZR_1_ENGINE_LEFT_MIN; position--) {
		writeb (ZR1_ENGINE_POS, position);
		task_sleep (TIME_50MS);
		if (switch_poll_logical (SW_ZR_1_FULL_LEFT)) {
			// the position we're at is where the opto turned on.
			zr1_pos_full_left_opto_on = position;
			break;
		}
	}

	// wait a bit for opto to become active to make sure
	task_sleep(TIME_500MS);

	if (!switch_poll_logical (SW_ZR_1_FULL_LEFT)) {
		// if we reached the min left value and the left opto is not active the opto may be dead
		zr1_calibration_failed(ERROR_CHECK_FULL_LEFT_OPTO);
		return;
	}


	// move from left to right until either the limit is hit or the right opto activates

	foundPos = FALSE;

	for (position = zr1_pos_full_left_opto_on; position < ZR_1_ENGINE_RIGHT_MAX; position++) {
		writeb (ZR1_ENGINE_POS, position);
		task_sleep (TIME_50MS);

		// if we've not already recorded the position at which the left opto turns off do that now
		if (!foundPos && !switch_poll_logical (SW_ZR_1_FULL_LEFT)) {
			zr1_pos_full_left_opto_off = position;
			foundPos = TRUE;
		}

		if (switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
			// the position we're at is where the opto turned on.
			zr1_pos_full_right_opto_on = position;
			break;
		}
	}

	// wait a bit for opto to become active to make sure
	task_sleep(TIME_500MS);

	if (!switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
		// if we reached the max right value and the right opto is not active the opto may be dead
		zr1_calibration_failed(ERROR_CHECK_FULL_RIGHT_OPTO);
		return;
	}

	// move from right to the center

	foundPos = FALSE;

	for (position = zr1_pos_full_right_opto_on; position > zr1_pos_center; position--) {
		writeb (ZR1_ENGINE_POS, position);
		task_sleep (TIME_50MS);

		// if we've not already recorded the position at which the right opto turns off do that now
		if (!foundPos && !switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
			zr1_pos_full_right_opto_off = position;
			foundPos = TRUE;

			// when the right opto turns off we can calculate the center position.
			zr1_calculate_center_pos();
		}
	}

	zr1_disable_solenoids();

	global_flag_on(GLOBAL_FLAG_ZR1_CALIBRATED);
}

void zr1_shake(void) {
	// TODO
}

void zr1_test_init (void)
{
	zr1_test_command = CALIBRATE;
	displaying_message = FALSE;
	calibration_error_message = NULL;
}

void zr1_test_draw (void)
{
	dmd_alloc_low_clean ();

	draw_test_title();

	sprintf ("POS %d", position);
	font_render_string_left (&font_mono5, 0, 6, sprintf_buffer);

	sprintf ("POWER %s",
		global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED) ? "ON" : "OFF");
	font_render_string_right (&font_mono5, 0, 6, sprintf_buffer);

	// TODO verify switches show closed when closed - they're optos.
	sprintf ("LEFT %s %d",
		(switch_poll_logical (SW_ZR_1_FULL_LEFT) ? "X" : "-"), zr1_pos_full_left_opto_off);
	font_render_string_left (&font_mono5, 0, 12, sprintf_buffer);

	sprintf ("%d %s RIGHT",
		zr1_pos_full_right_opto_off, (switch_poll_logical (SW_ZR_1_FULL_RIGHT) ? "X" : "-"));
	font_render_string_right (&font_mono5, 0, 12, sprintf_buffer);

	sprintf ("CENTER %d", zr1_pos_center);
	font_render_string_center (&font_mono5, 64, 20, sprintf_buffer);

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

		if (displaying_message) {
			continue;
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

