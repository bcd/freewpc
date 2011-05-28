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
 * Controls the racetrack
 *
 * The racetrack is controlled by three solenoid outputs
 * 1) race direction (control polarity of BOTH motors)
 * 2) left car (drives a motor for the left car)
 * 3) right car (drives a motor for the right car)
 *
 * There are 4 switches that are used by the racetrack.
 * 4 optos
 * one for each car's encoder wheel, used to determine if the car is stationary or moving.
 * one for each car's at-start-of-track, used to determine if the car is at the start of the track.
 *
 * There are few lamps too.
 * 8 lights arranged in a tree, 4 amber, 2 green, 2 red.
 * 2 lights under the start of the cars, appear to be controlled by the race enable solenoids. (TODO Verify)
 *
 * The lights in the tree are not all independent.
 * The two top amber lights can only be enabled at the same time.
 * The two bottom amber lights can only be enabled at the same time.
 * All the red and green tree lights can be enabled independently.
 *
 * A typical light sequence for a race would be:
 * 1) Both top amber ON - ready, pause
 * 2) Both bottom amber ON - set, pause
 * 3) Both green lights ON - go - race starts
 * 4) Amber lights go OFF
 * 5) Winning car's green light stays ON
 * 6) Loosing car's red light goes ON and green light goes OFF
 * 7) cars reset - all lights go OFF
 *
 * From the machine definition, for reference:
 *
 * Solenoids:
 * G = J126 on Power Driver Board
 * G1: Race Direction, nosearch
 * G2: Left Race Enable, motor
 * G3: Right Race Enable, motor
 *
 * Switches:
 * 51: Left Race Start, noscore, opto
 * 52: Right Race Start, noscore, opto
 * 55: Left Race Encoder, noscore, opto
 * 56: Right Race Encoder, noscore, opto
 *
 *
 * The racetrack needs to be calibrated at power-up.
 * The calibration routine needs to reset each car to the start of the racetrack
 * Then it needs to drive each car in turn to the end of the racetrack
 * Then finally it needs to reset them again to the start of the racetrack.
 *
 */
#include <freewpc.h>
#include <diag.h>
#include <corvette/racetrack.h>

//
// Calibration
//

// TODO work out how many encoder 'ticks' there are when driving each car
// from the start to the end of the track.
// 120 ticks = 100%. 120 / 100 = 1.2 ticks per percent.
// Thus, to move a car from the start to reach 50% we must drive it 1.2 * 50

// the calibration routine should count ticks for each car in both directions, the result should be similar

#define RACETRACK_CALIBRATE_TICKS 16 // 32 * 16 = 512MS
#define RACETRACK_WATCHDOG_TICKS 6 // 32 * 6 = 192MS


// position are specified as a percentage, 0% being start 100% being the end.
// percentages are used because they are easily used when drawing progress
// meters, raising events, etc.
U8 left_car_position;
U8 right_car_position;
U8 left_car_desired_position;
U8 right_car_desired_position;

// watchdog
U8 racetrack_watchdog_counter;
/*__fastram__*/ U8 racetrack_seen_encoder_mask;
/*__fastram__*/ U8 left_encoder_last_state;
/*__fastram__*/ U8 right_encoder_last_state;
/*__fastram__*/ U8 racetrack_watchdog_ticks_remaining;
U16 left_encoder_count; // counts the ON state changes of the encoder
U16 right_encoder_count; // counts the ON state changes of the encoder

// track lengths are specified in encoder ticks, only valid after calibration.
U16 left_car_track_length;
U16 right_car_track_length;


/*__fastram__*/ U8 racetrack_calibrate_ticks_remaining;
U8 racetrack_calibrate_counter;

U8 racetrack_calibrated;
U8 racetrack_calibration_attempted;

/* Mode that drives the racetrack state machine */
/*__fastram__*/ enum mech_racetrack_state racetrack_state;
/*__fastram__*/ enum mech_racetrack_state racetrack_previous_state;

