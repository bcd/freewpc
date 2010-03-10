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

#include <freewpc.h>
#include <diag.h>

#define ZR1_ENGINE_POS WPC_EXTBOARD2
#define ZR1_ENGINE_CONTROL WPC_EXTBOARD3

//
// Calibration
//

#define ZR_1_ENGINE_LEFT_MIN 0x01 // FIXME should be 0x00 but compiler bug causes problems with > operator when using "0" or "0x00" in zr1_calibrate()
#define ZR_1_ENGINE_RIGHT_MAX 0xFF
#define ZR_1_ENGINE_CENTER 0x7F

U8 calibration_running; // flag to indicate if calibration process is currently running
U8 foundPos;
U8 zr1_pos_center;
U8 zr1_engine_position; // only valid during calibration and while shaking
U8 zr1_pos_full_left_opto_on;
U8 zr1_pos_full_left_opto_off;
U8 zr1_pos_full_right_opto_on;
U8 zr1_pos_full_right_opto_off;

// errors
enum {
	ERROR_CHECK_F111 = 0,
	ERROR_CHECK_FULL_LEFT_OPTO,
	ERROR_CHECK_FULL_RIGHT_OPTO,

};

char *errors[] = {
	"ZR1 ERROR 1\nCHECK F111\n",
	"CHECK OPTO\nZR1 FULL LEFT\n",
	"CHECK OPTO\nZR1 FULL RIGHT\n"
};

//
// State
//

/** The logical states of the clock driver state machine */
enum mech_zr1_state
{
	/** The engine should not be moving at all */
	ZR1_STOPPED = 0,
	/** The engine should be shaking at the specified speed */
	ZR1_SHAKING,
	/** The engine should be calibrated */
	ZR1_CALIBRATING
};

/* Mode that drives the rtt state machine */
__fastram__ enum mech_zr1_state zr1_state;
U8 zr1_first_enable;
U8 zr1_calibration_attempted;

void zr1_initialise(void) {

	zr1_calibration_attempted = FALSE;
	zr1_first_enable = TRUE;

	zr1_reset();

	// TODO call zr1_disable_solenoids

}

void zr1_reset() {
	zr1_state = ZR1_CALIBRATING;

	global_flag_off(GLOBAL_FLAG_ZR1_WORKING);
	global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);

	// provide some default values, just in case. // TODO remove if possible
	zr1_pos_center = ZR_1_ENGINE_CENTER;
	zr1_pos_full_left_opto_on = ZR_1_ENGINE_LEFT_MIN;
	zr1_pos_full_left_opto_off = ZR_1_ENGINE_LEFT_MIN;
	zr1_pos_full_right_opto_on = ZR_1_ENGINE_RIGHT_MAX;
	zr1_pos_full_right_opto_off = ZR_1_ENGINE_RIGHT_MAX;

	// TODO this should call disable solenoids
}

// should not be used outside of this file
static void zr1_set_position_to_center(void) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	writeb (ZR1_ENGINE_POS, zr1_pos_center);
}

void zr1_stop(void) {
	zr1_state = ZR1_STOPPED;
	zr1_enable_solenoids();
	// TODO wait a bit for solenoids to react to new position value
	zr1_set_position_to_center();
	task_sleep(TIME_500MS);
	zr1_disable_solenoids();
}

U8 zr1_is_shaking(void) {
	return zr1_state == ZR1_SHAKING;
}

U8 zr1_can_shake(void) {
	return (
		feature_config.enable_zr1_engine &&
		global_flag_test(GLOBAL_FLAG_ZR1_WORKING) &&
		(zr1_state == ZR1_SHAKING || zr1_state == ZR1_STOPPED)
	);
}
void zr1_shake(void) {
	if (!zr1_can_shake()) {
		return;
	}

	zr1_state = ZR1_SHAKING;
}

/**
 * Enable the solenoids that drive the ZR1 ball shaker device
 *
 * This can be called regardless of calibration state.
 * If called before calibration the engine is engine position is set to the default center position.
 * Has no effect if solenoid power is already enabled.
 *
 * @See zr1_init()
 */
void zr1_enable_solenoids(void) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	if (global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED)) {
		return; // already on
	}

	if (zr1_first_enable) {
		zr1_set_position_to_center();
		zr1_first_enable = FALSE;
	}
	writeb (ZR1_ENGINE_CONTROL, 0); // disable the DISABLE_A/DISABLE_B lines
	global_flag_on(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
}

/**
 * Disable the solenoids that drive the ZR1 ball shaker device
 */
void zr1_disable_solenoids(void) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	writeb (ZR1_ENGINE_CONTROL, 1); // enable the DISABLE_A/DISABLE_B lines
	global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
}

