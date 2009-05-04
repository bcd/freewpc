
#include <freewpc.h>


void (*message_line_handler) (U8 index, const char *line);

U8 message_line_count;



static void line_counter (U8 index, const char *line)
{
	message_line_count++;
}


static void line_printer (U8 index, const char *line)
{
	U8 row;

	if (message_line_count == 1)
	{
		row = 16;
	}
	else if (message_line_count == 2)
	{
		if (index == 0)
			row = 10;
		else
			row = 20;
	}
	else
		return;

	font_render_string_center (&font_mono5, 64, row, line);
}


/**
 * Iterate over the lines of a message.  Each time a newline is
 * found, the contents of that line are passed to a callback
 * function for processing.
 *
 * The last line must also end with a newline, else it will not
 * be processed at all.
 */
static void message_iterate (char *msg)
{
	char *line = msg;
	U8 index = 0;

	while (*msg != '\0')
	{
		if (*msg == '\n')
		{
			*msg = '\0';
			message_line_handler (index, line);
			*msg = '\n';
			line = msg+1;
			index++;
		}
		msg++;
	}
}


/**
 * Write a multiline message to the display.
 */
void message_write (const char *msg, U8 page)
{
	dmd_alloc_low_clean ();
	wpc_push_page (page);

	if (msg != sprintf_buffer)
		sprintf ("%s", msg);

	message_line_handler = line_counter;
	message_line_count = 0;
	message_iterate (sprintf_buffer);

	message_line_handler = line_printer;
	message_iterate (sprintf_buffer);

	wpc_pop_page ();
}

