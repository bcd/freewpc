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


void ui_print_command (const char *cmdline)
{
}

void ui_write_debug (enum sim_log_class c, const char *format, va_list ap)
{
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
}
#else
void ui_refresh_display (unsigned int x, unsigned int y, char c)
{
}
#endif


void ui_update_ball_tracker (unsigned int ballno, unsigned int location)
{
	extern const char *sim_ball_location_name (unsigned int location);
}


void
on_window_destroy (GtkObject *object, gpointer user_data)
{
	gtk_main_quit ();
}


void ui_init (void)
{
	GtkBuilder *GTK_builder;
	GtkWidget *GTK_window;

	g_type_init ();
	GTK_builder = gtk_builder_new ();
	gtk_builder_add_from_file (GTK_builder,
		"platform/native/freewpc-gtk.xml", NULL);
	GTK_window = GTK_WIDGET (gtk_builder_get_object (GTK_builder, "window1"));
	gtk_builder_connect_signals (GTK_builder, NULL);
	g_object_unref (G_OBJECT (GTK_builder));
	gtk_widget_show (GTK_window);
	gtk_main ();
}


void ui_exit (void)
{
}

