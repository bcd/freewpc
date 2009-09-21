
#include <freewpc.h>

__local__ U8 trap_door_bonuses;

const score_id_t trap_door_bonus_scores[] = {
	SC_250K, SC_500K, SC_750K
};
const struct generic_ladder trap_door_bonus_score_rule = {
	3,
	trap_door_bonus_scores,
	&trap_door_bonuses,
};

__local__ U8 gangway_count;

const score_id_t gangway_scores[] = {
	SC_75K, SC_100K, SC_150K, SC_200K, SC_250K
};
const struct generic_ladder gangway_score_rule = {
	5,
	gangway_scores,
	&gangway_count,
};



bool gangway_available_p (void)
{
	return !multiball_mode_running_p ();
}

CALLSET_ENTRY (gangway, left_loop_shot)
{
}

CALLSET_ENTRY (gangway, right_loop_shot)
{
}

CALLSET_ENTRY (gangway, lamp_update)
{
	lamp_on_if (LM_FLIPPER_LANES, gangway_available_p ());
}

CALLSET_ENTRY (gangway, start_player)
{
	lamplist_apply (LAMPLIST_GANGWAYS, lamp_off);
	lamp_tristate_flash (lamplist_index (LAMPLIST_GANGWAYS, 0));
	gangway_count = 0;
	trap_door_bonuses = 0;
}

