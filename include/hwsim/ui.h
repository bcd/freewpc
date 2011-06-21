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

#ifndef _HWSIM_UI_H
#define _HWSIM_UI_H

void ui_init (void);
void ui_print_command (const char *cmd);
void ui_write_debug (enum sim_log_class c, const char *buffer);
void ui_write_solenoid (int, int);
void ui_write_lamp (int, int);
void ui_write_triac (int, int);
void ui_write_switch (int, int);
void ui_write_sound_reset (void);
void ui_write_sound_command (unsigned int x);
void ui_update_ball_tracker (unsigned int ballno, const char *location);
void ui_refresh_display (unsigned int x, unsigned int y, char c);
void ui_refresh_asciidmd (unsigned char *data);
void ui_exit (void);

#endif /* _HWSIM_UI_H */