enum mech_racetrack_calibrate_state {
	RT_CALIBRATE_CAR_RETURN = 0,
	RT_CALIBRATE_LEFT_CAR_FORWARDS,
	RT_CALIBRATE_RIGHT_CAR_FORWARDS
// TODO implement remaining states
//	RT_CALIBRATE_LEFT_CAR_BACKWARDS,
//	RT_CALIBRATE_RIGHT_CAR_BACKWARDS
};

/*__fastram__*/ enum mech_racetrack_calibrate_state racetrack_calibrate_state;
/*__fastram__*/ enum mech_racetrack_calibrate_state racetrack_calibrate_next_state;

// FIXME these are somewhat similar to mech_racetrack_calibration_messages but with line feeds instead of spaces
// TODO add more calibration result messages
char *mech_racetrack_diag_messages[] = {
	"NOT CALIBRATED\n",
	"CHECK RACETRACK\n",
	"CHECK LEFT TRACK\n",
	"CALIBRATED O.K.\n"
};

enum mech_racetrack_calibration_codes racetrack_last_calibration_result_code;


/*
	if (!global_flag_test(GLOBAL_FLAG_RACETRACK_SOLENOIDS_POWERED)) {
		sample_start (SND_STARTER_MOTOR, SL_500MS); // XXX
		global_flag_on(GLOBAL_FLAG_RACETRACK_SOLENOIDS_POWERED);
	}
*/

/*

	if (global_flag_test(GLOBAL_FLAG_RACETRACK_SOLENOIDS_POWERED)) {
		//sample_start (SND_SPARK_PLUG_01, SL_500MS); // XXX
		global_flag_off(GLOBAL_FLAG_RACETRACK_SOLENOIDS_POWERED);
	}
*/


/**
 * Simple watchdog implementation.
 *
 * The implementation works by first resetting a bitmask to zero (via a call to reset_race_encoder_watchdog()
 * the RTT then calls update_race_encoder_watchdog(), that method then checks each track encoder optos to
 * see if they have changed state since last time they were checked, if so the corresponding bit is set in the
 * bitmask.
 * Every so often the update_race_encoder_watchdog() method also updates a counter (which wraps) providing a simple
 * way of checking to see if the encoders are moving.
 *
 * Code that uses it should first call reset_race_encoder_watchdog() then periodically check racetrack_seen_encoder_mask and
 * racetrack_watchdog_counter.
 */

void reset_race_encoder_watchdog(void) {
	racetrack_seen_encoder_mask = 0;
	racetrack_watchdog_counter = 0;
	left_encoder_count = 0;
	right_encoder_count = 0;
	racetrack_watchdog_ticks_remaining = RACETRACK_WATCHDOG_TICKS;
	left_encoder_last_state = switch_poll_logical (SW_LEFT_RACE_ENCODER);
	right_encoder_last_state = switch_poll_logical (SW_RIGHT_RACE_ENCODER);
}

#define RT_LEFT_ENCODER_BIT (1 << 0)
#define RT_RIGHT_ENCODER_BIT (1 << 1)

void update_race_encoder_watchdog(void) {

	// check the optos and update a bitmask
	if (left_encoder_last_state != switch_poll_logical (SW_LEFT_RACE_ENCODER)) {
		racetrack_seen_encoder_mask |= RT_LEFT_ENCODER_BIT;
		left_encoder_last_state = switch_poll_logical (SW_LEFT_RACE_ENCODER);

		// update a state change counter each time the switch becomes ON
		if (left_encoder_last_state) {
			left_encoder_count++; // don't care if it wraps
		}
	}
	if (right_encoder_last_state != switch_poll_logical (SW_RIGHT_RACE_ENCODER)) {
		racetrack_seen_encoder_mask |= RT_RIGHT_ENCODER_BIT;
		right_encoder_last_state = switch_poll_logical (SW_RIGHT_RACE_ENCODER);

		// update a state change counter each time the switch becomes ON
		if (right_encoder_last_state) {
			right_encoder_count++; // don't care if it wraps
		}

	}

	// check the timer
	racetrack_watchdog_ticks_remaining--;
	if (racetrack_watchdog_ticks_remaining != 0) {
		return;
	}
	// reset the timer
	racetrack_watchdog_ticks_remaining = RACETRACK_WATCHDOG_TICKS;

	// update the watchdog counter
	racetrack_watchdog_counter++; // don't care if it wraps.
}


