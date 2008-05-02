/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
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

/* amode.c */
bool amode_page_delay(U8 secs);
void amode_flipper_sound_debounce_timer(void);
void amode_flipper_sound(void);
void amode_scroll(void);
void egg_left_flipper(void);
void egg_brian_image_deff(void);
void egg_right_flipper(void);
void amode_left_flipper(void);
void amode_right_flipper(void);
void amode_lamp_toggle_task(void);
void amode_leff(void);
void amode_show_design_credits(void);
void amode_deff(void);
/* autofire.c */
void autofire_catch(void);
void autofire_add_ball(void);
/* ballsave.c */
void ballsave_add_time(U8 secs);
void ballsave_disable(void);
void ballsave_launch(void);
/* clock.c */
void tz_dump_clock(void);
void tz_clock_rtt(void);
void tz_clock_start_forward(void);
void tz_clock_start_backward(void);
void tz_clock_stop(void);
/* deffs.c */
__machine__ void flash_and_exit_deff(U8 flash_count, task_ticks_t flash_delay);
__machine__ void printf_millions(U8 n);
__machine__ void printf_thousands(U8 n);
__machine__ void replay_deff(void);
__machine__ void extra_ball_deff(void);
__machine__ void special_deff(void);
__machine__ void jackpot_deff(void);
__machine__ void ballsave_deff(void);
/* door.c */
void door_set_flashing(U8 id);
void door_advance_flashing(void);
void door_award_rotate(void);
void door_award_deff(void);
void door_award_if_possible(void);
void door_award_enable(void);
/* gumball.c */
void gumball_mech_activate(void);
void gumball_mech_deactivate(void);
void gumball_diverter_open(void);
void gumball_diverter_close(void);
bool gumball_load_is_enabled(void);
void gumball_load_disable(void);
void gumball_load_enable(void);
void gumball_load_from_trough(void);
void gumball_release(void);
void sw_gumball_right_loop_entered(void);
/* leffs.c */
void flasher_happy_leff(void);
void left_ramp_leff(void);
void no_lights_leff(void);
void no_gi_leff(void);
void flash_all_leff(void);
void slot_kickout_leff(void);
/* loop.c */
__machine__ void enter_loop(void);
__machine__ void award_loop(void);
__machine__ void abort_loop(void);
__machine__ void award_left_loop(void);
__machine__ void award_right_loop(void);
/* mball.c */
void mball_light_lock (void);
void mball_check_light_lock (void);
/* mpf.c */
__machine__ void mpf_enable (void);
/* rampdiv.c */
void ramp_divert (void);
void ramp_divert_to_autoplunger (void);
/* skill.c */
__machine__ void enable_skill_shot (void);
__machine__ void disable_skill_shot (void);
/* sssmb.c */
__machine__ void sssmb_start (void);
__machine__ void sssmb_stop (void);
/* star.c */
__machine__ void star_reset(void);
__machine__ void star_draw(void);
__machine__ void star_task(void);
__machine__ void starfield_task(void);
__machine__ void starfield_start(void);
__machine__ void starfield_stop(void);

