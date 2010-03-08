/*
 * Copyright 2006, 2007, 2008, 2009, 2010 by Brian Dominy <brian@oddchange.com>
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
__machine2__ void show_driver_animation(void);
__machine2__ void show_text_on_stars(void);

/* stars.c */
__machine2__ void star_draw(void);

/* autofire.c */
void autofire_catch(void);
void autofire_add_ball(void);
/* ballsave.c */
__common__ void ballsave_add_time(U8 secs);
__common__ void ballsave_disable(void);
__common__ void ballsave_launch(void);
/* camera.c * */
__machine__ void door_start_camera (void);
/* clock.c */
void tz_dump_clock(void);
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
__machine__ void tz_ball_save_deff(void);
__machine__ void rollover_completed_deff (void);

/* door.c */
__machine__ void shot_slot_machine (void);
__machine__ void shot_piano (void);
__machine__ void award_door_panel (void);

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
/* These should all be done by tz.md */
/*__machine2__ void bonus_leff(void); 
__machine2__ void gi_cycle_leff(void); 
__machine2__ void flasher_happy_leff(void);
__machine2__ void left_ramp_leff(void);
__machine2__ void no_gi_leff(void);
__machine2__ void turn_on_gi_leff(void);
__machine2__ void flash_all_leff(void);
__machine2__ void slot_kickout_leff(void);
__machine2__ void gumball_strobe_leff(void); 
__machine2__ void clock_target_leff(void); 
__machine2__ void game_timeout_leff(void); 
__machine2__ void clock_round_started_leff(void); 
__machine2__ void multiball_running_leff(void); 
__machine2__ void strobe_up_leff(void); 
__machine2__ void multi_strobe_leff(void); 
__machine2__ void door_strobe_leff(void); 
__machine2__ void right_loop_leff(void); 
__machine2__ void left_loop_leff(void); 
__machine2__ void jets_active_leff(void);
__machine2__ void circle_out_leff(void); 
__machine2__ void color_cycle_leff(void); 
__machine2__ void lock_leff(void); 
__machine2__ void mpf_active_leff(void); 
*/

/* loop.c */
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
__machine__ void skill_shot_ready_deff (void);
__machine__ void skill_shot_made_deff (void);
/* sssmb.c */
__machine__ void sssmb_start (void);
__machine__ void sssmb_stop (void);
__machine__ bool sssmb_can_divert_to_plunger (void);

/* star.c */
__machine__ void star_draw(void);
/* spiralaward.c */
__machine2__ void start_spiralaward_timer (void);
__machine2__ void spiralaward_right_loop_completed (void);

/* lanes.c */
/*__machine2__ static void handle_outlane (void);
__machine2__ static bool rollover_completed (void);
__machine2__ static void award_rollover_completed (void);
__machine2__ static void check_rollover (void);*/
/* jets.c */

/* unlit.c*/
__machine2__ void award_unlit_shot (U8 unlit_called_from);
__machine2__ void backdoor_award_deff (void);
__machine2__ void tz_flipcode_default (void);
/* fastlock.c */
__machine__ void fastlock_right_loop_completed (void);
__machine__ bool fastlock_running (void);
/* powerball.c */
#define PB_IN_GUMBALL 0x4
#define PB_MAYBE_IN_PLAY 0x10
#define PB_IN_PLAY 0x8
__machine__ void pb_clear_location (U8 location);
/* thingfl.c */
/* outhole.c */
__common__ bool ballsave_test_active (void);