void racetrack_state_calibrate_exit(void) {
	racetrack_calibration_attempted = TRUE;

	sol_disable(SOL_RACE_DIRECTION);
	sol_disable(SOL_LEFT_RACE_ENABLE);
	sol_disable(SOL_RIGHT_RACE_ENABLE);


	if (racetrack_calibrated) {
		racetrack_state = RACETRACK_CAR_RETURN; // if calibration was ok, cars will already be in the right place.
	} else {
		racetrack_state = RACETRACK_INITIALIZE;
	}
}

void racetrack_calibration_failed(enum mech_racetrack_calibration_codes code) {
	// store the code for use outside the RTT as diag_post can't be called from an RTT.
	racetrack_last_calibration_result_code = code;
	racetrack_state_calibrate_exit();
}

void racetrack_calibration_complete(void) {
	racetrack_calibrated = TRUE;
	racetrack_last_calibration_result_code = CC_SUCCESS;
	global_flag_on(GLOBAL_FLAG_RACETRACK_WORKING);
	racetrack_state_calibrate_exit();
}

void racetrack_state_calibrate_enter(void) {
	racetrack_calibrated = FALSE;
	racetrack_calibration_attempted = FALSE;
	racetrack_last_calibration_result_code = CC_NOT_CALIBRATED;

	left_car_track_length = 0;
	right_car_track_length = 0;

	global_flag_off(GLOBAL_FLAG_RACETRACK_WORKING);

	reset_race_encoder_watchdog();
	racetrack_calibrate_ticks_remaining = RACETRACK_CALIBRATE_TICKS;

	// initialise the first calibration state
	racetrack_calibrate_counter = 0;
	racetrack_calibrate_state = RT_CALIBRATE_CAR_RETURN;

}

#define RT_CALIBRATE_WATCHDOG_COUNTER_IGNORE 10

