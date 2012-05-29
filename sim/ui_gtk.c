/*
 * Copyright 2010 by Brian Dominy <brian@oddchange.com>
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

#include <gtk/gtk.h>
#include <string.h>
#include <stdarg.h>
#include <simulation.h>

/* \file ui_gtk.c
 * \brief A GTK-based UI for the built-in WPC simulator.
 */

GtkBuilder *GTK_builder;
GtkWidget *GTK_window;

GtkWidget *debug_widget;
GtkTextBuffer *debug_text_buffer;
GtkAdjustment *debug_adj;

GtkWidget *msg_widget;
GtkTextBuffer *msg_text_buffer;
GtkAdjustment *msg_adj;

GtkWidget *dmd_widget;
GdkColor dmd_colors[PINIO_DMD_PIXEL_COLORS];
int dmd_current_color = -1;

unsigned char dmd_prev[PINIO_DMD_WIDTH * PINIO_DMD_HEIGHT];
unsigned char dmd_next[PINIO_DMD_WIDTH * PINIO_DMD_HEIGHT];

void ui_print_command (const char *cmdline)
{
}

void ui_write_debug (enum sim_log_class c, const char *buf)
{
	GtkTextBuffer *gtk_buf;
	GtkAdjustment *gtk_adj;

	gtk_buf = (c == SLC_DEBUG_PORT) ? debug_text_buffer : msg_text_buffer;
	gtk_adj = (c == SLC_DEBUG_PORT) ? debug_adj : msg_adj;

	gtk_text_buffer_insert_at_cursor (gtk_buf, buf, -1);
	gtk_text_buffer_insert_at_cursor (gtk_buf, "\n", -1);
	gtk_adjustment_set_value (gtk_adj, gtk_adjustment_get_upper (gtk_adj));
}


void ui_write_solenoid (int solno, int on_flag)
{
}


void ui_write_lamp (int lampno, int on_flag)
{
}


void ui_write_triac (int triacno, int on_flag)
{
}


void ui_write_switch (int switchno, int on_flag)
{
}


void ui_write_sound_command (unsigned int x)
{
}


void ui_write_sound_reset (void)
{
}


void ui_write_task (int taskno, int gid)
{
}


#if (MACHINE_DMD == 1)
void ui_refresh_asciidmd (unsigned char *data)
{
	memcpy (dmd_next, data, PINIO_DMD_WIDTH * PINIO_DMD_HEIGHT);
	gdk_window_invalidate_rect (dmd_widget->window, NULL, FALSE);
}
#else
void ui_refresh_display (unsigned int x, unsigned int y, char c)
{
}
#endif


void ui_update_ball_tracker (unsigned int ballno, const char *location)
{
}

void sw_toggle_cb (GtkObject *sw, gpointer user)
{
	sim_switch_toggle (GPOINTER_TO_INT (user));
}

void sw_activate_cb (GtkObject *sw, gpointer user)
{
	sim_switch_depress (GPOINTER_TO_INT (user));
}

gboolean display_expose_event_cb (GtkWidget *widget, GdkEventExpose *event,
	gpointer data)
{
	GdkGC	*fg, *bg, *gc;
	unsigned int x, y, width;
	unsigned char *nx;
	unsigned char *pv = dmd_prev;
	static int inited = 0;
	int color;

#define DMD_COLOR_BLACK 0
#define DMD_COLOR_DEFAULT 2

	if (!inited)
	{
		gdk_color_parse ("black", &dmd_colors[0]);
#ifdef CONFIG_PLATFORM_WPC
		gdk_color_parse ("orange3", &dmd_colors[1]);
		gdk_color_parse ("orange2", &dmd_colors[2]);
		gdk_color_parse ("orange1", &dmd_colors[3]);
#else
#error "No colormap defined for this platform"
#endif
		gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, &dmd_colors[0]);
		dmd_current_color = DMD_COLOR_DEFAULT;
		gtk_widget_modify_fg (widget, GTK_STATE_NORMAL, &dmd_colors[DMD_COLOR_DEFAULT]);
		inited = 1;
	}

	fg = widget->style->fg_gc[gtk_widget_get_state (widget)];
	bg = widget->style->bg_gc[gtk_widget_get_state (widget)];

