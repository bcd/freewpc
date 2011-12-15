
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_ARGV 4

typedef unsigned int dcc_color;

typedef unsigned int dcc_time;


typedef enum
{
	ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} dcc_halignment;

typedef enum
{
	ALIGN_TOP, ALIGN_MIDDLE, ALIGN_BOTTOM
} dcc_valignment;


typedef struct
{
	const char *name;
	unsigned int height;
} dcc_font;

dcc_font default_font = { "default", 8 };

typedef struct
{
	unsigned int x, y;
	unsigned int width, height;
} dcc_area;


typedef struct
{
	dcc_area area;
	dcc_color color;
	dcc_color bgcolor;
	dcc_font *font;
	dcc_time blink_time;
	dcc_halignment halign;
	dcc_valignment valign;
	const char *c_cond;
	const char *text;
	unsigned int argc;
	const char *argv[MAX_ARGV];

	/* Computed values */

/* Set if this is a static element which does not change
	from frame to frame */
	unsigned int static_flag : 1;
/* Set if this is element is to be rendered onto the overlay
	for optimization. */
	unsigned int overlay : 1;
} dcc_label;


typedef struct
{
	const char *c_name;
	dcc_time frame_time;
	dcc_color bgcolor;
	const char *c_bg;
	const char *c_init;
	const char *c_frame_init;
	const char *c_exit;
	unsigned int border_width;
	dcc_time duration;

	/* Computed values */
	unsigned int page_count;
	unsigned int single_frame : 1;
	unsigned int overlay : 1;
} dcc_effect;

dcc_effect the_effect;
dcc_area area_stack[16];
dcc_label label_list[16];

dcc_effect *current_effect = &the_effect;
dcc_area *current_area;
dcc_label *current_label;
dcc_label *last_label;
dcc_font *current_font;

unsigned int indent_level = 0;

int debug = 0;


void init_effect (const char *c_name)
{
	current_effect->c_name = c_name;
	current_effect->frame_time = 50; /* 20 fps */
	current_effect->bgcolor = 0;
	current_effect->c_init = NULL;
	current_effect->c_frame_init = NULL;
	current_effect->c_exit = NULL;
	current_effect->border_width = 0;
	current_effect->duration = 0; /* 0 = infinite */

	current_area = area_stack;
	current_area->x = current_area->y = 0;
	current_area->width = 128;
	current_area->height = 32;

	current_font = &default_font;

	last_label = NULL;
}


dcc_label *
alloc_label (const char *format)
{
	if (last_label == NULL)
	{
		current_label = label_list;
		/* For the first label, set its inherited attributes.  Color and
		font do not need to be specified if they do not change. */
		current_label->color = 3;
		current_label->bgcolor = 0;
		current_label->font = &default_font;
	}
	else
	{
		/* For subsequent labels, copy its inherited attributes from the
		previous */
		current_label++;
		current_label->color = (current_label-1)->color;
		current_label->bgcolor = (current_label-1)->bgcolor;
		current_label->font = (current_label-1)->font;
	}

	/* Set non-inherited attributes.  You must always specify these. */
	current_label->area = *current_area;
	current_label->blink_time = 0;
	current_label->halign = ALIGN_CENTER;
	current_label->valign = ALIGN_MIDDLE;
	current_label->c_cond = NULL;
	current_label->text = format;
	current_label->argc = 0;

	last_label = current_label;
	return current_label;
}


void
add_label_arg (dcc_label *label, const char *arg)
{
	current_label->argv[current_label->argc++] = arg;
}


void
add_label_condition (dcc_label *label, const char *condition)
{
	if (!label->c_cond)
	{
		label->c_cond = condition;
		return;
	}

	/* Concatenation this condition with the previous one */
	char *total_cond = malloc (strlen (condition) + strlen (label->c_cond) + 8);
	sprintf (total_cond, "%s && %s", label->c_cond, condition);
	label->c_cond = total_cond;
}


void
push_area (void)
{
	current_area[1] = current_area[0];
	current_area++;
}


