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
#include <corvette/zr1.h>

#define ZR1_ENGINE_POS WPC_EXTBOARD2
#define ZR1_ENGINE_CONTROL WPC_EXTBOARD3

//
// Calibration
//

#define ZR1_ENGINE_LEFT_MIN 0x00
#define ZR1_ENGINE_RIGHT_MAX 0xFF
#define ZR1_ENGINE_CENTER 0x7F

// NOTE: 16 in the calculations below is the frequency of ZR1 RTT task calls in MS, see corvette.sched:corvette_zr1_engine_rtt

// Specifies how long it takes the engine to move the engine from ANY position to the center, and settle.
#define ZR1_CENTER_TICKS 8 // 16 * 8 = 128MS

// Specifies how long it takes the engine to move from one side to the other at full speed using the default range.
#define ZR1_SHAKE_TICKS 4 // 16 * 4 = 64MS

// Specifies how long it takes the engine to move from one side to the other at full speed using the default range.
#define ZR1_BALL_SEARCH_TICKS 8 // 16 * 8 = 128MS

// Specifies how long the RTT should wait before changing the position of the engine during calibration.
#define ZR1_CALIBRATE_MOVE_TICKS 5 // 16 * 5 = 80MS

U8 foundPos;
U8 zr1_pos_center;
__fastram__ U8 zr1_last_position;
__fastram__ U8 zr1_shake_speed;
__fastram__ U8 zr1_shake_range;
__fastram__ U8 zr1_pos_shake_left;
__fastram__ U8 zr1_pos_shake_right;
U8 zr1_pos_full_left_opto_on;
U8 zr1_pos_full_left_opto_off;
U8 zr1_pos_full_right_opto_on;
U8 zr1_pos_full_right_opto_off;

// TODO these are somewhat similar to mech_zr1_calibration_messages but with line feeds instead of spaces
char *mech_zr1_diag_messages[] = {
	"NOT CALIBRATED\n",             // NEVER SEEN
	"ZR1 ERROR 1\nCHECK F111\n",
	"CHECK OPTO\nZR1 FULL LEFT\n",
	"CHECK OPTO\nZR1 FULL RIGHT\n",
	"ZR1 ERROR 2\nCHECK ENGINE\n",
	"CALIBRATED O.K.\n"             // NEVER SEEN
};

enum mech_zr1_calibration_codes zr1_last_calibration_result_code;

U8 zr1_previously_enabled;
U8 zr1_calibrated;
U8 zr1_calibration_attempted;

/* Mode that drives the rtt state machine */
__fastram__ enum mech_zr1_state zr1_state;
__fastram__ enum mech_zr1_state zr1_previous_state;

// RTT counters and flags
__fastram__ U8 zr1_center_ticks_remaining;
__fastram__ U8 zr1_shake_ticks_remaining;
//__fastram__ U16 zr1_calibrate_timeout_ticks_remaining;
__fastram__ U8 zr1_calibrate_move_ticks_remaining;

enum mech_zr1_shake_direction {
	ZR1_SHAKE_DIRECTION_LEFT = 0,
	ZR1_SHAKE_DIRECTION_RIGHT
};

__fastram__ enum mech_zr1_shake_direction zr1_shake_direction;

enum mech_zr1_calibrate_state {
	ZR1_CALIBRATE_CENTER = 0,
	ZR1_CALIBRATE_LEFT,
	ZR1_CALIBRATE_RIGHT,
	ZR1_CALIBRATE_RECENTER
};

__fastram__ enum mech_zr1_calibrate_state zr1_calibrate_state;


/*
 *
 * Methods shared between the RTT and common code
 *
 *
 */

void zr1_calculate_shake_range(void) {
	zr1_pos_shake_left = zr1_pos_full_left_opto_off + (((zr1_pos_center - zr1_pos_full_left_opto_off) / 5) * zr1_shake_range);
	zr1_pos_shake_right = zr1_pos_full_right_opto_off - (((zr1_pos_full_right_opto_off - zr1_pos_center) / 5) * zr1_shake_range);
}

/**
 * Reset the engine position limits used during calibration
 */
void zr1_reset_limits(void) {
	// provide some default values, just in case.
	zr1_pos_center = ZR1_ENGINE_CENTER;
	zr1_pos_full_left_opto_on = ZR1_ENGINE_LEFT_MIN;
	zr1_pos_full_left_opto_off = ZR1_ENGINE_LEFT_MIN;
	zr1_pos_full_right_opto_on = ZR1_ENGINE_RIGHT_MAX;
	zr1_pos_full_right_opto_off = ZR1_ENGINE_RIGHT_MAX;
	zr1_calculate_shake_range();
}

