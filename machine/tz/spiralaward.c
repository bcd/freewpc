/*
 * Copyright 2010 by Ewan Meadows <sonny_jim@hotmail.com>
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

/*
 * Spiral Award rules
 *
 * Hitting either left inlane rollover triggers a 3 second timer
 * and light display.  If the player hits the left_loop with the
 * timer then they receive a random Spiral award.
 *
 * Once they have collected each individual award, they will receive 20 million
 * but to retrigger the spiral award timer, they will have to complete
 * a rollover set, rather than just one.
 */


/* CALLSET_SECTION (spiralaward, __machine2__) */

#include <freewpc.h>
#include <eb.h>
#include <status.h>

U8 spiralaward;
__local__ U8 spiralawards_collected; 
__local__ bool spiralaward_set_completed; 
extern __local__ U8 mpf_enable_count;

const char *spiralaward_names[] = {
	"2 MILLION",
	"BATTLE THE POWER",
	"4 MILLION",
	"BATTLE THE POWER",
	"10 MILLION",
	"LIGHT EXTRA BALL"
};

const lampnum_t spiralaward_lamps[] = {
	LM_SPIRAL_2M,
	LM_SPIRAL_3M,
	LM_SPIRAL_4M,
	LM_SPIRAL_5M,
	LM_SPIRAL_10M,
	LM_SPIRAL_EB
};
static void write_spiralaward_text (void)
{
	if (spiralawards_collected < 6)
	{
		font_render_string_center (&font_var5, 64, 20, spiralaward_names[spiralaward]);
		font_render_string_center (&font_fixed6, 64, 5, "SPIRAL AWARD");
	}
	else 
	{
		font_render_string_center (&font_mono5, 64, 5, "SPIRALAWARD COMPLETED");
		sprintf ("20 MILLION");
		font_render_string_center (&font_term6, 64, 15, sprintf_buffer);
		font_render_string_center (&font_term6, 64, 25, spiralaward_names[spiralaward]);
	}
}

void spiralaward_collected_deff (void)
{	
	dmd_alloc_pair_clean ();
	U16 fno;
	for (fno = IMG_LOOP_END; fno >= IMG_LOOP_START; fno -= 2)
	{
		dmd_map_overlay ();
		dmd_clean_page_low ();
		write_spiralaward_text ();
		dmd_text_outline ();
		dmd_alloc_pair ();
		frame_draw (fno);
		callset_invoke (score_overlay);
		dmd_overlay_outline ();
		dmd_show2 ();
		task_sleep (TIME_33MS);
	}
	dmd_alloc_pair_clean ();
	write_spiralaward_text ();
	dmd_copy_low_to_high ();
	callset_invoke (score_overlay);
	dmd_show2 ();
	task_sleep_sec (2);
	deff_exit ();
}

static void flash_spiralaward_lamp (void)
{
	lamp_tristate_flash (spiralaward_lamps[spiralaward]);
	task_sleep_sec (3);
	/* Turn off lamp, award is collected */
	lamp_tristate_off (spiralaward_lamps[spiralaward]);
	task_exit ();
}

static void spiralaward_magnet_disable_task (void)
{
	task_sleep_sec (3);
	magnet_disable_catch (MAG_LEFT);
	task_exit ();
}

CALLSET_ENTRY (spiralaward, start_spiralaward_timer)
{	
	if (!timer_find_gid (GID_SPIRALAWARD) 
		&& single_ball_play ())
	{
		timer_restart_free (GID_SPIRALAWARD, TIME_3S);
		/* Allow the ball to pass through the loop,
		 * but not if the pb is out to help detection 
		 */
		if (!global_flag_test (GLOBAL_FLAG_POWERBALL_IN_PLAY))
			magnet_disable_catch (MAG_RIGHT);
		leff_start (LEFF_SPIRALAWARD);
		/* Only show the hint the first two times */
		if (spiralawards_collected < 1 && !spiralaward_set_completed);
			deff_start (DEFF_SHOOT_RIGHT_LOOP);
		/* Created as a task so it doesn't lock the calling thread */
		task_create_anon (spiralaward_magnet_disable_task);
	}
}

