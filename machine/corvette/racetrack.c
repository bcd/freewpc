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
 * There are 4 switches that are used by the racetrack:
 * 4 optos
 * one for each car's encoder wheel, used to determine if the car is stationary or moving.
 * one for each car's at-start-of-track, used to determine if the car is at the start of the track.
 *
 * There are about 727 encoder on/off cycles from the start to the end of the track.
 * If you count encoder cycles from the start of the track you can determine where on the track the car is.
 *
 * There are few lamps:
 * 8 lights arranged in a tree, 4 amber, 2 green, 2 red.
 * 2 lights under the start of the cars in vpinmame are activiated by the race enable solenoids and
 *   do not exist on the real machine.
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
 * Lamps:
 * 81: Right Tree Red
 * 82: Left Tree Red
 * 83: Tree Bottom Yellow
 * 84: Tree Top Yellow
 * 85: Right Tree Green
 * 86: Left Tree Green
 *
 *
 * The racetrack needs to be calibrated.
 * The calibration routine needs to reset each car to the start of the racetrack
 * Then it needs to drive each car in turn to the end of the racetrack
 * Then finally it needs to reset them again to the start of the racetrack.
 *
 * From the start to the end of the track = 670 encoder cycles = 100%. 670 / 100 = 6.7 encoder cycles per percent.
 * Thus, to move a car from the start to reach 50% we must drive it 6.7 * 50 = 335 cycles
 *
 * Here's what I found during experimentation:
 *
 * The schedule needs to be set to run the RTT frequently otherwise it's not possible to
 * count the race encoder switch transitions when the cars are moving quickly.
 *
 * The Williams ROM uses a fixed value of 670 switch transitions for the length of the track, as
 * displayed in it's test menu.  It doesn't care if the car reaches the end of the track or not.
 * It does not measure the length of the track by detecting stalled cars.
 *
 * The Williams ROM does not initialise the racetrack at power-up, it does it at the start of the first game.
 * This may be to reduce wear or power consumption.
 */
#include <freewpc.h>
#include <diag.h>
#include <corvette/racetrack.h>

// racetrack schedule as defined in corvetee.sched.
// it needs to be called frequently to control the speed of the cars
// and if stall detection is used to detect cars that have stalled so that the drive belt doesn't slip.
#define RACETRACK_SCHEDULE 16

#define RACETRACK_SPEED_FASTEST 1
#define RACETRACK_SPEED_FAST    2
#define RACETRACK_SPEED_MEDIUM  3
#define RACETRACK_SPEED_SLOW    4
#define RACETRACK_SPEED_SLOWEST 5

//
// Lane state
//

racetrack_lane_t racetrack_lanes[RACETRACK_LANES];

// 8-bit mask, first 4 bit left track, second 4 bits right track
__fastram__ U8 racetrack_encoder_mask; // See RT_EM_* defines
__fastram__ U8 racetrack_encoder_previous_mask;


#define LANE_STALL_IGNORE_TICKS (160 / RACETRACK_SCHEDULE) // check for stall after X ms
#define LANE_STALL_DETECT_TICKS (200 / RACETRACK_SCHEDULE) // check for stall every X ms
U8 racetrack_stall_ticks_remaining;
U8 racetrack_stall_ignore_ticks_remaining;

//
// Calibration
//

#define RACETRACK_LENGTH_MAX 727
#define RACETRACK_LENGTH_LIMIT 715 // used to prevent belt slipping when driving backwards in case of faulty start-of-track opto
#define RACETRACK_LENGTH_USABLE 670 // used to prevent belt slipping when driving forwards.


#define RACETRACK_CALIBRATE_TICKS (1024 / RACETRACK_SCHEDULE)
#define RACETRACK_CALIBRATE_TIMEOUT_COUNTER 15

U8 racetrack_calibrate_ticks; // ticks remaining is initialised to this value, which changes depending on requirements
U8 racetrack_calibrate_ticks_remaining;
U8 racetrack_calibrate_counter;

U8 racetrack_calibrated;
U8 racetrack_calibration_attempted;