/*
 *
 * RTT ONLY methods
 *
 */

// should not be used outside of this file
void zr1_set_position_to_center(void) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	zr1_last_position = zr1_pos_center;
	writeb (ZR1_ENGINE_POS, zr1_last_position);
}

// should not be used outside of this file
void zr1_set_position(U8 position) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	zr1_last_position = position;
	writeb (ZR1_ENGINE_POS, zr1_last_position);
}


/**
 * Enable the solenoids that drive the ZR1 ball shaker device
 *
 * Should not be used outside of this file
 *
 * This can be called regardless of calibration state.
 * If the solenoids have not previously been enabled the engine will move to it's center position
 * when the solenoid power is enabled, otherwise the engine will move to it's last-set position.
 *
 * @See zr1_init()
 */
void zr1_enable_solenoids(void) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	if (!zr1_previously_enabled) {
		zr1_previously_enabled = TRUE;
		zr1_set_position_to_center();
	}
	writeb (ZR1_ENGINE_CONTROL, 1); // disable the DISABLE_A/DISABLE_B lines
	if (!global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED)) {
		//sample_start (SND_STARTER_MOTOR, SL_500MS); // XXX - trigger sound when changing power
		global_flag_on(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
	}
}

/**
 * Disable the solenoids that drive the ZR1 ball shaker device
 *
 * Should not be used outside of this file
 */
void zr1_disable_solenoids(void) {
	if (!feature_config.enable_zr1_engine) {
		return; // disabled
	}

	writeb (ZR1_ENGINE_CONTROL, 0); // enable the DISABLE_A/DISABLE_B lines
	if (global_flag_test(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED)) {
		//sample_start (SND_SPARK_PLUG_01, SL_500MS); // XXX - trigger sound when changing power
		global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);
	}
}

void zr1_calculate_center_pos(void) {
	zr1_pos_center = ((U16)zr1_pos_full_right_opto_off + zr1_pos_full_left_opto_off) / 2;
}

void zr1_state_center_enter(void) {
	zr1_set_position_to_center();
	zr1_enable_solenoids();
	zr1_center_ticks_remaining = ZR1_CENTER_TICKS;
}

void zr1_state_center_run(void) {
	if (zr1_center_ticks_remaining > 0) {
		zr1_center_ticks_remaining--;
	}
	zr1_enable_solenoids();
}

void zr1_state_calibrate_exit(void) {
	zr1_calibration_attempted = TRUE;
	zr1_state = ZR1_FLOAT;
}

void zr1_calibration_failed(enum mech_zr1_calibration_codes code) {
	// store the code for use outside the RTT as diag_post can't be called from an RTT.
	zr1_last_calibration_result_code = code;
	zr1_reset_limits();
	zr1_state_calibrate_exit();
}

void zr1_calibration_complete(void) {
	zr1_calibrated = TRUE;
	zr1_last_calibration_result_code = CC_SUCCESS;
	zr1_calculate_shake_range();
	global_flag_on(GLOBAL_FLAG_ZR1_WORKING);
	zr1_state_calibrate_exit();
}

void zr1_state_calibrate_enter(void) {
	zr1_calibrated = FALSE;
	zr1_calibration_attempted = FALSE;
	zr1_last_calibration_result_code = CC_NOT_CALIBRATED;
	global_flag_off(GLOBAL_FLAG_ZR1_WORKING);
	zr1_reset_limits();

	if (switch_poll_logical (SW_ZR_1_FULL_LEFT) && switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
		// Both engine optos cannot be on at the same time, probably F111 fuse, switch matrix problem, or one or more dirty/faulty optos.
		zr1_calibration_failed(CC_CHECK_F111);
		return;
	}

	// initialise the first calibration state
	zr1_set_position_to_center();
	zr1_enable_solenoids();
	zr1_calibrate_move_ticks_remaining = ZR1_CENTER_TICKS * 4; // wait a bit longer for settle during calibration.
	zr1_calibrate_state = ZR1_CALIBRATE_CENTER;
}