static void award_spiralaward (void)
{	
	bounded_increment (spiralawards_collected, 6);
	
	/* Pick a random award, random_scaled returns N-1 */
	spiralaward = random_scaled (6);
	/* Check to see if it's been previously awarded */
	while (!lamp_test(spiralaward_lamps[spiralaward]))
		spiralaward = random_scaled (6);

	/* Don't award extra ball until the last three */
	while (spiralaward == 5 && spiralawards_collected < 3)
		spiralaward = random_scaled (6);
	
	
	switch (spiralaward)
	{
		case 0:
			score (SC_2M);
			break;
		case 1:
			sound_send (SND_ARE_YOU_READY_TO_BATTLE);
			bounded_increment (mpf_enable_count, 99);
			break;
		case 2:
			score (SC_4M);
			break;
		case 3:
			sound_send (SND_ARE_YOU_READY_TO_BATTLE);
			bounded_increment (mpf_enable_count, 99);
			break;
		case 4:
			sound_send (SND_TEN_MILLION_POINTS);
			/* Light the 10M door panel */
			lamp_on (LM_PANEL_10M);
			score (SC_10M);
			break;
		case 5:
			sound_send (SND_GET_THE_EXTRA_BALL);
			light_easy_extra_ball ();
			break;
	}
	deff_start (DEFF_SPIRALAWARD_COLLECTED);
	
	/* Run lamp flash as task so it can run in parallel */
	task_create_gid (GID_FLASH_SPIRALAWARD_LAMP, flash_spiralaward_lamp);
	/* reset lamps after all 6 have been collected */
	if (spiralawards_collected == 6)
	{	
		/* Wait until lamp flash has finished */
		while (task_find_gid (GID_FLASH_SPIRALAWARD_LAMP))
			task_sleep (TIME_500MS);
		/* Reset Spiral Lamps */
		lamplist_apply (LAMPLIST_SPIRAL_AWARDS, lamp_on);
		/* Don't allow the Spiral EB twice */
		lamp_off (LM_SPIRAL_EB);
		/* Set to 1, as we have 'collected' the EB lamp */
		spiralawards_collected = 1;
		spiralaward_set_completed = TRUE;
	}
}

CALLSET_ENTRY (spiralaward, award_right_loop)
{
	if (task_kill_gid (GID_SPIRALAWARD_APPROACHING))
	{
		task_kill_gid (GID_SPIRALAWARD);
		leff_stop (LEFF_SPIRALAWARD);
		sound_send (SND_SLOT_PAYOUT);
		award_spiralaward ();
		magnet_disable_catch (MAG_LEFT);
	}
}

CALLSET_ENTRY (spiralaward, lamp_update)
{
}

CALLSET_ENTRY (spiralaward, end_ball)
{
	task_kill_gid (GID_FLASH_SPIRALAWARD_LAMP);
	task_kill_gid (GID_SPIRALAWARD);
}

CALLSET_ENTRY (spiralaward, start_player)
{
	lamplist_apply (LAMPLIST_SPIRAL_AWARDS, lamp_on);
	spiralawards_collected = 0;
	spiralaward_set_completed = FALSE;
	/* Turn off Spiral EB if impossible to collect */
	if (!can_award_extra_ball ())
	{
		lamp_off (LM_SPIRAL_EB);
		spiralawards_collected = 1;
	}
}

CALLSET_ENTRY (spiralaward, status_report)
{
	status_page_init ();
	sprintf ("%d SPIRALAWARDS", spiralawards_collected);
	font_render_string_center (&font_mono5, 64, 10, sprintf_buffer);
	status_page_complete ();
}