/* Mode that drives the racetrack state machine */
enum mech_racetrack_state racetrack_state;
enum mech_racetrack_state racetrack_previous_state;

enum mech_racetrack_calibrate_state racetrack_calibrate_state;
enum mech_racetrack_calibrate_state racetrack_calibrate_previous_state;

// FIXME these are somewhat similar to mech_racetrack_calibration_messages but with line feeds instead of spaces
char *mech_racetrack_diag_messages[] = {
	"NOT CALIBRATED\n",
	"CHECK RACETRACK\n",
	"CHECK LEFT TRACK\n",
	"CHECK RIGHT TRACK\n",
	"CALIBRATED O.K.\n"
};

enum mech_racetrack_calibration_codes racetrack_last_calibration_result_code;

U16 encoder_difference;
U8 new_speed;

/**
 * @param lane_number See LANE_* defines.
 */
void racetrack_process_lane(U8 lane_number) {

	//
	// handle transitions
	//

	switch(racetrack_lanes[lane_number].state) {
		case LANE_RETURN:
			//
			// detect at-start-of-track
			//
			if (switch_poll_logical(racetrack_lanes[lane_number].start_switch)) {
				racetrack_lanes[lane_number].state = LANE_STOP;
			}
		break;
		case LANE_SEEK:
			if (racetrack_lanes[lane_number].encoder_count >= racetrack_lanes[lane_number].desired_encoder_count) {
				racetrack_lanes[lane_number].state = LANE_STOP;
			}
		break;
		default:
			// shut the compiler up
		break;

	}

	switch(racetrack_lanes[lane_number].state) {
		case LANE_SEEK:
		case LANE_CALIBRATE:
			//
			// detect end-of-track (usable)
			//

			if (racetrack_lanes[lane_number].encoder_count > RACETRACK_LENGTH_USABLE) {
				racetrack_lanes[lane_number].state = LANE_STOP;
				if (lane_number == LANE_LEFT) {
					racetrack_encoder_mask |= RT_EM_END_OF_TRACK_LEFT;
				} else {
					racetrack_encoder_mask |= RT_EM_END_OF_TRACK_RIGHT;
				}
				break;
			}
		case LANE_RETURN:
			//
			// detect end-of-track (limit)
			//
			// If the cars are pushed all the way to the end of the track, and then reversed we
			// should wait for the start-of-track opto to be switched on, if it's not switched
			// on and we should stop if the encoder has registered almost all of the track.
			if (racetrack_lanes[lane_number].encoder_count > RACETRACK_LENGTH_LIMIT) {
				racetrack_lanes[lane_number].state = LANE_STOP;
				break;
			}

			//
			// detect stall
			//

			// don't look for a stall condition if we've only just started to move, wait a bit first
			if (racetrack_stall_ignore_ticks_remaining > 0) {
				racetrack_stall_ignore_ticks_remaining--;
				break;
			}

			racetrack_stall_ticks_remaining--;
			if (racetrack_stall_ticks_remaining > 0) {
				break;
			}

			racetrack_stall_ticks_remaining = LANE_STALL_DETECT_TICKS;

			if (
				(lane_number == LANE_LEFT && ((racetrack_encoder_mask & RT_EM_SEEN_LEFT) == 0)) ||
				(lane_number == LANE_RIGHT && ((racetrack_encoder_mask & RT_EM_SEEN_RIGHT) == 0))
			) {
				racetrack_lanes[lane_number].state = LANE_STOP; // stalled!
				if (lane_number == LANE_LEFT) {
					racetrack_encoder_mask |= RT_EM_STALLED_LEFT;
				} else {
					racetrack_encoder_mask |= RT_EM_STALLED_RIGHT;
				}
			} else {
				// clear the encoder seen flag
				if (lane_number == LANE_LEFT) {
					racetrack_encoder_mask &= ~RT_EM_SEEN_LEFT;
				} else {
					racetrack_encoder_mask &= ~RT_EM_SEEN_RIGHT;
				}
			}
		break;
		default:
			// shut the compiler up
		break;

	}

	//
	// handle state
	//

	switch (racetrack_lanes[lane_number].state) {
		case LANE_STOP:
			sol_disable(racetrack_lanes[lane_number].solenoid);
		break;

		case LANE_SEEK:
			// adjust the speed depending on the difference between desired position and actual position
			// if they're close, use a slow speed, if they're far apart use a fast speed.
			// using a fast speed to move a short distance means the car will over-run it's desired position

			encoder_difference = racetrack_lanes[lane_number].desired_encoder_count - racetrack_lanes[lane_number].encoder_count;
			new_speed = racetrack_lanes[lane_number].speed;
			if (encoder_difference > 50) {
				new_speed = RACETRACK_SPEED_FASTEST;
			} else if (encoder_difference > 25) {
				new_speed = RACETRACK_SPEED_MEDIUM;
			} else {
				new_speed = RACETRACK_SPEED_SLOWEST;
			}
			if (new_speed < racetrack_lanes[lane_number].speed) {
				// go faster
				racetrack_lanes[lane_number].speed_ticks_remaining = 1; // cause the solenoid to enable now (see below)
				racetrack_lanes[lane_number].speed = new_speed;
			} else if (new_speed > racetrack_lanes[lane_number].speed) {
				// go slower
				racetrack_lanes[lane_number].speed_ticks_remaining = 2; // cause the solenoid to disable now (see below)
				racetrack_lanes[lane_number].speed = new_speed;
			}
			// follow though ...
		case LANE_CALIBRATE:
		case LANE_RETURN:
			// turn the solenoid on once every 'speed' ticks
			racetrack_lanes[lane_number].speed_ticks_remaining--;
			if (racetrack_lanes[lane_number].speed_ticks_remaining == 0) {
				racetrack_lanes[lane_number].speed_ticks_remaining = racetrack_lanes[lane_number].speed;

				if (!(
					((racetrack_encoder_mask & RT_EM_STALLED_LEFT) > 0 && lane_number == LANE_LEFT) ||
					((racetrack_encoder_mask & RT_EM_STALLED_RIGHT) > 0 && lane_number == LANE_RIGHT)
				)) {
					// never enable the solenoid if the car has stalled
					sol_enable(racetrack_lanes[lane_number].solenoid);
				}
			} else {
				sol_disable(racetrack_lanes[lane_number].solenoid);
			}
		break;
		default:
			// shut the compiler up
		break;
	}
}