void zr1_calculate_center_pos(void) {
	zr1_pos_center = (zr1_pos_full_right_opto_off + zr1_pos_full_left_opto_off ) / 2;
}

U8 zr1_can_calibrate(void) {
	if (!feature_config.enable_zr1_engine) {
		return FALSE;
	}

	// can't restart calibrating - no way to stop calibration once started
	return !calibration_running;
}


void zr1_calibration_failed(U8 code) {
	calibration_running = FALSE;
	audit_increment (&feature_audits.zr1_errors);
	diag_post_error (errors[code], PAGE);
}

void zr1_calibration_succeded(void) {
	calibration_running = FALSE;
	zr1_state = ZR1_STOPPED;
	global_flag_on(GLOBAL_FLAG_ZR1_WORKING);
}

void zr1_calibrate(void) {

	if (!zr1_can_calibrate()) {
		return;
	}
	calibration_running = TRUE;

	zr1_calibration_attempted = TRUE;

	// reset state and flags before doing anything
	zr1_reset();

	zr1_disable_solenoids(); // XXX remove when zr1_reset() calls it instead

	zr1_enable_solenoids();
	zr1_engine_position = zr1_pos_center;

	// wait till the engine is in the center.
	task_sleep(TIME_1S);

	// check for opto still active
	if (switch_poll_logical (SW_ZR_1_FULL_LEFT) || switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
		// engine not in center
		zr1_calibration_failed(ERROR_CHECK_F111);
	}

	// engine positioned in the center


	// move from center to the left until either the limit is hit or the left opto activates

	for (; zr1_engine_position > ZR_1_ENGINE_LEFT_MIN; zr1_engine_position--) {
		writeb (ZR1_ENGINE_POS, zr1_engine_position);
		task_sleep (TIME_50MS);
		if (switch_poll_logical (SW_ZR_1_FULL_LEFT)) {
			// the position we're at is where the opto turned on.
			zr1_pos_full_left_opto_on = zr1_engine_position;
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

	for (zr1_engine_position = zr1_pos_full_left_opto_on; zr1_engine_position < ZR_1_ENGINE_RIGHT_MAX; zr1_engine_position++) {
		writeb (ZR1_ENGINE_POS, zr1_engine_position);
		task_sleep (TIME_50MS);

		// if we've not already recorded the position at which the left opto turns off do that now
		if (!foundPos && !switch_poll_logical (SW_ZR_1_FULL_LEFT)) {
			zr1_pos_full_left_opto_off = zr1_engine_position;
			foundPos = TRUE;
		}

		if (switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
			// the position we're at is where the opto turned on.
			zr1_pos_full_right_opto_on = zr1_engine_position;
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

	for (zr1_engine_position = zr1_pos_full_right_opto_on; zr1_engine_position > zr1_pos_center; zr1_engine_position--) {
		writeb (ZR1_ENGINE_POS, zr1_engine_position);
		task_sleep (TIME_50MS);

		// if we've not already recorded the position at which the right opto turns off do that now
		if (!foundPos && !switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
			zr1_pos_full_right_opto_off = zr1_engine_position;
			foundPos = TRUE;

			// when the right opto turns off we can calculate the center position.
			zr1_calculate_center_pos();
		}
	}

	zr1_disable_solenoids();

	zr1_calibration_succeded();

}

CALLSET_ENTRY (zr1, diagnostic_check)
{
	if (!feature_config.enable_zr1_engine)
		diag_post_error ("ZR1 ENGINE DISABLED\nBY ADJUSTMENT\n", PAGE);

	while (unlikely (zr1_state == ZR1_CALIBRATING))
		task_sleep (TIME_500MS); // it takes a few seconds to complete

	if (!global_flag_test (GLOBAL_FLAG_ZR1_WORKING))
		diag_post_error ("ZR1 ENGINE IS\nNOT WORKING\n", PAGE);
}

CALLSET_ENTRY (zr1, init)
{
	zr1_initialise();
}

CALLSET_ENTRY (zr1, amode_start)
{
	if (!feature_config.enable_zr1_engine) {
		return;
	}

	if (!zr1_calibration_attempted) {
		zr1_calibrate();
		return;
	}

	zr1_stop();
}

CALLSET_ENTRY (zr1, amode_stop, test_start)
{
	zr1_stop();
}

/**
 * Reset the engine to the center position at the start of each ball.
 */
CALLSET_ENTRY (zr1, start_ball, end_ball)
{
	zr1_stop();
}

void corvette_zr1_engine_rtt (void) {

	if (zr1_state != ZR1_SHAKING) {
		return;
	}

	// TODO implement
	// look for zr1_state == ZR1_SHAKING and alternate the position between zr1_pos_full_left_opto_off and zr1_pos_full_right_opto_off
	// with a short delay (~200M) between them to allow the engine to move left and right

}
