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
#include <window.h>
#include <test.h>
#include <corvette/racetrack.h>

// FIXME - font_render_string_right vertically offsets the text by 1 pixel (downwards)
// When this is removed removed all references to FR_WORKAROUND
#define FRSR_WORKAROUND 1

enum {
	FIRST_TEST = 0,
	CALIBRATE = FIRST_TEST,
	RACE,
	CAR_TEST,
	CAR_RETURN,
	FLOAT,
	LAST_TEST = FLOAT
} racetrack_test_command;

char *racetrack_test_short_names[] = {
	"CALIBRATE",
	"RACE",
	"CAR TEST",
	"CAR RETURN",
	"FLOAT"
};

char *racetrack_state_codes[] = {
	"INI",
	"CAL",
	"FLT",
	"RDY",
	"TES",
	"RTN",
	"RAC"
};

char *racetrack_lane_state_codes[] = {
	"STP",
	"SEK",
	"RTN",
	"CAL"
};

// TODO add more calibration result messages
// error messages, see enum mech_racetrack_calibration_codes;
char *mech_racetrack_calibration_messages[] = {
	"NOT CALIBRATED",
	"CHECK RACETRACK",
	"CHECK LEFT TRACK",
	"CALIBRATED O.K."
};

extern /*__fastram__*/ enum mech_racetrack_state racetrack_state;
extern U8 racetrack_calibrated;
extern U8 racetrack_calibration_attempted;
extern enum mech_racetrack_calibration_codes racetrack_last_calibration_result_code;

extern __fastram__ U8 racetrack_encoder_mask;
extern racetrack_lane_t racetrack_lanes[RACETRACK_LANES];

U8 position_step_index;
U8 position_steps[] = {
	1,
	5,
	10,
	25
};
#define MAX_POSITION_STEPS 4


void racetrack_test_init (void)
{
	racetrack_test_command = CALIBRATE;
	position_step_index = 0;
}

void racetrack_draw_test_title(void) {
	font_render_string_center (&font_mono5, 64, 2, "RACETRACK TEST");
	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 5);
}

// 21 characters wide max when using font_mono5
// ~30 characters when using font_var5

#define LINE_1_Y 7
#define LINE_2_Y 13
#define LINE_3_Y 19

void racetrack_test_draw (void)
{
	dmd_alloc_low_clean ();

	racetrack_draw_test_title();

	//123456789012345678901234567890
	// LS:sss/nnn S:sss RS:sss/nnn

	sprintf ("LS:%s/%d S:%s RS:%s/%d",
		racetrack_lane_state_codes[racetrack_lanes[LANE_LEFT].state],
		racetrack_lanes[LANE_LEFT].car_position,
		racetrack_state_codes[racetrack_state],
		racetrack_lane_state_codes[racetrack_lanes[LANE_RIGHT].state],
		racetrack_lanes[LANE_RIGHT].car_position
	);
	font_render_string_center (&font_var5, 64, LINE_1_Y + 2, sprintf_buffer);

	sprintf ("L:%c%c%c%c%c%c %ld",
		switch_poll_logical (SW_LEFT_RACE_ENCODER) ? 'E' : '-',
		switch_poll_logical (SW_LEFT_RACE_START) ? 'R' : '-',
		(racetrack_encoder_mask & RT_EM_SEEN_LEFT) > 0 ? 'Y' : 'N',
		(racetrack_encoder_mask & RT_EM_PREVIOUS_STATE_LEFT) > 0 ? '1' : '0',
		(racetrack_encoder_mask & RT_EM_STALLED_LEFT) > 0 ? 'S' : '-',
		(racetrack_encoder_mask & RT_EM_END_OF_TRACK_LEFT) > 0 ? 'M' : '-',
		racetrack_lanes[LANE_LEFT].encoder_count
	);
	font_render_string_left (&font_var5, 0, LINE_3_Y, sprintf_buffer);

	sprintf ("R:%c%c%c%c%c%c %ld",
		switch_poll_logical (SW_RIGHT_RACE_ENCODER) ? 'E' : '-',
		switch_poll_logical (SW_RIGHT_RACE_START) ? 'R' : '-',
		(racetrack_encoder_mask & RT_EM_SEEN_RIGHT) > 0 ? 'Y' : 'N',
		(racetrack_encoder_mask & RT_EM_PREVIOUS_STATE_RIGHT) > 0 ? '1' : '0',
		(racetrack_encoder_mask & RT_EM_STALLED_RIGHT) > 0 ? 'S' : '-',
		(racetrack_encoder_mask & RT_EM_END_OF_TRACK_RIGHT) > 0 ? 'M' : '-',
		racetrack_lanes[LANE_RIGHT].encoder_count
	);
	font_render_string_right (&font_var5, 0, LINE_3_Y - FRSR_WORKAROUND, sprintf_buffer);

	switch (racetrack_test_command) {
		case CALIBRATE:
			if (racetrack_state == RACETRACK_CALIBRATE) {
				font_render_string_center(&font_var5, 64, LINE_2_Y + 2, "CALIBRATING");
			} else {
				//dbprintf("calibration result: %d\n", racetrack_last_calibration_result_code);
				font_render_string_center(&font_var5, 64, LINE_2_Y + 2, mech_racetrack_calibration_messages[racetrack_last_calibration_result_code]);
			}
		break;
		case RACE:
			if (racetrack_state == RACETRACK_RACE) {
				sprintf("LDP:%d STEP: %d RDP:%d",
					racetrack_lanes[LANE_LEFT].desired_car_position,
					position_steps[position_step_index],
					racetrack_lanes[LANE_RIGHT].desired_car_position
				);
				font_render_string_center(&font_var5, 64, LINE_2_Y + 2, sprintf_buffer);
			}
		default:
			// shut the compiler up
		break;
	}

	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 25);

	sprintf(racetrack_test_short_names[racetrack_test_command]);
	font_render_string_left (&font_mono5, 0, 27, sprintf_buffer);

	dmd_show_low ();
}