void set_lane_speed(U8 lane_number, U8 speed) {
	racetrack_lanes[lane_number].speed = speed;
	racetrack_lanes[lane_number].speed_ticks_remaining = racetrack_lanes[lane_number].speed;
	racetrack_stall_ticks_remaining = LANE_STALL_DETECT_TICKS;
	racetrack_stall_ignore_ticks_remaining = LANE_STALL_IGNORE_TICKS;
}

void racetrack_reset_track_state(void) {
	racetrack_encoder_mask = 0;
	racetrack_stall_ticks_remaining = LANE_STALL_DETECT_TICKS;

	racetrack_lanes[LANE_LEFT].state = LANE_STOP;
	racetrack_lanes[LANE_RIGHT].state = LANE_STOP;

	racetrack_lanes[LANE_LEFT].encoder_count = 0;
	racetrack_lanes[LANE_RIGHT].encoder_count = 0;

	racetrack_lanes[LANE_LEFT].desired_encoder_count = 0;
	racetrack_lanes[LANE_RIGHT].desired_encoder_count = 0;

	racetrack_lanes[LANE_LEFT].desired_position = 0;
	racetrack_lanes[LANE_RIGHT].desired_position = 0;

	// cause the speed tick tester to re-evaluate on the next tick.
	racetrack_lanes[LANE_LEFT].speed_ticks_remaining = 1;
	racetrack_lanes[LANE_RIGHT].speed_ticks_remaining = 1;

	if (switch_poll_logical (SW_LEFT_RACE_ENCODER)) {
		racetrack_encoder_mask |= RT_EM_PREVIOUS_STATE_LEFT;
	}
	if (switch_poll_logical (SW_RIGHT_RACE_ENCODER)) {
		racetrack_encoder_mask |= RT_EM_PREVIOUS_STATE_RIGHT;
	}
}

