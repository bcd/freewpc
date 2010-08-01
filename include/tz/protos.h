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
__machine2__ bool can_award_camera (void);

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
void gumball_diverter_open(void);
void gumball_diverter_close(void);
void gumball_load_from_trough(void);
void gumball_release(void);
void sw_gumball_right_loop_entered(void);

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
__machine2__ void spiralaward_right_loop_completed (void);


/* unlit.c*/
__machine2__ void award_unlit_shot (U8 unlit_called_from);
__machine2__ void backdoor_award_deff (void);
__machine2__ void tz_flipcode_default (void);
/* fastlock.c */
__machine__ void fastlock_loop_completed (void);
__machine__ bool fastlock_running (void);
/* powerball.c */
#define PB_IN_GUMBALL 0x4
#define PB_MAYBE_IN_PLAY 0x10
#define PB_IN_PLAY 0x8
__machine__ void pb_clear_location (U8 location);
/* thingfl.c */
/* outhole.c */
__common__ bool ballsave_test_active (void);

//__machine2__ void tz_flipcode_1 (void);
__machine2__ void tz_flipcode_2 (void);

/* magnet.c */
#define MAG_LEFT 0
#define MAG_UPPER_RIGHT 1
#define MAG_RIGHT 2

void magnet_enable_catch (U8 magnet);
void magnet_enable_catch_and_hold (U8 magnet, U8 secs);
void magnet_enable_catch_and_throw (U8 magnet);
void magnet_disable_catch (U8 magnet);
void magnet_reset (void);

/* maghelpers.c */
__machine3__ bool magnet_enabled (U8 magnet);
__machine3__ bool magnet_busy (U8 magnet);