void zr1_state_calibrate_run(void) {
	zr1_enable_solenoids();

	zr1_calibrate_move_ticks_remaining--;
	if (zr1_calibrate_move_ticks_remaining != 0) {
		return;
	}

	// reset the move timer
	zr1_calibrate_move_ticks_remaining = ZR1_CALIBRATE_MOVE_TICKS;

	// we've waited long enough now until we should check things again.

	switch(zr1_calibrate_state) {
		case ZR1_CALIBRATE_CENTER:
			if (switch_poll_logical (SW_ZR_1_FULL_LEFT) || switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
				// If either of the optos is still on then the engine is not in the center, bail!
				zr1_calibration_failed(CC_CHECK_ENGINE);
				break;
			}

			zr1_calibrate_state = ZR1_CALIBRATE_LEFT;
		break;

		case ZR1_CALIBRATE_LEFT:
			// move from center to the left until either the limit is hit or the left opto activates

			if (switch_poll_logical (SW_ZR_1_FULL_LEFT)) {
				// the position we're at is where the opto turned on.
				zr1_pos_full_left_opto_on = zr1_last_position;

				// setup for next state
				foundPos = FALSE;
				zr1_calibrate_state = ZR1_CALIBRATE_RIGHT;
				zr1_calibrate_move_ticks_remaining = ZR1_CALIBRATE_MOVE_TICKS;
				break;
			}

			if (zr1_last_position > ZR1_ENGINE_LEFT_MIN) {
				zr1_set_position(zr1_last_position - 1);
			} else {
				// if we reached the min left value and the left opto is not active the opto may be dead
				zr1_calibration_failed(CC_CHECK_FULL_LEFT_OPTO);
			}
		break;

		case ZR1_CALIBRATE_RIGHT:
			// move from left to right until either the limit is hit or the right opto activates

			// if we've not already recorded the position at which the left opto turns off do that now
			if (!foundPos && !switch_poll_logical (SW_ZR_1_FULL_LEFT)) {
				zr1_pos_full_left_opto_off = zr1_last_position;
				foundPos = TRUE;
			}

			if (switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
				// the position we're at is where the opto turned on.
				zr1_pos_full_right_opto_on = zr1_last_position;

				// setup for next state
				foundPos = FALSE;
				zr1_calibrate_state = ZR1_CALIBRATE_RECENTER;
				zr1_calibrate_move_ticks_remaining = ZR1_CALIBRATE_MOVE_TICKS;
				break;
			}

			if (zr1_last_position < ZR1_ENGINE_RIGHT_MAX) {
				zr1_set_position(zr1_last_position + 1);
			} else {
				zr1_calibration_failed(CC_CHECK_FULL_RIGHT_OPTO);
			}
		break;

		case ZR1_CALIBRATE_RECENTER:
			// move from right to the center

			// if we've not already recorded the position at which the right opto turns off do that now
			if (!foundPos && !switch_poll_logical (SW_ZR_1_FULL_RIGHT)) {
				zr1_pos_full_right_opto_off = zr1_last_position;
				foundPos = TRUE;

				// when the right opto turns off we can calculate the center position.
				zr1_calculate_center_pos();
			}

			if (zr1_last_position > zr1_pos_center) {
				zr1_set_position(zr1_last_position - 1);
			} else {

				if (foundPos) {
					zr1_calibration_complete();
				} else {
					// the right opto should have turned off by now
					zr1_calibration_failed(CC_CHECK_FULL_RIGHT_OPTO);
				}
			}
		break;
	}
}

void zr1_state_float_enter(void) {
	zr1_set_position_to_center();
	zr1_center_ticks_remaining = ZR1_CENTER_TICKS;
}

void zr1_state_float_run(void) {
	if (zr1_center_ticks_remaining > 0) {
		zr1_center_ticks_remaining--;
		return;
	}
	// turn the solenoids after the engine has centered
	zr1_disable_solenoids();
}

void zr1_state_shake_enter(void) {
	zr1_shake_ticks_remaining = ZR1_SHAKE_TICKS / 2; // we assume engine is in the center before shaking starts, that being the case it only has to travel half the distance when beginning the shake

	// always begin by moving left
	zr1_shake_direction = ZR1_SHAKE_DIRECTION_LEFT;
	zr1_set_position(zr1_pos_shake_left);
	zr1_enable_solenoids();
}