void racetrack_update_track_state(void) {
	racetrack_process_lane(LANE_LEFT);
	racetrack_process_lane(LANE_RIGHT);
}



void racetrack_state_calibrate_exit(void) {
	racetrack_calibration_attempted = TRUE;

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
	// reset flags
	racetrack_calibrated = FALSE;
	racetrack_calibration_attempted = FALSE;
	racetrack_last_calibration_result_code = CC_NOT_CALIBRATED;
	global_flag_off(GLOBAL_FLAG_RACETRACK_WORKING);

	// initialise the first calibration state

	racetrack_calibrate_counter = 0;
	racetrack_calibrate_previous_state = RT_CALIBRATE_INITIALISE;
	racetrack_calibrate_state = RT_CALIBRATE_CAR_RETURN;

	racetrack_calibrate_ticks = RACETRACK_CALIBRATE_TICKS;
	racetrack_calibrate_ticks_remaining = racetrack_calibrate_ticks;
}

void racetrack_state_calibrate_run(void) {

	//
	// handle transition
	//

	if (racetrack_calibrate_state != racetrack_calibrate_previous_state) {
		racetrack_calibrate_counter = 0;
		racetrack_calibrate_ticks_remaining = 1; // process the state this tick!
		racetrack_calibrate_previous_state = racetrack_calibrate_state;

		// process new state
		switch(racetrack_calibrate_state) {
			case RT_CALIBRATE_CAR_RETURN:

				/**
				 * The first time this state is hit the cars can be anywhere on the track
				 *
				 * The goal is to return both cars to the start position
				 */

				sol_disable(SOL_RACE_DIRECTION); // backwards
				racetrack_reset_track_state();

				set_lane_speed(LANE_LEFT, RACETRACK_SPEED_FASTEST);
				set_lane_speed(LANE_RIGHT, RACETRACK_SPEED_FASTEST);

				racetrack_lanes[LANE_LEFT].state = LANE_RETURN;
				racetrack_lanes[LANE_RIGHT].state = LANE_RETURN;
			break;
			case RT_CALIBRATE_LEFT_CAR_FORWARDS:
				/**
				 * The first time this state is hit both cars will be at the start position.
				 *
				 * The goal is to drive the left car to the end of the track and stop
				 */
				sol_enable(SOL_RACE_DIRECTION); // forwards
				racetrack_reset_track_state();
				racetrack_lanes[LANE_LEFT].state = LANE_CALIBRATE;
			break;
			case RT_CALIBRATE_RIGHT_CAR_FORWARDS:
				/**
				 * The first time this state is hit the left car will be at the end of the track and the
				 * right car will be at the start of the track.
				 *
				 * The goal is to drive the right car to the end of the track and stop
				 */
				sol_enable(SOL_RACE_DIRECTION); // forwards
				racetrack_lanes[LANE_RIGHT].state = LANE_CALIBRATE;
			break;
			case RT_CALIBRATE_LEFT_CAR_RETURN:
				/**
				 * The first time this state is hit both cars will be at the end of the track
				 *
				 * The goal is to drive the left car to the end of the start of the track and stop
				 */
				sol_disable(SOL_RACE_DIRECTION); // backwards
				racetrack_reset_track_state();
				racetrack_lanes[LANE_LEFT].state = LANE_RETURN;
			break;
			case RT_CALIBRATE_RIGHT_CAR_RETURN:
				/**
				 * The first time this state is hit the left car will be at the start of the track and the
				 * right car will be at the end of the track
				 *
				 * The goal is to drive the right car to the end of the start of the track and stop
				 */
				sol_disable(SOL_RACE_DIRECTION); // backwards
				racetrack_lanes[LANE_RIGHT].state = LANE_RETURN;
			break;
			default:
				// shut the compiler up
			break;
		}
	}

	racetrack_calibrate_ticks_remaining--;
	if (racetrack_calibrate_ticks_remaining != 0) {
		// TODO check if we need to update the SOL_RACE_DIRECTION more frequently, seems to work so far for me on my machine
		return;
	}
	// reset the timer
	racetrack_calibrate_ticks_remaining = RACETRACK_CALIBRATE_TICKS;

	// we've waited long enough now until we should check things again.

	racetrack_calibrate_counter++;


	switch(racetrack_calibrate_state) {
		case RT_CALIBRATE_CAR_RETURN:
			sol_disable(SOL_RACE_DIRECTION); // backwards

			if (switch_poll_logical (SW_LEFT_RACE_START) && switch_poll_logical (SW_RIGHT_RACE_START)) {
				racetrack_calibrate_state = RT_CALIBRATE_LEFT_CAR_FORWARDS;
				break;
			}

			// both cars should be moving backwards

			if (racetrack_calibrate_counter >= RACETRACK_CALIBRATE_TIMEOUT_COUNTER) {
				// both start of track optos should be on
				racetrack_calibration_failed(CC_CHECK_RACETRACK);
				break;
			}
		break;

		case RT_CALIBRATE_LEFT_CAR_FORWARDS:
			sol_enable(SOL_RACE_DIRECTION); // forwards

			if ((racetrack_encoder_mask & RT_EM_END_OF_TRACK_LEFT) != 0) {
				racetrack_calibrate_state = RT_CALIBRATE_RIGHT_CAR_FORWARDS;
				break;
			}

			if (racetrack_calibrate_counter >= RACETRACK_CALIBRATE_TIMEOUT_COUNTER) {
				racetrack_calibration_failed(CC_CHECK_LEFT_TRACK);
				break;
			}
		break;

		case RT_CALIBRATE_RIGHT_CAR_FORWARDS:
			sol_enable(SOL_RACE_DIRECTION); // forwards
			if ((racetrack_encoder_mask & RT_EM_END_OF_TRACK_RIGHT) != 0) {
				racetrack_calibrate_state = RT_CALIBRATE_LEFT_CAR_RETURN;
				break;
			}

			if (racetrack_calibrate_counter >= RACETRACK_CALIBRATE_TIMEOUT_COUNTER) {
				racetrack_calibration_failed(CC_CHECK_RIGHT_TRACK);
				break;
			}
		break;

		case RT_CALIBRATE_LEFT_CAR_RETURN:
			sol_disable(SOL_RACE_DIRECTION); // backwards

			if (switch_poll_logical (SW_LEFT_RACE_START)) {
				racetrack_calibrate_state = RT_CALIBRATE_RIGHT_CAR_RETURN;
				break;
			}

			// left car should be moving backwards

			if (racetrack_calibrate_counter >= RACETRACK_CALIBRATE_TIMEOUT_COUNTER) {
				// both start of track optos should be on
				racetrack_calibration_failed(CC_CHECK_LEFT_TRACK);
				break;
			}
		break;

		case RT_CALIBRATE_RIGHT_CAR_RETURN:
			sol_disable(SOL_RACE_DIRECTION); // backwards

			if (switch_poll_logical (SW_RIGHT_RACE_START)) {
				racetrack_calibration_complete();
				break;
			}

			// right car should be moving backwards

			if (racetrack_calibrate_counter >= RACETRACK_CALIBRATE_TIMEOUT_COUNTER) {
				// both start of track optos should be on
				racetrack_calibration_failed(CC_CHECK_RIGHT_TRACK);
				break;
			}
		break;
		default:
			// shut the compiler up
		break;

	}
}