void racetrack_state_calibrate_run(void) {

	racetrack_calibrate_ticks_remaining--;
	if (racetrack_calibrate_ticks_remaining != 0) {
		return;
	}
	// reset the timer
	racetrack_calibrate_ticks_remaining = RACETRACK_CALIBRATE_TICKS;

	// we've waited long enough now until we should check things again.

	racetrack_calibrate_counter++;

	racetrack_calibrate_next_state = racetrack_calibrate_state;

	switch(racetrack_calibrate_state) {
		case RT_CALIBRATE_CAR_RETURN:

			/**
			 * The first time this state is hit the cars can be anywhere on the track
			 *
			 * The goal is to return both cars to the start position
			 */

			if (switch_poll_logical (SW_LEFT_RACE_START) && switch_poll_logical (SW_RIGHT_RACE_START)) {
				racetrack_calibrate_next_state = RT_CALIBRATE_LEFT_CAR_FORWARDS;
				break;
			}

			sol_disable(SOL_RACE_DIRECTION); // backwards

			if (switch_poll_logical (SW_LEFT_RACE_START)) {
				sol_disable(SOL_LEFT_RACE_ENABLE);
			} else {
				sol_enable(SOL_LEFT_RACE_ENABLE);
			}

			if (switch_poll_logical (SW_RIGHT_RACE_START)) {
				sol_disable(SOL_RIGHT_RACE_ENABLE);
			} else {
				sol_enable(SOL_RIGHT_RACE_ENABLE);
			}

			// both cars should be moving backwards

			if (racetrack_calibrate_counter < 2) {
				// wait ~1 seconds for the cars to move before checking the encoders.
				break;
			}

			// the time elapsed = ((RT_CALIBRATE_WATCHDOG_COUNTER_IGNORE * RACETRACK_WATCHDOG_TICKS) + RACETRACK_CALIBRATE_TICKS) * SCHEDULED RTT TIME
			// eg. ((10 * 6) + 16) * 32 = 2432ms (~2.5 seconds, should be plenty long enough for the cars to have moved.

			if (racetrack_seen_encoder_mask == 0) {
				// the cars should have been moving backwards, but the encoders were never seen.

				// Not sure if it's possible at this stage to determine what's at fault.
				// could be opto power, racetrack encoder or start optos, switch power, switch matrix, engine power, left or right car mechanisms (motor, gears, pulleys)
				// Ideally the racetrack test-menu would be able to help isolate the problem.
				racetrack_calibration_failed(CC_CHECK_RACETRACK);
				break;
			}

			if (racetrack_calibrate_counter >= 20) {
				// the encoder was still moving after 20 seconds
				racetrack_calibration_failed(CC_CHECK_RACETRACK);
				break;
			}

			// check again later
		break;
		case RT_CALIBRATE_LEFT_CAR_FORWARDS:

			/**
			 * The first time this state is hit both cars will be at the start position.
			 *
			 * The goal is to drive the left car to the end of the track and stop
			 */
			sol_enable(SOL_RACE_DIRECTION);
			sol_enable(SOL_LEFT_RACE_ENABLE);
			sol_disable(SOL_RIGHT_RACE_ENABLE);

			if (switch_poll_logical (SW_LEFT_RACE_START)) {
				if (racetrack_watchdog_counter >= RT_CALIBRATE_WATCHDOG_COUNTER_IGNORE) {
					// car didn't move forwards, or opto stuck on
					racetrack_calibration_failed(CC_CHECK_RACETRACK);
					break;
				}
				// wait till the car moves off the start before checking again
				break;
			}

			// the car has definitely moved, but has it stalled yet?

			if ((racetrack_seen_encoder_mask & RT_LEFT_ENCODER_BIT) == 0) {
				// yes, it has stalled as the encoder hasn't changed

				left_car_track_length = left_encoder_count;
				racetrack_calibrate_next_state = RT_CALIBRATE_RIGHT_CAR_FORWARDS;
				break;
			}

			// car still moving

			if (racetrack_calibrate_counter >= 20) { // ~20 seconds
				// If the encoder wheel is still going round after 20 seconds then something is wrong
				// could be a sticking car.
				racetrack_calibration_failed(CC_CHECK_LEFT_TRACK);
				break;
			}
		break;

		case RT_CALIBRATE_RIGHT_CAR_FORWARDS:

			/**
			 * The first time this state is hit the left car will be at the end of the track and the
			 * right car will be at the start of the track.
			 *
			 * The goal is to drive the right car to the end of the track and stop
			 */

			// TODO implement remaining calibration states
			racetrack_calibration_complete();
		break;

	}

	if (racetrack_calibrate_next_state != racetrack_calibrate_state) {
		reset_race_encoder_watchdog();
		racetrack_calibrate_state = racetrack_calibrate_next_state;
		racetrack_calibrate_counter = 0;
		racetrack_calibrate_ticks_remaining = 1; // wait for the next tick, then run. otherwise we'd wait too long between states.
	}

}

static inline void racetrack_state_float_enter(void) {
}

static inline void racetrack_state_float_run(void) {
	sol_disable(SOL_RACE_DIRECTION);
	sol_disable(SOL_LEFT_RACE_ENABLE);
	sol_disable(SOL_RIGHT_RACE_ENABLE);
}

static inline void racetrack_state_ready_enter(void) {
}

static inline void racetrack_state_ready_run(void) {
	sol_disable(SOL_RACE_DIRECTION);
	sol_disable(SOL_LEFT_RACE_ENABLE);
	sol_disable(SOL_RIGHT_RACE_ENABLE);
}


static inline void racetrack_state_car_return_enter(void) {
}

static inline void racetrack_state_car_return_run(void) {

	if (switch_poll_logical (SW_LEFT_RACE_START) && switch_poll_logical (SW_RIGHT_RACE_START)) {
		racetrack_state = RACETRACK_READY;
		return;
	}

	// TODO return the cars, similar to how RT_CALIBRATE_CAR_RETURN works.
}

