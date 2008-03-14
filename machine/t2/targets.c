
#include <freewpc.h>

extern inline const switch_info_t *switch_lookup_current (void)
{
	extern const switch_info_t switch_table[];
	const U8 sw = (U8)task_get_arg ();
	return &switch_table[sw];
}


typedef const struct
{
	void (*made) (void);
	void (*make_unlit) (void);
	void (*make_lit) (void);
	void (*make_flash) (void);
	void (*light_all) (void);
	void (*flash_all) (void);
} bank_ops_t;


typedef const struct
{
	bank_ops_t *ops;	
	const char *name;
	U8 gid;
	U8 sw_count;
	U8 switches[8];
	U8 lamplist;
} bank_properties_t;

#define BANK_SKILL 0
#define BANK_CENTER 1
#define BANK_RIGHT 2
#define BANK_TOP 3


extern void callset_bank_center_made (void); /* callset_invoke (bank_center_made) */
extern void callset_bank_center_make_unlit (void); /* callset_invoke (bank_center_make_unlit) */
extern void callset_bank_center_make_lit (void); /* callset_invoke (bank_center_make_lit) */
extern void callset_bank_center_make_flash (void); /* callset_invoke (bank_center_make_flash) */
extern void callset_bank_center_light_all (void); /* callset_invoke (bank_center_light_all) */
extern void callset_bank_center_flash_all (void); /* callset_invoke (bank_center_flash_all) */

const bank_ops_t bank_center_ops = {
	.made = callset_bank_center_made,
	.make_unlit = callset_bank_center_make_unlit,
	.make_lit = callset_bank_center_make_lit,
	.make_flash = callset_bank_center_make_flash,
	.light_all = callset_bank_center_light_all,
	.flash_all = callset_bank_center_flash_all,
};

extern void callset_bank_top_made (void); /* callset_invoke (bank_top_made) */
extern void callset_bank_top_make_unlit (void); /* callset_invoke (bank_top_make_unlit) */
extern void callset_bank_top_make_lit (void); /* callset_invoke (bank_top_make_lit) */
extern void callset_bank_top_make_flash (void); /* callset_invoke (bank_top_make_flash) */
extern void callset_bank_top_light_all (void); /* callset_invoke (bank_top_light_all) */
extern void callset_bank_top_flash_all (void); /* callset_invoke (bank_top_flash_all) */

const bank_ops_t bank_top_ops = {
	.made = callset_bank_top_made,
	.make_unlit = callset_bank_top_make_unlit,
	.make_lit = callset_bank_top_make_lit,
	.make_flash = callset_bank_top_make_flash,
	.light_all = callset_bank_top_light_all,
	.flash_all = callset_bank_top_flash_all,
};


const bank_properties_t bank_properties_table[] = {
	[BANK_SKILL] = {
		.name = "SKILL 5-BANK",
		.gid = GID_BANK_SKILL,
		.sw_count = 5,
		.lamplist = LAMPLIST_SKILL,
	},

	[BANK_CENTER] = {
		.ops = &bank_center_ops,
		.name = "CENTER 3-BANK",
		.gid = GID_BANK_CENTER,
		.sw_count = 3,
		.switches = { SW_STANDUP_MID_LEFT, SW_STANDUP_MID_CENTER, SW_STANDUP_MID_RIGHT, },
		.lamplist = LAMPLIST_CENTER,
	},

	[BANK_RIGHT] = {
		.name = "RIGHT 3-BANK",
		.gid = GID_BANK_RIGHT,
		.sw_count = 3,
		.lamplist = LAMPLIST_RELIGHT_KICKBACK,
	},

	[BANK_TOP] = {
		.ops = &bank_top_ops,
		.name = "TOP LANES",
		.gid = GID_BANK_TOP,
		.sw_count = 3,
		.switches = { SW_TOP_LANE_LEFT, SW_TOP_LANE_CENTER, SW_TOP_LANE_RIGHT, },
		.lamplist = LAMPLIST_TOP_LANES,
	},
};


U8 bank_get_lamplist (const U8 bankno)
{
	return bank_properties_table[bankno].lamplist;
}


void bank_clear (const U8 bankno)
{
	const bank_properties_t *bank = &bank_properties_table[bankno];
	U8 i;
	for (i=0 ; i < bank->sw_count; i++)
		lamp_tristate_off (switch_lookup_lamp (bank->switches[i]));
}

void bank_flash (const U8 bankno)
{
	const bank_properties_t *bank = &bank_properties_table[bankno];
	U8 i;
	for (i=0 ; i < bank->sw_count; i++)
		lamp_tristate_flash (switch_lookup_lamp (bank->switches[i]));
}

void bank_set (const U8 bankno)
{
	const bank_properties_t *bank = &bank_properties_table[bankno];
	U8 i;
	for (i=0 ; i < bank->sw_count; i++)
		lamp_tristate_on (switch_lookup_lamp (bank->switches[i]));
}