void zr1_state_shake_run(void) {
	zr1_enable_solenoids();

	if (zr1_shake_ticks_remaining > 0) {
		zr1_shake_ticks_remaining--;
		return;
	}
	// reset counter
	zr1_shake_ticks_remaining = ZR1_SHAKE_TICKS * zr1_shake_speed;

	// reverse direction
	if (zr1_shake_direction == ZR1_SHAKE_DIRECTION_LEFT) {
		zr1_shake_direction = ZR1_SHAKE_DIRECTION_RIGHT;
		zr1_set_position(zr1_pos_shake_right);
	} else {
		zr1_shake_direction = ZR1_SHAKE_DIRECTION_LEFT;
		zr1_set_position(zr1_pos_shake_left);
	}
}

// Ball searching reuses some of the shake variables to save space
// zr1_shake_ticks_remaining, zr1_shake_direction

void zr1_state_ball_search_enter(void) {
	zr1_shake_ticks_remaining = ZR1_BALL_SEARCH_TICKS / 2; // we assume engine is in the center before searching starts, that being the case it only has to travel half the distance when beginning the search

	// always begin by moving left
	zr1_shake_direction = ZR1_SHAKE_DIRECTION_LEFT;
	zr1_set_position(zr1_pos_full_left_opto_on);
	zr1_enable_solenoids();
}

void zr1_state_ball_search_run(void) {
	zr1_enable_solenoids();

	if (zr1_shake_ticks_remaining > 0) {
		zr1_shake_ticks_remaining--;
		return;
	}
	// reset counter
	zr1_shake_ticks_remaining = (U8)ZR1_BALL_SEARCH_TICKS * (U8)ZR1_BALL_SEARCH_SPEED;

	// reverse direction
	if (zr1_shake_direction == ZR1_SHAKE_DIRECTION_LEFT) {
		zr1_shake_direction = ZR1_SHAKE_DIRECTION_RIGHT;
		zr1_set_position(zr1_pos_full_right_opto_on);
	} else {
		zr1_shake_direction = ZR1_SHAKE_DIRECTION_LEFT;
		zr1_set_position(zr1_pos_full_left_opto_on);
	}
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

		case ZR1_FLOAT:
			if (zr1_previous_state != zr1_state) {
				zr1_state_float_enter();
			} else {
				zr1_state_float_run();
			}
		break;

		case ZR1_SHAKE:
			if (zr1_previous_state != zr1_state) {
				zr1_state_shake_enter();
			} else {
				zr1_state_shake_run();
			}
		break;

		case ZR1_BALL_SEARCH:
			if (zr1_previous_state != zr1_state) {
				zr1_state_ball_search_enter();
			} else {
				zr1_state_ball_search_run();
			}
		break;

		default:
			// shut the compiler up
			break;
	}
	zr1_previous_state = zr1_state;
}

/*
 *
 * NON-RTT methods
 *
 */

void zr1_reset(void) {
	disable_interrupts();
	zr1_previously_enabled = FALSE;
	zr1_calibrated = FALSE;
	zr1_calibration_attempted = FALSE;
	zr1_last_calibration_result_code = CC_NOT_CALIBRATED;
	zr1_state = ZR1_FLOAT;
	zr1_previous_state = ZR1_INITIALIZE; // Note: this state must be used so that zr1_state_float_enter is called, without this first state zr1_state_float_enter would not be called.

	zr1_shake_speed = ZR1_SHAKE_SPEED_DEFAULT;
	zr1_shake_range = ZR1_SHAKE_RANGE_DEFAULT;

	global_flag_off(GLOBAL_FLAG_ZR1_WORKING);
	global_flag_off(GLOBAL_FLAG_ZR1_SOLENOIDS_POWERED);

	zr1_reset_limits();
	enable_interrupts();
	zr1_disable_solenoids();

}

void zr1_enter_state(enum mech_zr1_state new_state) {
	U8 allow_state_change = FALSE;
	switch (new_state) {
		case ZR1_SHAKE:
			allow_state_change = zr1_calibrated;
			break;
		default:
			allow_state_change = TRUE;
	}
	if (!allow_state_change) {
		dbprintf("current state: %d, disallowing new state: %d\n", zr1_state, new_state);
	} else {
		dbprintf("current state: %d, enabling new state: %d\n", zr1_state, new_state);
		disable_interrupts();
		zr1_state = new_state;
		enable_interrupts();
	}
}

void zr1_float(void) {
	zr1_enter_state(ZR1_FLOAT);
}