void corvette_racetrack_rtt (void) {

	update_race_encoder_watchdog();

	switch (racetrack_state) {
		case RACETRACK_CALIBRATE:
			if (racetrack_previous_state != racetrack_state) {
				racetrack_state_calibrate_enter();
			} else {
				racetrack_state_calibrate_run();
			}
		break;

		case RACETRACK_FLOAT:
			if (racetrack_previous_state != racetrack_state) {
				racetrack_state_float_enter();
			} else {
				racetrack_state_float_run();
			}
		break;

		case RACETRACK_CAR_RETURN:
			if (racetrack_previous_state != racetrack_state) {
				racetrack_state_car_return_enter();
			} else {
				racetrack_state_car_return_run();
			}
		break;

		case RACETRACK_READY:
			if (racetrack_previous_state != racetrack_state) {
				racetrack_state_ready_enter();
			} else {
				racetrack_state_ready_run();
			}
		break;


		// TODO implement remaining states

		default:
			// shut the compiler up
		break;
	}
	racetrack_previous_state = racetrack_state;
}

/*
 *
 * NON-RTT methods
 *
 */

void racetrack_reset(void) {
	disable_interrupts();

	left_car_track_length = 0;
	right_car_track_length = 0;
	racetrack_calibrated = FALSE;
	racetrack_calibration_attempted = FALSE;
	racetrack_last_calibration_result_code = CC_NOT_CALIBRATED;
	racetrack_state = RACETRACK_INITIALIZE;
	racetrack_previous_state = RACETRACK_INITIALIZE;
	reset_race_encoder_watchdog();
	global_flag_off(GLOBAL_FLAG_RACETRACK_WORKING);

	enable_interrupts();
}

void racetrack_enter_state(enum mech_racetrack_state new_state) {
	U8 allow_state_change = FALSE;
	switch (new_state) {
		case RACETRACK_CAR_RETURN:
		case RACETRACK_RACE:
			allow_state_change = racetrack_calibrated;
			break;
		default:
			allow_state_change = TRUE;
	}
	if (!allow_state_change) {
		dbprintf("current state: %d, disallowing new state: %d\n", racetrack_state, new_state);
	} else {
		dbprintf("current state: %d, enabling new state: %d\n", racetrack_state, new_state);
		disable_interrupts();
		racetrack_state = new_state;
		enable_interrupts();
	}
}

void racetrack_float(void) {
	racetrack_enter_state(RACETRACK_FLOAT);
}

void racetrack_calibrate(void) {
	racetrack_enter_state(RACETRACK_CALIBRATE);
}

void racetrack_race(void) {
	racetrack_enter_state(RACETRACK_RACE);
}

void racetrack_car_return(void) {
	racetrack_enter_state(RACETRACK_CAR_RETURN);
}

CALLSET_ENTRY (racetrack, init)
{
	racetrack_reset();
}

CALLSET_ENTRY (racetrack, amode_stop, test_start, stop_game)
{
	dbprintf ("racetrack: amode_stop, test_start, stop_game entry\n");
	if (racetrack_state != RACETRACK_CALIBRATE) {
		racetrack_car_return();
	}
}

CALLSET_ENTRY (racetrack, start_ball, end_ball)
{
	if (racetrack_state != RACETRACK_CALIBRATE) {
		racetrack_car_return();
	}

}

CALLSET_ENTRY (racetrack, diagnostic_check)
{
	if (!feature_config.enable_racetrack) {
		dbprintf ("racetrack: RACETRACK DISABLED BY ADJUSTMENT\n");

		diag_post_error ("RACETRACK DISABLED\nBY ADJUSTMENT\n", PAGE);
		return;
	}

	if (!racetrack_calibration_attempted) {
		return;
	}

	dbprintf ("calibration result: %d - %s", racetrack_last_calibration_result_code, mech_racetrack_diag_messages[racetrack_last_calibration_result_code]); // No trailing \n as diag message contains one already
	if (racetrack_last_calibration_result_code != CC_SUCCESS) {
		audit_increment (&feature_audits.racetrack_errors);
		diag_post_error (mech_racetrack_diag_messages[racetrack_last_calibration_result_code], PAGE);
	}
}

CALLSET_ENTRY (racetrack, start_game) {
	dbprintf ("racetrack: start_game entry\n");

	if (!racetrack_calibration_attempted) {
		dbprintf ("starting racetrack calibration\n");
		racetrack_calibrate();
	}

	dbprintf ("racetrack: start_game exit\n");
}