#define DMD_PIXEL_BOX_SIZE 4

	for (color = 0; color < PINIO_DMD_PIXEL_COLORS; color++)
	{
		nx = dmd_next;
		for (y = 0; y < PINIO_DMD_HEIGHT; y++)
		{
			for (x = 0; x < PINIO_DMD_WIDTH; x++, nx++)
			{
				if (color != *nx)
					continue;

				if (color == DMD_COLOR_BLACK)
					gc = bg;
				else if (color != dmd_current_color)
				{
					gtk_widget_modify_fg (widget, GTK_STATE_NORMAL,
						&dmd_colors[color]);
					gc = fg = widget->style->fg_gc[gtk_widget_get_state (widget)];
					dmd_current_color = color;
				}
				else
					gc = fg;
				width = (*nx != 0) ? (DMD_PIXEL_BOX_SIZE-1) : DMD_PIXEL_BOX_SIZE;
				gdk_draw_rectangle (widget->window, gc, TRUE,
					x*DMD_PIXEL_BOX_SIZE+1, y*DMD_PIXEL_BOX_SIZE+1, width, width);
			}
		}
	}
	return TRUE;
}


/**
 * Handle program shutdown from the menus/close button.
 */
void window1_destroy_cb (GtkObject *object, gpointer user)
{
	sim_exit (0);
}


/**
 * Call the gtk main loop, but for only one iteration.
 *
 * Instead of having GTK run the main loop, FreeWPC remains in
 * control of the overall program, and we call into GTK periodically
 * to let it process its events.
 */
void gtk_poll (void)
{
	gtk_main_iteration_do (FALSE);
}


static GtkWidget *ui_widget_named (const char *name)
{
	return GTK_WIDGET (gtk_builder_get_object (GTK_builder, name));
}

void ui_connect_sw (const char *name, unsigned int no)
{
	GtkWidget *sw;
	sw = GTK_WIDGET (gtk_builder_get_object (GTK_builder, name));
	g_signal_connect (sw, "pressed", G_CALLBACK (sw_toggle_cb),
		GINT_TO_POINTER (no));
	g_signal_connect (sw, "released", G_CALLBACK (sw_toggle_cb),
		GINT_TO_POINTER (no));
}

void ui_init (void)
{
	g_type_init ();
	GTK_builder = gtk_builder_new ();
	gtk_builder_add_from_file (GTK_builder,
		"platform/native/freewpc-gtk.xml", NULL);
	GTK_window = GTK_WIDGET (gtk_builder_get_object (GTK_builder, "window1"));
	gtk_builder_connect_signals (GTK_builder, NULL);

	debug_widget = ui_widget_named ("debug_textview");
	debug_text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (debug_widget));
	debug_adj = GTK_ADJUSTMENT (gtk_builder_get_object (GTK_builder,
		"debug_v_adjustment"));

	msg_widget = ui_widget_named ("msg_textview");
	msg_text_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (msg_widget));
	msg_adj = GTK_ADJUSTMENT (gtk_builder_get_object (GTK_builder,
		"msg_v_adjustment"));

	dmd_widget = ui_widget_named ("display_area");

	/* Connect the coin door buttons */
	/* TODO - autogenerate this block */
	ui_connect_sw ("sw_left_coin", 0);
	ui_connect_sw ("sw_center_coin", 1);
	ui_connect_sw ("sw_right_coin", 2);
	ui_connect_sw ("sw_fourth_coin", 3);
	ui_connect_sw ("sw_escape", 4);
	ui_connect_sw ("sw_down", 5);
	ui_connect_sw ("sw_up", 6);
	ui_connect_sw ("sw_enter", 7);

	g_object_unref (G_OBJECT (GTK_builder));
	gtk_widget_show (GTK_window);
}


void ui_exit (void)
{
}

