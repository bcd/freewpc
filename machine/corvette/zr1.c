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

/**
 * Controls the ZR1 engine ball shaker
 *
 * @TODO add ball search functionality (another state?)
 */
#include <freewpc.h>
#include <diag.h>
#include <corvette/zr1_state.h>

#define ZR1_ENGINE_POS WPC_EXTBOARD2
#define ZR1_ENGINE_CONTROL WPC_EXTBOARD3

//
// Calibration
//

#define ZR1_ENGINE_LEFT_MIN 0x01 // FIXME should be 0x00 but compiler bug causes problems with > operator when using "0" or "0x00" in zr1_calibrate()
#define ZR1_ENGINE_RIGHT_MAX 0xFF
#define ZR1_ENGINE_CENTER 0x7F

// 16 is the frequency of the ZR1 rtt, see corvette.sched:corvette_zr1_engine_rtt
#define ZR1_CENTER_TICKS 8 // 16 * 8 = 128MS

U8 foundPos;
U8 zr1_pos_center;
U8 zr1_engine_position; // TODO rename to zr1_last_position
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

/* Mode that drives the rtt state machine */
__fastram__ enum mech_zr1_state zr1_state;
__fastram__ enum mech_zr1_state zr1_previous_state;
U8 zr1_previously_enabled;
U8 zr1_calibrated;
U8 zr1_calibration_attempted;
U8 zr1_center_ticks_remaining;

void zr1_reset(void) {
	zr1_previously_enabled = FALSE;
	zr1_calibrated = FALSE;
	zr1_calibration_attempted = FALSE;
	zr1_state = ZR1_IDLE;
	zr1_previous_state = ZR1_IDLE;

	global_flag_off(GLOBAL_FLAG_ZR1_WORKING);
	global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);

	// provide some default values, just in case.
	zr1_pos_center = ZR1_ENGINE_CENTER;
	zr1_pos_full_left_opto_on = ZR1_ENGINE_LEFT_MIN;
	zr1_pos_full_left_opto_off = ZR1_ENGINE_LEFT_MIN;
	zr1_pos_full_right_opto_on = ZR1_ENGINE_RIGHT_MAX;
	zr1_pos_full_right_opto_off = ZR1_ENGINE_RIGHT_MAX;
}

// should not be used outside of this file
void zr1_set_position_to_center(void) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	zr1_engine_position = zr1_pos_center;
	writeb (ZR1_ENGINE_POS, zr1_engine_position);
}


/**
 * Enable the solenoids that drive the ZR1 ball shaker device
 *
 * This can be called regardless of calibration state.
 * Has no effect if solenoid power is already enabled.
 * If the solenoids have not previously been enabled the engine will move to it's center position
 * when the solenoid power is enabled, otherwise the engine will move to it's last-set position.
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

	if (!zr1_previously_enabled) {
		zr1_previously_enabled = TRUE;
		zr1_set_position_to_center();
	}
	writeb (ZR1_ENGINE_CONTROL, 1); // disable the DISABLE_A/DISABLE_B lines
	global_flag_on(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
}

/**
 * Disable the solenoids that drive the ZR1 ball shaker device
 */
void zr1_disable_solenoids(void) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	if (!global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED)) {
		return; // already off
	}

	writeb (ZR1_ENGINE_CONTROL, 0); // enable the DISABLE_A/DISABLE_B lines
	global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
}

void zr1_calculate_center_pos(void) {
	zr1_pos_center = (zr1_pos_full_right_opto_off + zr1_pos_full_left_opto_off ) / 2;
}

U8 zr1_enter_state(enum mech_zr1_state new_state) {
	U8 allow_state_change = FALSE;
	switch (zr1_state) {
		case ZR1_SHAKE:
			allow_state_change = zr1_calibrated;
			break;
		default:
			allow_state_change = TRUE;
	}
	if (!allow_state_change) {
		return allow_state_change; // always FALSE
	}
	zr1_state = new_state;
	return allow_state_change; // always TRUE
}

void zr1_center(void) {
	zr1_enter_state(ZR1_CENTER);
}

void zr1_state_center_enter(void) {
	interrupt_dbprintf ("zr1_state_center_enter: enter\n");
	zr1_set_position_to_center();
	zr1_enable_solenoids();
	zr1_center_ticks_remaining = ZR1_CENTER_TICKS;
	interrupt_dbprintf ("zr1_state_center_enter: exit\n");
}

void zr1_state_center_run(void) {
	interrupt_dbprintf ("zr1_state_center_run: enter\n");
	if (zr1_center_ticks_remaining > 0) {
		zr1_center_ticks_remaining--;
		interrupt_dbprintf ("zr1_state_center_run: zr1_center_ticks_remaining = %d\n", zr1_center_ticks_remaining);
		if (zr1_center_ticks_remaining == 0) {
			dbprintf ("zr1_state_center_run: engine centered\n");
		}
	}
	interrupt_dbprintf ("zr1_state_center_run: exit\n");
}

void zr1_calibrate(void) {
	zr1_enter_state(ZR1_CALIBRATE);
}

void zr1_state_calibrate_enter(void) {
	zr1_calibration_attempted = TRUE;
}

void zr1_state_calibrate_run(void) {
	// TODO port old zr1_calibrate() routine to RTT
}

void zr1_idle(void) {
	zr1_enter_state(ZR1_IDLE);
}

