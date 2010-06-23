
/* Powerball Test :
 * - Show where the powerball is.
 * - Show state of the proximity sensors.
 * - Options to kick to move balls around, updating status
 */

#include <freewpc.h>
#include <window.h>
#include <test.h>

/* TODO : copied from powerball.c */
#define PB_MISSING       0x0
#define PB_IN_LOCK       0x1
#define PB_IN_TROUGH     0x2
#define PB_IN_GUMBALL    0x4
#define PB_IN_PLAY       0x8
#define PB_MAYBE_IN_PLAY 0x10

#define PB_HELD         (PB_IN_LOCK | PB_IN_TROUGH | PB_IN_GUMBALL)
#define PB_KNOWN			(PB_HELD | PB_IN_PLAY)


enum {
	KICK_TROUGH,
	KICK_LOCK,
	RELEASE_GUMBALL,
	LOAD_GUMBALL,
} pb_test_command;


extern U8 pb_location;
extern U8 pb_depth;


void pb_test_init (void)
{
	pb_test_command = KICK_TROUGH;
}

void pb_test_draw (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_mono5, 64, 0, "POWERBALL TEST");

	if (pb_location == PB_MISSING)
		sprintf ("LOST");
	else if (pb_location & PB_IN_LOCK)
		sprintf ("LOCK");
	else if (pb_location & PB_IN_TROUGH)
		sprintf ("TROUGH");
	else if (pb_location & PB_IN_GUMBALL)
		sprintf ("GUMBALL");
	else if (pb_location & PB_IN_PLAY)
		sprintf ("P.F.");
	else if (pb_location & PB_MAYBE_IN_PLAY)
		sprintf ("NOT SURE");
	else
		sprintf ("ERROR");
	font_render_string_left (&font_mono5, 0, 6, sprintf_buffer);

	if (pb_location & (PB_IN_LOCK | PB_IN_TROUGH))
	{
		sprintf ("POS. %d", pb_depth);
		font_render_string_right (&font_mono5, 127, 6, sprintf_buffer);
	}

	sprintf ("TROUGH SW. %s",
		switch_poll_logical (SW_TROUGH_PROXIMITY) ? "CLOSED" : "OPEN");
	font_render_string_left (&font_mono5, 0, 12, sprintf_buffer);

	sprintf ("SLOT SW. %s",
		switch_poll_logical (SW_SLOT_PROXIMITY) ? "CLOSED" : "OPEN");
	font_render_string_left (&font_mono5, 0, 18, sprintf_buffer);

	switch (pb_test_command)
	{
		case KICK_TROUGH: sprintf ("REL. TROUGH"); break;
		case KICK_LOCK: sprintf ("REL. LOCK"); break;
		case RELEASE_GUMBALL: sprintf ("REL. GUMBALL"); break;
		case LOAD_GUMBALL: sprintf ("LOAD GUMBALL"); break;
	}
	font_render_string_left (&font_mono5, 0, 24, sprintf_buffer);

	dmd_show_low ();
}


void pb_test_thread (void)
{
	for (;;)
	{
		task_sleep (TIME_500MS);
		pb_test_draw ();
	}
}


void pb_test_up (void)
{
	if (pb_test_command < LOAD_GUMBALL)
		pb_test_command++;
}


void pb_test_down (void)
{
	if (pb_test_command > 0)
		pb_test_command--;
}


void pb_test_enter (void)
{
	sound_send (SND_TEST_ENTER);
	switch (pb_test_command)
	{
		case KICK_TROUGH:
			device_request_kick (device_entry (DEVNO_TROUGH));
			break;
		case KICK_LOCK:
			device_request_kick (device_entry (DEVNO_LOCK));
			break;
		case RELEASE_GUMBALL:
			gumball_release ();
			break;
		case LOAD_GUMBALL:
			gumball_load_from_trough ();
			break;
	}
}


struct window_ops tz_powerball_test_window = {
	DEFAULT_WINDOW,
	.init = pb_test_init,
	.draw = pb_test_draw,
	.up = pb_test_up,
	.down = pb_test_down,
	.enter = pb_test_enter,
	.thread = pb_test_thread,
};


struct menu tz_powerball_test_item = {
	.name = "POWERBALL TEST",
	.flags = M_ITEM,
	.var = { .subwindow = { &tz_powerball_test_window, NULL } },
};