bool bank_test_all (const U8 bankno)
{
	const bank_properties_t *bank = &bank_properties_table[bankno];
	U8 i;
	for (i=0 ; i < bank->sw_count; i++)
		if (!lamp_test (switch_lookup_lamp (bank->switches[i])))
			return FALSE;
	return TRUE;
}


U8 bank_active;
const switch_info_t *bank_swinfo;


void bank_target_off (void)
{
	lamp_tristate_off (bank_swinfo->lamp);
}


void bank_target_on (void)
{
	lamp_tristate_on (bank_swinfo->lamp);
	if (bank_test_all (bank_active))
	{
		dbprintf ("yep, completed it\n");
		far_indirect_call_handler (bank_properties_table[bank_active].ops->light_all, EVENT_PAGE);
	}
}


void bank_target_flash (void)
{
	lamp_tristate_flash (bank_swinfo->lamp);
}


/** Handle any switch that is part of a target bank. */
void bank_sw_handler (const U8 bankno)
{
	const bank_properties_t *bank = &bank_properties_table[bankno];

	/* Debounce hits to the entire target bank, not just to
	individual switches.  Each bank has its timer for this. */
	if (timer_find_gid (bank->gid))
	{
		dbprintf ("bank %d skipped debounce, gid %d\n", bankno, bank->gid);
		return;
	}
	else
		timer_restart_free (bank->gid, TIME_200MS);

	/* Lookup information on the specific switch that triggered */
	bank_swinfo = switch_lookup_current ();
	bank_active = bankno;

	/* Throw an event just saying that the bank was hit */
	far_indirect_call_handler (bank->ops->made, EVENT_PAGE);

	/* Throw an event based on the state of the lamp for the
	affected switch */
	if (lamp_flash_test (bank_swinfo->lamp))
	{
		dbprintf ("bank %d made flashing\n", bankno);
		far_indirect_call_handler (bank->ops->make_flash, EVENT_PAGE);
		dbprintf ("called %p\n", bank->ops->make_flash);
	}
	else if (lamp_test (bank_swinfo->lamp))
	{
		dbprintf ("bank %d made lit\n", bankno);
		far_indirect_call_handler (bank->ops->make_lit, EVENT_PAGE);
	}
	else
	{
		dbprintf ("bank %d made unlit\n", bankno);
		far_indirect_call_handler (bank->ops->make_unlit, EVENT_PAGE);
	}
}


CALLSET_ENTRY (centerbank, bank_center_make_lit)
{
	sound_send (SND_TARGET_1);
}


CALLSET_ENTRY (centerbank, bank_center_make_flash)
{
	sound_send (SND_TARGET_5);
	score (SC_25K);
	bank_target_on ();
}

CALLSET_ENTRY (centerbank, bank_center_light_all)
{
	bank_flash (BANK_CENTER);
	score (SC_250K);
	sound_send (SND_EXTRA_BALL_COLLECT);
}



CALLSET_ENTRY (bonus_x, bank_top_make_unlit)
{
	sound_send (SND_DATABASE_CLICK);
	score (SC_5K);
	bank_target_on ();
}


CALLSET_ENTRY (bonus_x, bank_top_light_all)
{
	sound_send (SND_COPTER_1);
	score (SC_100K);
	bank_clear (BANK_TOP);
}


CALLSET_ENTRY (centerbank, start_game)
{
	bank_flash (BANK_CENTER);
	bank_clear (BANK_TOP);
	bank_clear (BANK_RIGHT);
	bank_clear (BANK_SKILL);
}


CALLSET_ENTRY (lanes, sw_left_flipper)
{
	if (in_live_game)
	{
		lamplist_rotate_previous (LAMPLIST_TOP_LANES, lamp_matrix);
	}
}

CALLSET_ENTRY (lanes, sw_right_flipper)
{
	if (in_live_game)
	{
		lamplist_rotate_next (LAMPLIST_TOP_LANES, lamp_matrix);
	}
}

CALLSET_ENTRY (bank, sw_standup_mid_left) 
{ bank_sw_handler (BANK_CENTER); }
CALLSET_ENTRY (bank, sw_standup_mid_center) 
{ bank_sw_handler (BANK_CENTER); }
CALLSET_ENTRY (bank, sw_standup_mid_right) 
{ bank_sw_handler (BANK_CENTER); }

CALLSET_ENTRY (bank, sw_top_lane_left) 
{ bank_sw_handler (BANK_TOP); }
CALLSET_ENTRY (bank, sw_top_lane_center) 
{ bank_sw_handler (BANK_TOP); }
CALLSET_ENTRY (bank, sw_top_lane_right) 
{ bank_sw_handler (BANK_TOP); }