void zr1_calibrate(void) {
	zr1_enter_state(ZR1_CALIBRATE);
}

void zr1_center(void) {
	zr1_enter_state(ZR1_CENTER);
}

void zr1_shake(void) {
	zr1_enter_state(ZR1_SHAKE);
}

void zr1_start_ball_search(void) {
	zr1_enter_state(ZR1_BALL_SEARCH);
}

/**
 * Sets the shake speed
 *
 * New speed is forced to between 1 and 10
 *
 * 1 = fast
 * 10 = slow
 *
 * The value is used as a multiplier
 */
void zr1_set_shake_speed(U8 new_shake_speed) {
	if (new_shake_speed > ZR1_SHAKE_SPEED_MAX) {
		new_shake_speed = ZR1_SHAKE_SPEED_MAX;
	}
	if (new_shake_speed < ZR1_SHAKE_SPEED_MIN) {
		new_shake_speed = ZR1_SHAKE_SPEED_MIN;
	}
	disable_interrupts();
	zr1_shake_speed = new_shake_speed;
	enable_interrupts();
}

/**
 * Sets the shake range
 *
 * New range is forced to between 1 and 5
 *
 * 1 = 1/5th of the maximum range (narrowest arc)
 * 5 = 5/5ths of the maximum range (widest arc)
 *
 * @see zr1_calculate_shake_range
 */
void zr1_set_shake_range(U8 new_shake_range) {
	if (new_shake_range > ZR1_SHAKE_RANGE_MAX) {
		new_shake_range = ZR1_SHAKE_RANGE_MAX;
	}
	if (new_shake_range < ZR1_SHAKE_RANGE_MIN) {
		new_shake_range = ZR1_SHAKE_RANGE_MIN;
	}
	disable_interrupts();
	zr1_shake_range = new_shake_range;
	zr1_calculate_shake_range();
	enable_interrupts();
}

CALLSET_ENTRY (zr1, init)
{
	zr1_reset();
}

CALLSET_ENTRY (zr1, amode_stop, test_start, stop_game)
{
	dbprintf ("zr1: amode_stop, test_start, stop_game entry\n");
	zr1_float();
}


void zr1_shake_2sec_task (void)
{
	zr1_shake();
	task_sleep_sec (2);
	zr1_center();
	task_exit ();
}

/**
 * Reset the engine to the center position at the start of each ball.
 */
CALLSET_ENTRY (zr1, start_ball, end_ball)
{
	task_create_gid1 (GID_ZR1_SHAKE, zr1_shake_2sec_task);
}

/**
 * Reset the engine to the center position at the end of each ball.
 */
CALLSET_ENTRY (zr1, end_ball)
{
	zr1_center();
}


CALLSET_ENTRY (zr1, diagnostic_check)
{
	if (!feature_config.enable_zr1_engine) {
		dbprintf ("zr1: ZR1 ENGINE DISABLED BY ADJUSTMENT\n");

		diag_post_error ("ZR1 DISABLED\nBY ADJUSTMENT\n", PAGE);
		return;
	}

	if (!zr1_calibration_attempted) {
		return;
	}

	dbprintf ("calibration result: %d - %s", zr1_last_calibration_result_code, mech_zr1_diag_messages[zr1_last_calibration_result_code]); // No trailing \n as diag message contains one already
	if (zr1_last_calibration_result_code != CC_SUCCESS) {
		audit_increment (&feature_audits.zr1_errors);
		diag_post_error (mech_zr1_diag_messages[zr1_last_calibration_result_code], PAGE);
	}
}

CALLSET_ENTRY (zr1, init_complete, amode_start) {
	dbprintf ("zr1: init_complete/amode_start entry\n");

	if (!zr1_calibration_attempted) {
		dbprintf ("starting zr1 calibration\n");
		zr1_calibrate();
	}

	dbprintf ("zr1: init_complete/amode_start exit\n");
}

CALLSET_ENTRY (zr1, ball_search) {
	dbprintf ("zr1: ball_search\n");
	zr1_start_ball_search();
}

CALLSET_ENTRY (zr1, ball_search_end) {
	dbprintf ("zr1: ball_search\n");
	if (in_live_game) {
		zr1_center();
	} else {
		zr1_float();
	}
}

CALLSET_ENTRY (zr1, start_game) {
	zr1_center();
}

CALLSET_ENTRY (zr1, end_game) {
	zr1_float();
}
