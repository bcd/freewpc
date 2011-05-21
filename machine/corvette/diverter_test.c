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

/* diverter Test :
 * - Enable/Disable solenoids.
 * - Calibrate the left right and center positions.
 * - Shake engine.
 */

#include <freewpc.h>
#include <window.h>
#include <test.h>
#include <diverter.h>

// FIXME - font_render_string_right vertically offsets the text by 1 pixel (downwards)
// When this is removed removed all references to FR_WORKAROUND
#define FRSR_WORKAROUND 1

enum {
	FIRST_TEST = 0,
	START = FIRST_TEST,
	STOP,
	LAST_TEST = STOP
} diverter_test_command;

char *diverter_test_short_names[] = {
	"START",
	"STOP"
};


extern __fastram__ S8 diverter_timer;
extern __fastram__ U8 fliptronic_powered_coil_outputs;

void diverter_test_init (void)
{
	diverter_test_command = START;
}

void diverter_draw_test_title(void) {
	font_render_string_center (&font_mono5, 64, 2, "DIVERTER TEST");
	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 5);
}

#define LINE_1_Y 7
#define LINE_2_Y 13
#define LINE_3_Y 19

void diverter_test_draw (void)
{
	dmd_alloc_low_clean ();

	diverter_draw_test_title();

	// 21 characters wide max when using 5 point font.

	if (diverter_timer < 0) {
		sprintf ("HOLD");
	} else if (diverter_timer > 0) {
		sprintf ("FULL");
	} else {
		sprintf ("FLOAT");
	}
	font_render_string_center (&font_var5, 64, LINE_1_Y + 2, sprintf_buffer);

	sprintf ("TIMER:%d\n", diverter_timer);
	font_render_string_center (&font_var5, 64, LINE_2_Y + 2, sprintf_buffer);

	// XXX for debugging
	sprintf ("FPCO:%d\n", fliptronic_powered_coil_outputs);
	font_render_string_center (&font_var5, 64, LINE_3_Y + 2, sprintf_buffer);


	dmd_draw_horiz_line ((U16 *)dmd_low_buffer, 25);

	sprintf(diverter_test_short_names[diverter_test_command]);
	font_render_string_left (&font_mono5, 0, 27, sprintf_buffer);



	dmd_show_low ();
}


void diverter_test_thread (void)
{
	for (;;)
	{

		task_sleep (TIME_100MS);

		diverter_test_draw ();
	}
}


void diverter_test_up (void)
{
	if (diverter_test_command < LAST_TEST)
		diverter_test_command++;
}


void diverter_test_down (void)
{
	if (diverter_test_command > FIRST_TEST)
		diverter_test_command--;
}

void diverter_test_escape (void) {

	// stop everything
	diverter_stop();

	window_pop();
}

void diverter_test_enter (void)
{
	sound_send (SND_TEST_ENTER);
  	switch (diverter_test_command)
  	{
  		case START:
			dbprintf ("diverter_test_enter: starting 'open'\n");
			diverter_start();
			diverter_test_command = STOP;
  		break;
  		case STOP:
			dbprintf ("diverter_test_enter: starting 'close'\n");
			diverter_stop();
			diverter_test_command = START;
  		break;
  	}

}


struct window_ops corvette_diverter_test_window = {
	DEFAULT_WINDOW,
	.init = diverter_test_init,
	.draw = diverter_test_draw,
	.up = diverter_test_up,
	.down = diverter_test_down,
	.enter = diverter_test_enter,
	.escape = diverter_test_escape,
	.thread = diverter_test_thread,
};


struct menu corvette_diverter_test_item = {
	.name = "DIVERTER TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &corvette_diverter_test_window, NULL } },
};
