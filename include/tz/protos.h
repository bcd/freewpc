/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
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
void autofire_handler(void);
void autofire_add_ball(void);
void autofire_init(void);
/* ballsave.c */
void ballsave_enable(void);
void ballsave_disable(void);
bool ballsave_test_active(void);
void ballsave_launch(void);
/* bonus.c */
void bonus_leff(void);
void bonus_deff(void);
/* camera.c */
void camera_start_ball(void);
/* clock.c */
void tz_dump_clock(void);
void tz_clock_rtt(void);
void tz_clock_start_forward(void);
void tz_clock_start_backward(void);
void tz_clock_stop(void);
void tz_clock_init(void);
/* deadend.c */
void deadend_start_ball(void);
/* deffs.c */
void flash_and_exit_deff(U8 flash_count, task_ticks_t flash_delay);
void printf_millions(U8 n);
void printf_thousands(U8 n);
void replay_deff(void);
void extra_ball_deff(void);
void special_deff(void);
void jackpot_deff(void);
void ballsave_deff(void);
/* door.c */
void door_set_flashing(U8 id);
void door_advance_flashing(void);
void door_award_rotate(void);
void door_award_deff(void);
void door_award_if_possible(void);
void door_award_enable(void);
void door_start_player(void);
void door_start_ball(void);
/* greed.c */
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
void gumball_init(void);
/* jets.c */
void jet_start_player(void);
void jet_start_ball(void);
void sw_jet_sound(void);
/* lanes.c */
/* leffs.c */
void flasher_happy_leff(void);
void left_ramp_leff(void);
void no_lights_leff(void);
void no_gi_leff(void);
void flash_all_leff(void);
void slot_kickout_leff(void);
/* leftramp.c */
void left_ramp_deff(void);
void leftramp_start_player(void);
/* lock.c */
void lock_update_lamps(void);
void lock_start_player(void);
void lock_start_ball(void);
void lock_enter(device_t *dev);
void lock_kick_attempt(device_t *dev);
void lock_init(void);
/* loop.c */
void enter_loop(void);
void award_loop(void);
void abort_loop(void);
void award_left_loop(void);
void award_right_loop(void);
/* mball.c */
void mball_light_lock (void);
void mball_check_light_lock (void);
/* mpf.c */
__machine__ void mpf_enable (void);
/* rightramp.c */
void right_ramp_default_deff(void);
void sw_right_ramp_task(void);
void sw_right_ramp_init(void);
/* skill.c */
__machine__ void enable_skill_shot (void);
__machine__ void disable_skill_shot (void);
/* sling.c */
void slingshot_rtt(void);
/* slot.c */
void slot_update_lamps(void);
void slot_kick_sound(void);
void slot_enter(device_t *dev);
void slot_kick_attempt(device_t *dev);
void slot_start_ball(void);
void slot_init(void);
/* star.c */
void new_star_task(void);
void new_starfield_start(void);
void new_starfield_stop(void);
void star_task(void);
void starfield_task(void);
void starfield_start(void);
void starfield_stop(void);
/* test.c */
void autofire_launch_proc(void);
void autofire_to_gumball_proc(void);
void release_gumball_proc(void);