static inline void racetrack_state_float_enter(void) {
	racetrack_reset_track_state();
	racetrack_lanes[LANE_LEFT].state = LANE_STOP;
	racetrack_lanes[LANE_RIGHT].state = LANE_STOP;
}

static inline void racetrack_state_float_run(void) {
	sol_disable(SOL_RACE_DIRECTION); // keep it disabled, just in case, TODO look at schematics to see if we need this
}

static inline void racetrack_state_ready_enter(void) {
	racetrack_reset_track_state();
	racetrack_lanes[LANE_LEFT].state = LANE_STOP;
	racetrack_lanes[LANE_RIGHT].state = LANE_STOP;
}

static inline void racetrack_state_ready_run(void) {
	sol_disable(SOL_RACE_DIRECTION); // keep it disabled, just in case, TODO look at schematics to see if we need this
}


static inline void racetrack_state_car_return_enter(void) {

	sol_disable(SOL_RACE_DIRECTION);
	racetrack_reset_track_state();

	set_lane_speed(LANE_LEFT, RACETRACK_SPEED_MEDIUM);
	set_lane_speed(LANE_RIGHT, RACETRACK_SPEED_MEDIUM);

	racetrack_lanes[LANE_LEFT].state = LANE_RETURN;
	racetrack_lanes[LANE_RIGHT].state = LANE_RETURN;
}

