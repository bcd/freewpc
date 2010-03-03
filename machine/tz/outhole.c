
/* CALLSET_SECTION (outhole, __machine2__) */

#include <freewpc.h>
/* How many balls have drained in three seconds */
U8 multidrain_count;

void multidrain_deff (void)
{
	dmd_alloc_pair_clean ();
	font_render_string_center (&font_fixed6, 64, 16, "MULTIDRAIN");
	dmd_show2 ();
	task_sleep_sec (2);
	deff_exit ();
}

CALLSET_ENTRY (outhole, sw_outhole)
{	
	if (in_live_game && !timer_find_gid(GID_OUTHOLE_DEBOUNCE))
	{
		timer_restart_free (GID_OUTHOLE_DEBOUNCE, TIME_1S);
		/* Timer to check if 3 balls drain quickly */
		if (!timer_find_gid (GID_MULTIDRAIN) && multi_ball_play ())
		{
			multidrain_count = 0;
			timer_restart_free (GID_MULTIDRAIN, TIME_10S);
		}
	
		if (timer_find_gid (GID_MULTIDRAIN))
		{
			/* There are 6 balls installed normally */
			bounded_increment (multidrain_count, 6);
			if (multidrain_count == 3)
			{
				//music_timed_disable (TIME_3S);
				sound_send (SND_HEY_ITS_ONLY_PINBALL);
				deff_start (DEFF_MULTIDRAIN);
				timer_restart_free (GID_MULTIDRAIN_ANIM_RUNNING, TIME_2S);
			}
		}
		
		deff_start (DEFF_BALL_EXPLODE);
		/* Don't allow the deff to start more than once a second */
		if (in_live_game && !timer_find_gid (GID_MULTIDRAIN_ANIM_RUNNING))
			deff_start (DEFF_BALL_EXPLODE);
	}
	
}

CALLSET_ENTRY (outhole, ball_start)
{
	multidrain_count = 0;
	timer_kill_gid (GID_MULTIDRAIN);
}
