
#ifndef _HWSIM_UI_H
#define _HWSIM_UI_H

void ui_init (void);
void ui_print_command (const char *cmd);
void ui_write_debug (enum sim_log_class c, const char *format, va_list ap);
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