void
push_column_area (void)
{
	push_area ();
	current_area->width = 0;
}


void
push_row_area (void)
{
	push_area ();
	current_area->height = 0;
}


void
add_column (unsigned int width)
{
	current_area->x += current_area->width;
	current_area->width = width;
}


void
add_row (unsigned int height)
{
	current_area->y += current_area->height;
	current_area->height = height;
}


void
pop_area (void)
{
	current_area--;
}


void
add_padding (unsigned int hpad, unsigned int vpad)
{
	current_area->x += hpad;
	current_area->y += vpad;
	current_area->width -= 2 * hpad;
	current_area->height -= 2 * vpad;
}


void
adjust_indent (signed int offset)
{
	indent_level += offset;
}


void
output (const char *format, ...)
{
	va_list ap;

	int spc;
	for (spc=0; spc < indent_level; spc++)
		fputc (' ', stdout);
	va_start (ap, format);
	vfprintf (stdout, format, ap);
	va_end (ap);
	fputc ('\n', stdout);
}


void
block_begin (void)
{
	output ("{");
	adjust_indent (+3);
}

void
block_end (void)
{
	adjust_indent (-3);
	output ("}");
}


void
output_delay (dcc_time delay)
{
	if (delay == 0)
	{
		output ("task_suspend ();");
		return;
	}

	while (delay >= 30000) {
		output ("task_sleep (TIME_30S);");
		delay -= 30000;
	}

	int secs = delay / 1000;
	int msecs = delay % 1000;
	int tenths_of_sec = msecs / 100;
	int hundredths_of_sec = msecs % 100;

	if (secs)
		output ("task_sleep_sec (TIME_%uS);", secs);
	if (tenths_of_sec)
		output ("task_sleep (TIME_%u00MS);", tenths_of_sec);
	if (hundredths_of_sec)
		output ("task_sleep (TIME_%uMS);", hundredths_of_sec);
}


void
output_cond (const char *condition,
	const char *true_stmt, const char *false_stmt)
{
	output ("if (%s)", condition);
	block_begin ();
	output (true_stmt);
	block_end ();
	output ("else");
	block_begin ();
	output (false_stmt);
	block_end ();
}


void
open_if_block (const char *c_cond)
{
	output ("if (%s)", c_cond);
	block_begin ();
}


void
close_if_block (const char *c_cond)
{
	block_end ();
}


void
output_sprintf (const char *format,
	int argc, const char *argv[])
{
	output ("sprintf (%s", format);
	int n;
	for (n=0; n < argc; n++) {
		output (", %s", argv[n]);
	}
}


void
emit_label (dcc_label *label)
{
	unsigned int sx, sy;
	const char *alignment;
	const char *text;

	if (debug)
		output ("/* area=(%u,%u,%u,%u) */", label->area.x, label->area.y,
			label->area.width, label->area.height);

	sx = label->area.x;
	alignment = "left";
	if (label->halign == ALIGN_CENTER) {
		sx = sx + (label->area.width / 2);
		alignment = "center";
	}
	else if (label->halign == ALIGN_RIGHT) {
		sx = sx + label->area.width - 1;
		alignment = "right";
	}

	sy = label->area.y;
	if (label->valign == ALIGN_MIDDLE)
		sy = sy + (label->area.height / 2);
	else if (label->halign == ALIGN_BOTTOM)
		sy = sy + label->area.height - label->font->height;

	if (label->c_cond)
		open_if_block (label->c_cond);

	/* TODO - if the label is being emitted onto a dirty page,
	perform a rough erase of the area */

	if (label->argc == 0)
		text = label->text;
	else
	{
		output_sprintf (label->text, label->argc, label->argv);
		text = "sprintf_buffer";
	}

	output ("font_render_string_%s (&font_%s, %u, %u, %s);",
		alignment, label->font->name, sx, sy, text);

	/* TODO - handle color */

	/* TODO - perform a rough invert if there is a bgcolor */

	/* TODO - if we are on the overlay to write onto an image,
	see if we need to generate the outline. */

	if (label->c_cond)
		close_if_block (label->c_cond);
}


