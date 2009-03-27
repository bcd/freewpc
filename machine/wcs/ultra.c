
#include <freewpc.h>

U8 ultra_award_count[4];


void ultra_add_shot (U8 shot)
{
}

void ultra_add (void)
{
	if (!lamp_flash_test (LM_ULTRA_RAMPS))
		ultra_add_shot (0);
	else if (!lamp_flash_test (LM_ULTRA_GOALIE))
		ultra_add_shot (1);
	else if (!lamp_flash_test (LM_ULTRA_JETS))
		ultra_add_shot (2);
	else if (!lamp_flash_test (LM_ULTRA_SPINNER))
		ultra_add_shot (3);

	else if (!lamp_test (LM_ULTRA_RAMPS))
		ultra_add_shot (0);
	else if (!lamp_test (LM_ULTRA_GOALIE))
		ultra_add_shot (1);
	else if (!lamp_test (LM_ULTRA_JETS))
		ultra_add_shot (2);
	else if (!lamp_test (LM_ULTRA_SPINNER))
		ultra_add_shot (3);
}

void ultra_collect (U8 shot)
{
	ultra_award_count[shot]++;
}

CALLSET_ENTRY (ultra, left_ramp_shot, right_ramp_shot)
{
	if (lamp_flash_test (LM_ULTRA_RAMPS))
		ultra_collect (0);
}

CALLSET_ENTRY (ultra, sw_goalie)
{
	if (lamp_flash_test (LM_ULTRA_GOALIE))
		ultra_collect (1);
}

CALLSET_ENTRY (ultra, sw_left_jet, sw_right_jet, sw_bottom_jet)
{
	if (lamp_flash_test (LM_ULTRA_JETS))
		ultra_collect (2);
}

CALLSET_ENTRY (ultra, sw_spinner_slow)
{
	if (lamp_flash_test (LM_ULTRA_SPINNER))
		ultra_collect (3);
}

CALLSET_ENTRY (ultra, lamp_update)
{
	lamp_on_if (LM_ULTRA_RAMP_COLLECT, lamp_flash_test (LM_ULTRA_RAMPS));
}

CALLSET_ENTRY (ultra, start_player)
{
}

CALLSET_ENTRY (ultra, start_ball)
{
	lamplist_apply (LAMPLIST_ULTRA_MODES, lamp_off);
	lamplist_apply (LAMPLIST_ULTRA_MODES, lamp_flash_off);
	ultra_award_count[0] = 0;
	ultra_award_count[1] = 0;
	ultra_award_count[2] = 0;
	ultra_award_count[3] = 0;
}