void racetrack_test_thread (void)
{
	for (;;)
	{

		task_sleep (TIME_33MS);

		racetrack_test_draw ();
	}
}

void racetrack_test_left (void)
{
	switch (racetrack_test_command) {
		case RACE:
			if (racetrack_state != RACETRACK_RACE) {
				break;
			}
			racetrack_set_desired_car_position(LANE_LEFT, racetrack_lanes[LANE_LEFT].desired_car_position + position_steps[position_step_index]);
		break;
		case CAR_TEST:
			if (racetrack_state != RACETRACK_CAR_TEST) {
				break;
			}
			disable_interrupts();
			if (racetrack_lanes[LANE_LEFT].state == LANE_STOP) {
				racetrack_lanes[LANE_LEFT].state = LANE_CALIBRATE;
				racetrack_encoder_mask &= ~RT_EM_SEEN_LEFT;
				racetrack_encoder_mask &= ~RT_EM_STALLED_LEFT;
			} else {
				racetrack_lanes[LANE_LEFT].state = LANE_STOP;
			}
			enable_interrupts();
		break;
		default:
			// shut the compiler up
		break;
	}
}

void racetrack_test_right (void)
{
	switch (racetrack_test_command) {
		case RACE:
			if (racetrack_state != RACETRACK_RACE) {
				break;
			}
			racetrack_set_desired_car_position(LANE_RIGHT, racetrack_lanes[LANE_RIGHT].desired_car_position + position_steps[position_step_index]);
		break;
		case CAR_TEST:
			if (racetrack_state != RACETRACK_CAR_TEST) {
				break;
			}
			disable_interrupts();
			if (racetrack_lanes[LANE_RIGHT].state == LANE_STOP) {
				racetrack_lanes[LANE_RIGHT].state = LANE_CALIBRATE;
				racetrack_encoder_mask &= ~RT_EM_SEEN_RIGHT;
				racetrack_encoder_mask &= ~RT_EM_STALLED_RIGHT;
			} else {
				racetrack_lanes[LANE_RIGHT].state = LANE_STOP;
			}
			enable_interrupts();
		break;

		default:
			// shut the compiler up
		break;
	}
}

void racetrack_test_start (void)
{
	switch (racetrack_test_command) {
		case RACE:
			if (racetrack_state != RACETRACK_RACE) {
				break;
			}

			position_step_index++;
			if (position_step_index >= MAX_POSITION_STEPS) {
				position_step_index = 0;
			}
		break;

		default:
			// shut the compiler up
		break;
	}
}

void racetrack_test_up (void)
{
	if (racetrack_test_command < LAST_TEST)
		racetrack_test_command++;
}

void racetrack_test_down (void)
{
	if (racetrack_test_command > FIRST_TEST)
		racetrack_test_command--;
}

void racetrack_test_escape (void) {

	// allow calibrate to continue, but stop everything else
	if (racetrack_state != RACETRACK_CALIBRATE) {
		racetrack_car_return();
	}

	window_pop();
}

void racetrack_test_enter (void)
{
	sound_send (SND_TEST_ENTER);
  	switch (racetrack_test_command)
  	{
  		case CALIBRATE:
 			dbprintf ("racetrack_test_enter: starting 'calibrate'\n");
 			racetrack_calibrate();
  		break;

  		case RACE:
  			racetrack_race();
  		break;

  		case CAR_TEST:
  			racetrack_car_test();
  		break;

  		case CAR_RETURN:
  			racetrack_car_return();
  		break;

  		case FLOAT:
 			dbprintf ("racetrack_test_enter: starting 'float'\n");
  			racetrack_float();
  		break;

  	}

}


struct window_ops corvette_racetrack_test_window = {
	DEFAULT_WINDOW,
	.init = racetrack_test_init,
	.draw = racetrack_test_draw,
	.up = racetrack_test_up,
	.down = racetrack_test_down,
	.enter = racetrack_test_enter,
	.escape = racetrack_test_escape,
	.left = racetrack_test_left,
	.right = racetrack_test_right,
	.start = racetrack_test_start,
	.thread = racetrack_test_thread,
};


struct menu corvette_racetrack_test_item = {
	.name = "RACETRACK TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &corvette_racetrack_test_window, NULL } },
};