void
emit_deff_init (void)
{
	/* Emit initializer for tracking which frame we are on. */
	output ("U8 iter = 0;");

	/* Emit overlay initialization, if any. */
	if (current_effect->overlay)
	{
	}

	/* Emit user initialization */
	if (current_effect->c_init)
		output (current_effect->c_init);
}


void
emit_deff_exit (void)
{
	if (current_effect->single_frame)
		output_delay (current_effect->duration);
	/* Emit user exit code */
	if (current_effect->c_exit)
		output (current_effect->c_exit);
	output ("deff_exit ();");
}


void
emit_loop_init (void)
{
	/* Emit page allocation code */
	output ("dmd_alloc_pair ();");
	if (current_effect->c_bg)
		output (current_effect->c_bg);
	else
		output ("dmd_clean_page_low ();");

	/* Draw the border */
	if (current_effect->border_width)
	{
		output ("dmd_draw_border (dmd_low_buffer);");
	}
}


void
emit_loop_exit (void)
{
	/* Emit code to show the frame */
	if (current_effect->page_count > 1)
		output ("dmd_show2 ();");
	else
		output ("dmd_show_low ();");

	/* Emit a delay for the frame rate.  If only a single frame
	needs to be rendered, this turns into a break. */
	if (current_effect->single_frame)
		output ("break;");
	else
		output ("iter++;");
		output_delay (current_effect->frame_time);
}


void
emit_effect (void)
{
	output ("void %s_deff (void)", current_effect->c_name);
	block_begin (); /* begin function */
	emit_deff_init ();

	output ("while (1)");
	block_begin (); /* begin main loop */
	emit_loop_init ();
	dcc_label *label;
	if (last_label)
		for (label = label_list; label <= last_label; label++)
			emit_label (label);
	emit_loop_exit ();
	block_end (); /* end main loop */

	emit_deff_exit ();
	block_end (); /* end function */
	output ("\n");
}


void
finish_effect (void)
{
	/* Analyze all labels and the effect as a whole, computing
	internal values. */
	dcc_label *label;
	for (label = label_list; label <= last_label; label++)
	{
		label->overlay = 0;
		label->static_flag =
			((label->c_cond == NULL) && (label->blink_time == 0) &&
			 (label->argc == 0));

		/* Handle blinking elements. */
		if (label->blink_time)
		{
			label->c_cond = "iter & 0x01";
		}
	}

	/* See if this qualifies as a single frame effect.  This is true
	if all elements are static. */
	current_effect->single_frame = 1;
	for (label = label_list; label <= last_label; label++)
		if (label->static_flag == 0)
		{
			current_effect->single_frame = 0;
			break;
		}

	current_effect->overlay = 0;

	/* Generate C code */
	emit_effect ();
}


void
gen_simple_message (const char *name, const char *message,
	dcc_time duration)
{
	init_effect (name);
	dcc_label *l = alloc_label (message);
	current_effect->duration = duration;
	finish_effect ();
}

void
gen_simple_blink (const char *name, const char *message,
	dcc_time duration, dcc_time blink_time)
{
	init_effect (name);
	dcc_label *l = alloc_label (message);
	l->blink_time = blink_time;
	current_effect->duration = duration;
	current_effect->frame_time = blink_time;
	finish_effect ();
}

void
gen_two_line (void)
{
	init_effect ("two_line");
	push_row_area ();
		add_row (16);
			alloc_label ("SPECIAL");
		add_row (16);
			alloc_label ("WHEN LIT");
	pop_area ();
	current_effect->duration = 2000;
	finish_effect ();
}


int
main (int argc, char *argv[])
{
	output ("#include <freewpc.h>\n");
	gen_simple_message ("game_over", "\"GAME OVER\"", 1500);
	gen_simple_blink ("replay", "\"REPLAY\"", 3000, 200);
	gen_two_line ();
}

