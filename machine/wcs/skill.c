
#include <freewpc.h>

__local__ U8 skill_count;

void skill_start (void)
{
	flag_on (FLAG_SKILL_LIT);
}

void skill_stop (void)
{
	flag_off (FLAG_SKILL_LIT);
}

void skill_made (void)
{
	skill_stop ();
	skill_count++;
	sound_start (ST_MUSIC, MUS_SKILL_SHOT_CITY, SL_1S, PRI_GAME_QUICK6);
	sound_start (ST_SPEECH, SND_CROWD_CHEER, SL_2S, PRI_GAME_QUICK4);
}

void skill_miss (void)
{
	skill_stop ();
	sound_start (ST_SAMPLE, SND_DING1, SL_500MS, PRI_GAME_QUICK6);
	sound_start (ST_SPEECH, SND_CROWD_AWW, SL_2S, PRI_GAME_QUICK4);
}

CALLSET_ENTRY (skill_shot, sw_skill_shot_front, sw_skill_shot_rear)
{
	if (flag_test (FLAG_SKILL_LIT))
	{
		skill_miss ();
	}
}

CALLSET_ENTRY (skill_shot, sw_skill_shot_center)
{
	if (flag_test (FLAG_SKILL_LIT))
	{
		skill_made ();
	}
}

CALLSET_ENTRY (skill_shot, lamp_update)
{
	lamp_flash_if (LM_SKILL_MIDDLE, flag_test (FLAG_SKILL_LIT));
}

CALLSET_ENTRY (skill_shot, start_player)
{
	skill_count = 0;
}

CALLSET_ENTRY (skill_shot, serve_ball)
{
	skill_start ();
}