static inline void racetrack_state_car_return_run(void) {

	if (switch_poll_logical (SW_LEFT_RACE_START) && switch_poll_logical (SW_RIGHT_RACE_START)) {
		racetrack_state = RACETRACK_READY;
		return;
	}

	sol_disable(SOL_RACE_DIRECTION);

	// TODO check lane 'stalled' bits, if either one gets set then disable the racetrack?
}

static inline void racetrack_state_car_test_enter(void) {

	sol_enable(SOL_RACE_DIRECTION);

	racetrack_reset_track_state();

	set_lane_speed(LANE_LEFT, RACETRACK_SPEED_MEDIUM);
	set_lane_speed(LANE_RIGHT, RACETRACK_SPEED_MEDIUM);

	racetrack_lanes[LANE_LEFT].state = LANE_STOP;
	racetrack_lanes[LANE_RIGHT].state = LANE_STOP;

}

static inline void racetrack_state_car_test_run(void) {
	// in this mode, the test menu should:
	// a) change the lane states between LANE_STOPPED and LANE_CALIBRATE
	// b) set the desired lane speeds

	sol_enable(SOL_RACE_DIRECTION);
}

static inline void racetrack_state_race_enter(void) {

	sol_enable(SOL_RACE_DIRECTION);

	racetrack_reset_track_state();

	// lane is speed controlled by lane state, no need to set it here

	racetrack_lanes[LANE_LEFT].state = LANE_SEEK;
	racetrack_lanes[LANE_RIGHT].state = LANE_SEEK;

}

static inline void racetrack_state_race_run(void) {
	// In this mode the game-code should increase the desired_encoder_count's value
	// the RTT will advance the cars so they reach the desired encoder count.
	sol_enable(SOL_RACE_DIRECTION);

	/* move this into lane state? */
	if (racetrack_lanes[LANE_LEFT].desired_encoder_count > racetrack_lanes[LANE_LEFT].encoder_count) {
		racetrack_lanes[LANE_LEFT].state = LANE_SEEK;
	}
	if (racetrack_lanes[LANE_RIGHT].desired_encoder_count > racetrack_lanes[LANE_RIGHT].encoder_count) {
		racetrack_lanes[LANE_RIGHT].state = LANE_SEEK;
	}
}

// This RTT needs to be FAST as it's scheduled frequently
void corvette_racetrack_encoder_rtt (void) {

	racetrack_encoder_previous_mask = racetrack_encoder_mask;
	// check the optos and update the bitmask

	// left lane
	if (rt_switch_poll (SW_LEFT_RACE_ENCODER)) {
		racetrack_encoder_mask |= (RT_EM_PREVIOUS_STATE_LEFT);
	} else {
		racetrack_encoder_mask &= ~RT_EM_PREVIOUS_STATE_LEFT;
	}

	if ((racetrack_encoder_mask & RT_EM_PREVIOUS_STATE_LEFT) != (racetrack_encoder_previous_mask & RT_EM_PREVIOUS_STATE_LEFT)) {
		// encoder state changed
		racetrack_lanes[LANE_LEFT].encoder_count++;
		racetrack_encoder_mask |= (RT_EM_SEEN_LEFT);
	}


	// right lane
	if (rt_switch_poll (SW_RIGHT_RACE_ENCODER)) {
		racetrack_encoder_mask |= (RT_EM_PREVIOUS_STATE_RIGHT);
	} else {
		racetrack_encoder_mask &= ~RT_EM_PREVIOUS_STATE_RIGHT;
	}

	if ((racetrack_encoder_mask & RT_EM_PREVIOUS_STATE_RIGHT) != (racetrack_encoder_previous_mask & RT_EM_PREVIOUS_STATE_RIGHT)) {
		// encoder state changed
		racetrack_lanes[LANE_RIGHT].encoder_count++;
		racetrack_encoder_mask |= (RT_EM_SEEN_RIGHT);
	}
}

