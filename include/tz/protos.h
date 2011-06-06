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

/* tz_amode.c */
__machine2__ void show_text_on_stars(void);

/* stars.c */
__machine2__ void star_draw(void);

/* autofire.c */
__machine__ void autofire_catch(void);
__machine__ void autofire_open_for_trough(void);
__machine__ void autofire_add_ball(void);
/* ballsave.c */
__common__ void ballsave_add_time(U8 secs);
__common__ void ballsave_disable(void);
__common__ void ballsave_launch(void);

/* camera.c * */
__machine2__ bool can_award_camera (void);

/* clock.c */
__machine__ void tz_dump_clock(void);
__machine__ void tz_clock_start_forward(void);
__machine__ void tz_clock_start_backward(void);
__machine__ void tz_clock_stop(void);
__machine__ void tz_clock_reset(void);
//__machine__ void tz_clock_show_time (U8 hours, U8 minutes);
__machine__ void tz_clock_reverse_direction (void);
__machine__ void tz_clock_set_speed (U8 speed);
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
__machine__ void award_door_panel_task (void);
__machine__ bool can_award_door_panel (void);
__machine__ bool check_relight_slot_or_piano (void);
__machine__ void door_advance_flashing (void);

/* gumball.c */
__machine__ void gumball_diverter_open(void);
__machine__ void gumball_diverter_close(void);
__machine__ void gumball_load_from_trough(void);
__machine__ void gumball_release(void);
__machine__ void sw_gumball_right_loop_entered(void);

/* loop.c */

/* mball.c */
void mball_light_lock (void);
void mball_check_light_lock (void);
__machine__ bool can_lock_ball (void);
__machine__ bool mball_restart_active (void);
/* mpf.c */
__machine__ void mpf_enable (void);
__machine__ bool mpf_ready_p (void);

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

/* spiralaward.c */
__machine2__ void spiralaward_right_loop_completed (void);


/* unlit.c*/
__machine2__ void award_unlit_shot (U8 unlit_called_from);
__machine2__ void backdoor_award_deff (void);
__machine2__ void tz_flipcode_default (void);
/* fastlock.c */
__machine__ void fastlock_loop_completed (void);
__machine__ bool fastlock_running (void);
__machine__ void fastlock_lock_entered (void);
/* powerball.c */
#define PB_IN_GUMBALL 0x4
#define PB_MAYBE_IN_PLAY 0x10
#define PB_IN_PLAY 0x8

typedef enum {
	PF_STEEL_DETECTED = 1,
	PF_PB_DETECTED,
	TROUGH_STEEL_DETECTED,
	TROUGH_PB_DETECTED,
	GUMBALL_PB_DETECTED,
} pb_event_t;


__machine__ void pb_clear_location (U8 location);
__machine__ void powerball_magnet_detect_task (void);
__machine__ bool pb_maybe_in_play (void);
__machine__ bool pb_in_lock (void);
__machine__ void pb_announce (void);
__machine__ void pb_detect_event (pb_event_t event);
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

/* maghelper.c */
__machine3__ bool magnet_enabled (U8 magnet);
__machine3__ bool magnet_busy (U8 magnet);
__machine3__ void magnet_enable_monitor_task (void);
__machine3__ void start_bttz_outhole_monitor (void);

/* factoids.c */
__machine3__ void show_random_factoid (void);

/* master.c */
__machine3__ void loop_master_check (void);
__machine3__ void combo_master_check (void);

__machine3__ void snake_start (void);

/* hurryup.c */
__machine3__ bool hurryup_active (void);

/* music.c */
__machine2__ void slow_music_lin (U8 count, U8 delay);
__machine2__ void slow_music_log (U8 count, U8 delay);
__machine2__ void speed_up_music_lin (U8 count, U8 delay);

/* oddchange.c */

/* shots.c */
__machine2__ bool task_find_or_kill_gid (free_timer_id_t gid);

/* test_bitmap.c */
__machine2__ void stardrop_overlay_draw (void);
__machine2__ void check_bitmap_overlay (void);

/* dollar.c */
__machine4__ void dollar_overlay (void);
__machine4__ void init_all_dollars (void);

/* bar.c */

struct progress_bar_ops 
{
	U8 x;
	U8 y;
	U8 *fill_level;
	U8 *max_level;
	U8 bar_width; //Needs to be a multiple of 5 + 
};

__machine4__ void draw_progress_bar (struct progress_bar_ops *ops);