void zr1_state_idle_enter(void) {
	zr1_set_position_to_center();
	zr1_center_ticks_remaining = ZR1_CENTER_TICKS;
}

void zr1_state_idle_run(void) {
	if (zr1_center_ticks_remaining > 0) {
		zr1_center_ticks_remaining--;
	} else {
		zr1_disable_solenoids();
	}
}

void zr1_shake(void) {
	zr1_enter_state(ZR1_SHAKE);
}

void corvette_zr1_engine_rtt (void) {

	switch (zr1_state) {
		case ZR1_CENTER:
			if (zr1_previous_state != zr1_state) {
				zr1_state_center_enter();
			} else {
				zr1_state_center_run();
			}
		break;

		case ZR1_CALIBRATE:
			if (zr1_previous_state != zr1_state) {
				zr1_state_calibrate_enter();
			} else {
				zr1_state_calibrate_run();
			}
		break;

		case ZR1_IDLE:
			if (zr1_previous_state != zr1_state) {
				zr1_state_idle_enter();
			} else {
				zr1_state_idle_run();
			}
		break;

		case ZR1_SHAKE:
			// TODO implement
			// alternate the engine's position between zr1_pos_full_left_opto_off and zr1_pos_full_right_opto_off
			// with a short delay (~200MS?) between them to allow the engine to move left and right
		break;

		default:
			// shut the compiler up
			break;
	}
	zr1_previous_state = zr1_state;
}

/*
void zr1_calibration_failed(U8 code) {
	calibration_running = FALSE;
	zr1_disable_solenoids();
	audit_increment (&feature_audits.zr1_errors);
	diag_post_error (errors[code], MACHINE_PAGE);

	dbprintf("zr1 engine calibration failed\n");
	dbprintf("error: %s\n", errors[code]);

}

void zr1_calibration_succeded(void) {
	dbprintf("zr1 engine calibration complete\n");

	calibration_running = FALSE;

	zr1_stop();
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

	// center engine

	dbprintf("zr1 engine calibration stage 1\n");

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
	dbprintf("zr1 engine calibration stage 2\n");

	for (; zr1_engine_position > ZR1_ENGINE_LEFT_MIN; zr1_engine_position--) {
		writeb (ZR1_ENGINE_POS, zr1_engine_position);
		task_sleep (ZR1_CALIBRATION_SPEED);
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

	dbprintf("zr1 engine calibration stage 3\n");
	foundPos = FALSE;

	for (zr1_engine_position = zr1_pos_full_left_opto_on; zr1_engine_position < ZR1_ENGINE_RIGHT_MAX; zr1_engine_position++) {
		writeb (ZR1_ENGINE_POS, zr1_engine_position);
		task_sleep (ZR1_CALIBRATION_SPEED);

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

	dbprintf("zr1 engine calibration stage 4\n");
	foundPos = FALSE;

	for (zr1_engine_position = zr1_pos_full_right_opto_on; zr1_engine_position > zr1_pos_center; zr1_engine_position--) {
		writeb (ZR1_ENGINE_POS, zr1_engine_position);
		task_sleep (ZR1_CALIBRATION_SPEED);

		// if we've not already recorded the position at which the right opto turns off do that now
		if (!foundPos && !switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
			zr1_pos_full_right_opto_off = zr1_engine_position;
			foundPos = TRUE;

			// when the right opto turns off we can calculate the center position.
			zr1_calculate_center_pos();
		}
	}

	zr1_calibration_succeded();

}
*/

CALLSET_ENTRY (zr1, diagnostic_check)
{
	if (zr1_state == ZR1_CALIBRATE) {
		dbprintf ("zr1: diagnostic_check - waiting for calibration\n");
		while (zr1_state == ZR1_CALIBRATE) {
			task_sleep(TIME_1S);
		}
		dbprintf ("zr1: diagnostic_check - calibration performed\n");
	}

	if (!feature_config.enable_zr1_engine) {
		dbprintf ("zr1: ZR1 ENGINE DISABLED BY ADJUSTMENT\n");

		diag_post_error ("ZR1 ENGINE DISABLED\nBY ADJUSTMENT\n", MACHINE_PAGE);
		return;
	}

	if (!global_flag_test (GLOBAL_FLAG_ZR1_WORKING)) {
		dbprintf ("zr1: ZR1 ENGINE IS NOT WORKING\n");

		diag_post_error ("ZR1 ENGINE IS\nNOT WORKING\n", MACHINE_PAGE);
		return;
	}
}

CALLSET_ENTRY (zr1, init_complete, amode_start) {
	dbprintf ("zr1: init_complete/amode_start entry\n");

	if (!zr1_calibration_attempted) {
		dbprintf ("starting zr1 calibration\n");
		zr1_calibrate();
		return;
	}

	dbprintf ("zr1: init_complete/amode_start exit\n");
}

CALLSET_ENTRY (zr1, init)
{
	zr1_reset();
}

CALLSET_ENTRY (zr1, amode_stop, test_start, stop_game)
{
	dbprintf ("zr1: amode_stop, test_start, stop_game entry\n");
	zr1_enter_state(ZR1_IDLE);
}

/**
 * Reset the engine to the center position at the start of each ball.
 */
CALLSET_ENTRY (zr1, start_ball, end_ball)
{
	zr1_idle();
}