void corvette_racetrack_rtt (void) {

	racetrack_update_track_state();

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

		case RACETRACK_CAR_TEST:
			if (racetrack_previous_state != racetrack_state) {
				racetrack_state_car_test_enter();
			} else {
				racetrack_state_car_test_run();
			}
		break;


		case RACETRACK_READY:
			if (racetrack_previous_state != racetrack_state) {
				racetrack_state_ready_enter();
			} else {
				racetrack_state_ready_run();
			}
		break;

		case RACETRACK_RACE:
			if (racetrack_previous_state != racetrack_state) {
				racetrack_state_race_enter();
			} else {
				racetrack_state_race_run();
			}
		break;

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

	// initialise the lanes
	memset(racetrack_lanes, 0, sizeof(racetrack_lanes));

	racetrack_lanes[LANE_LEFT].solenoid = SOL_LEFT_RACE_ENABLE;
	racetrack_lanes[LANE_RIGHT].solenoid = SOL_RIGHT_RACE_ENABLE;

	racetrack_lanes[LANE_LEFT].start_switch = SW_LEFT_RACE_START;
	racetrack_lanes[LANE_RIGHT].start_switch = SW_RIGHT_RACE_START;

	set_lane_speed(LANE_LEFT, RACETRACK_SPEED_MEDIUM);
	set_lane_speed(LANE_RIGHT, RACETRACK_SPEED_MEDIUM);

	// set flags
	racetrack_calibrated = FALSE;
	racetrack_calibration_attempted = FALSE;
	racetrack_last_calibration_result_code = CC_NOT_CALIBRATED;

	// set states
	racetrack_reset_track_state();
	racetrack_state = RACETRACK_INITIALIZE;
	racetrack_previous_state = RACETRACK_INITIALIZE;
	global_flag_off(GLOBAL_FLAG_RACETRACK_WORKING);

	enable_interrupts();
}

void racetrack_enter_state(enum mech_racetrack_state new_state) {
	U8 allow_state_change = FALSE;
	switch (new_state) {
 		case RACETRACK_RACE:
			allow_state_change = racetrack_calibrated && racetrack_state == RACETRACK_READY;
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

/*
 *
 * User API
 *
 */

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

void racetrack_car_test(void) {
	racetrack_enter_state(RACETRACK_CAR_TEST);
}

/**
 * Get the car position as a percentage.
 */
U8 racetrack_get_actual_car_position(U8 lane_number) {
	U8 percentage = ((racetrack_lanes[lane_number].encoder_count >> 1) * (U16)100) / (RACETRACK_LENGTH_USABLE >> 1);
	return percentage > 100 ? 100 : percentage;
}

void racetrack_set_desired_car_position(U8 lane, U8 position_percentage) {

	if (position_percentage > 100) {
		position_percentage = 100;
	}

	if (position_percentage <= racetrack_lanes[lane].desired_position) {
		return; // only recalculate encoder position if necessary
	}

	racetrack_lanes[lane].desired_position = position_percentage;


	disable_interrupts();
	// same as RACETRACK_LENGTH_USABLE / 100 * position_percentage, but without needing non-integer maths and staying in the bounds of a U16
	racetrack_lanes[lane].desired_encoder_count = ((RACETRACK_LENGTH_USABLE >> 1) * position_percentage) / (U16)50;
	enable_interrupts();
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
