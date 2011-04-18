
#include <freewpc.h>


#define MF_TIMED 0
#define MF_MB 1
#define MF_FLASH_SCORE 0x2
#define MF_FLASH_HELP 0x4
#define MF_HELP_DYNAMIC 0x8
#define MF_OTHER_SCORE 0x10

struct mode_deff
{
	U8 flags;
	const char *title;
	union
	{
		const char *fixed;
		void (*dynamic) (void);
	} msg;
	union
	{
		struct timed_mode_ops *timed;
		struct mb_mode_ops *mb;
	} mode;
	score_pointer_t other_score;
};

U8 deff_loop_count;

const char *generic_mode_help (struct mode_deff *mdeff)
{
	if (mdeff->flags & MF_HELP_DYNAMIC)
	{
		mdeff->msg.dynamic ();
		return sprintf_buffer;
	}
	else
		return mdeff->msg.fixed;
}

void generic_mode_deff (struct mode_deff *mdeff)
{
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_mono5, 64, 4, mdeff->title);
	if (!(mdeff->flags & MF_HELP_DYNAMIC))
		font_render_string_center (&font_var5, 64, 28, generic_mode_help (mdeff));

	for (;;)
	{
		++deff_loop_count;
		dmd_alloc_low_clean ();
		dmd_overlay ();

		if (!(mdeff->flags & MF_FLASH_SCORE) || (deff_loop_count % 2))
		{
			if (mdeff->flags & MF_OTHER_SCORE)
				sprintf_score (mdeff->other_score);
			else
				sprintf_current_score ();
			font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
		}

		if (mdeff->flags & MF_FLASH_HELP && (deff_loop_count % 2))
			dmd_rough_erase (0, 26, 128, 5);
		else if (mdeff->flags & MF_HELP_DYNAMIC)
			font_render_string_center (&font_var5, 64, 28, generic_mode_help (mdeff));

		if (!(mdeff->flags & MF_MB))
		{
			sprintf ("%d", *mdeff->mode.timed->timer);
			font_render_string_left (&font_var5, 2, 2, sprintf_buffer);
			font_render_string_right (&font_var5, 126, 2, sprintf_buffer);
		}
		dmd_show_low ();
		task_sleep (TIME_200MS);
	}
}


void generic_quick_deff (void (*draw) (void))
{
	dmd_alloc_low_clean ();
	draw ();
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


void stub_deff (void)
{
	dmd_alloc_low_clean ();
	sprintf_far_string (names_of_deffs + deff_get_active ());
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_1500MS);
	deff_exit ();
}


void generic_score_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 6, sprintf_buffer);
	sprintf_score (score_deff_get ());
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}


void generic_jackpot_deff (const char *msg, U8 flashes)
{
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed6, 64, 7, msg);
	sprintf_score (score_deff_get ());
	font_render_string_center (&font_fixed10, 64, 22, sprintf_buffer);
	dmd_copy_low_to_high ();
	dmd_invert_page (dmd_low_buffer);
	while (flashes > 0)
	{
		dmd_show_low ();
		task_sleep (TIME_133MS);
		dmd_show_high ();
		task_sleep (TIME_133MS);
		flashes--;
	}
	task_sleep (TIME_1500MS);
	deff_exit ();
}


void dmd_fill_bg (void)
{
	U16 *dmd = (U16 *)dmd_low_buffer;
	U16 val = 0x5555;
	U16 rows = 32;
	while (rows > 0)
	{
		*dmd++ = val;
		*dmd++ = val;
		*dmd++ = val;
		*dmd++ = val;
		*dmd++ = val;
		*dmd++ = val;
		*dmd++ = val;
		*dmd++ = val;
		val = ~val;
		rows--;
	}
}


U16 diamond_bmp[] = {
	0x0180,
	0x0240,
	0x0420,
	0x0810,
	0x1008,
	0x2004,
	0x4002,
	0x8001,
	0xBEED,
	0x4002,
	0x2004,
	0x1008,
	0x0810,
	0x0420,
	0x0240,
	0x0180,
	0xDEAD,
};

void draw_diamond_border (U16 *dst)
{
	U16 *src = diamond_bmp;
	while (*src != 0xDEAD)
	{
		if (*src != 0xBEED)
		{
			*dst = *src;
			dst += 8;
		}
		src++;
	}
}

void draw_diamond_full (U16 *dst)
{
	U16 *src = diamond_bmp;
	U16 val = 0;
	while (*src != 0xBEED)
	{
		val |= *src;
		*dst = val;
		dst += 8;
		src++;
	}
	src++;
	val &= ~0x8001;
	while (*src != 0xDEAD)
	{
		*dst = val;
		val &= ~*src;
		dst += 8;
		src++;
	}
}


void test_bg_deff (void)
{
	dmd_map_overlay ();
	dmd_clean_page_low ();
	font_render_string_center (&font_fixed10, 64, 16, "MEGA JACKPOT");
	dmd_text_outline ();

	dmd_alloc_pair ();
	dmd_fill_bg ();
	dmd_clean_page_high ();
	dmd_overlay_outline ();
	dmd_show2 ();
	task_sleep_sec (3);
	deff_exit ();
}

//void jackpot_deff (void) { test_bg_deff (); }
void jackpot_deff (void) { generic_jackpot_deff ("JACKPOT", 1); }
void double_jackpot_deff (void) { generic_jackpot_deff ("DOUBLE JACKPOT", 2); }
void super_jackpot_deff (void) { generic_jackpot_deff ("SUPER JACKPOT", 10); }

extern struct timed_mode_ops *double_score_mode;
struct mode_deff double_scoring_deff_config = {
	MF_TIMED,
	"DOUBLE SCORING", { .fixed = "ALL SHOTS SCORE 2X" }, { .timed = &double_score_mode }
};
void double_scoring_deff (void) { generic_mode_deff (&double_scoring_deff_config); }


extern struct mb_mode_ops *quick_mb_mode;
struct mode_deff quick_mb_deff_config = {
	MF_MB,
	"QUICK MULTIBALL", { .fixed = "ALL SHOTS = 25,000" }, { .mb = &quick_mb_mode }
};
void quick_mb_deff (void) { generic_mode_deff (&quick_mb_deff_config); }


extern struct mb_mode_ops *trickmb_mode;
struct mode_deff trick_mb_deff_config = {
	MF_MB,
	"TRICK MULTIBALL", { .fixed = "SHOOT JACKPOTS" }, { .mb = &trickmb_mode }
};
void trick_mb_deff (void) { generic_mode_deff (&trick_mb_deff_config); }


extern struct mb_mode_ops *trickmb_mode;
struct mode_deff super_trick_mb_deff_config = {
	MF_MB,
	"SUPER TRICK", { .fixed = "ALL JACKPOTS 2X" }, { .mb = &trickmb_mode }
};
void super_trick_mb_deff (void) { generic_mode_deff (&super_trick_mb_deff_config); }


extern struct mb_mode_ops *main_mb_mode;
struct mode_deff main_mb_deff_config = {
	MF_MB,
	"MULTIBALL", { .fixed = "SHOOT THE TRUNK" }, { .mb = &main_mb_mode }
};
void main_mb_deff (void) { generic_mode_deff (&trick_mb_deff_config); }

extern struct mb_mode_ops *midnight_mb_mode;
// Alternate between:
// ALL SWITCHES = %8b
// %d HITS TO INCREASE VALUE
// JACKPOT = %8b
struct mode_deff midnight_mb_deff_config = {
	MF_MB,
	"MIDNIGHT MULTIBALL", { .fixed = "ALL SWITCHES SCORE" }, { .mb = &main_mb_mode }
};
void midnight_mb_deff (void) { generic_mode_deff (&midnight_mb_deff_config); }


extern struct timed_mode_ops *hurryup_mode;
extern score_t hurryup_value;
struct mode_deff hurryup_deff_config = {
	MF_TIMED | MF_FLASH_HELP | MF_OTHER_SCORE,
	"HURRY UP", { .fixed = "SHOOT THE TRUNK" }, { .timed = &hurryup_mode },
	hurryup_value
};
void hurryup_deff (void) { generic_mode_deff (&hurryup_deff_config); }


void theatre_letter_draw (void)
{
	U8 count;
	deff_data_pull (count);

	sprintf ("THEATRE %d", count);
	font_render_string_center (&font_var5, 64, 6, sprintf_buffer);
	sprintf_score (score_deff_get ());
	font_render_string_center (&font_fixed6, 64, 16, sprintf_buffer);
	if (count < 7)
		font_render_string_center (&font_var5, 64, 28, "COMPLETE TO START HURRYUP");
	else
		font_render_string_center (&font_var5, 64, 28, "HURRYUP IS LIT");
}
void theatre_letter_deff (void) { generic_quick_deff (theatre_letter_draw); }


void magic_letter_draw (void)
{
	U8 count;
	deff_data_pull (count);

	sprintf ("MAGIC", count);
	font_render_string_center (&font_fixed10, 64, 11, sprintf_buffer);
	if (count < 5)
		font_render_string_center (&font_var5, 64, 28, "COMPLETE TO LIGHT LOCK");
	else
		font_render_string_center (&font_var5, 64, 28, "LOCK IS LIT");
	//font_render_string_center (&font_var5, 64, 28, "MULTIBALL IS LIT");
}
void magic_letter_deff (void) { generic_quick_deff (magic_letter_draw); }



void trick_shot_deff (void)
{
	dmd_alloc_low_clean ();
	font_render_string_center (&font_var5, 64, 4, "MODE NAME");
	sprintf_current_score ();
	font_render_string_center (&font_bitmap8, 64, 16, sprintf_buffer);
	font_render_string_center (&font_var5, 64, 28, "N MORE SHOTS TO COMPLETE");
	//font_render_string_center (&font_var5, 64, 28, "TRICK COMPLETED");
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void bonusx_deff (void)
{
	U8 bonusx;
	deff_data_pull (bonusx);

	dmd_alloc_low_clean ();
	sprintf ("BONUS %dX", bonusx);
	font_render_string_center (&font_retroville, 64, 16, sprintf_buffer);
	dmd_show_low ();
	task_sleep (TIME_1500MS);
	deff_exit ();
}

void random_award_deff (void)
{
	extern const char *rnd_choice;
	dmd_alloc_low_clean ();
	font_render_string_center (&font_retroville, 64, 9, "MYSTERY");
	dmd_show_low ();
	task_sleep (TIME_1500MS);
	dmd_alloc_low_clean ();
	font_render_string_center (&font_fixed6, 64, 16, rnd_choice);
	dmd_sched_transition (&trans_random_boxfade);
	dmd_show_low ();
	task_sleep_sec (2);
	deff_exit ();
}

void badge_awarded_deff (void)
{
	dmd_buffer_t addr;

	dmd_alloc_pair ();
	dmd_clean_page_low ();
	addr = dmd_low_buffer + DMD_BYTE_WIDTH*17UL + 3;
	draw_diamond_border ((U16 *)addr);
	draw_diamond_full ((U16 *)(addr+3));
	draw_diamond_full ((U16 *)(addr+6));
	draw_diamond_border ((U16 *)(addr+9));
	font_render_string_center (&font_fixed6, 64, 5, "ACT I COMPLETE");
	dmd_copy_low_to_high ();
	font_render_string_center (&font_var5, 64, 13, "COMPLETE ALL FOR FINALE");
	deff_swap_low_high (64, TIME_66MS);
	deff_exit ();
}

void dmd_dup_partial (void)
{
	dmd_pagenum_t tmp;

	dmd_dup_mapped ();
#if 0
	tmp = dmd_low_page;

	pinio_dmd_window_set (PINIO_DMD_WINDOW_0, DMD_OVERLAY_PAGE+1);
	dmd_rough_copy (64 - 8, 16 - 2, 8*2, 2*2);

	pinio_dmd_window_set (PINIO_DMD_WINDOW_0, DMD_OVERLAY_PAGE);
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, tmp);
	dmd_rough_copy (64 - 8, 16 - 2, 8*2, 2*2);

	pinio_dmd_window_set (PINIO_DMD_WINDOW_0, tmp);
	pinio_dmd_window_set (PINIO_DMD_WINDOW_1, tmp+1);
#endif
	dmd_show2 ();
}


void dmd_dither_prepare (void)
{
	U16 *src = (U16 *)dmd_low_buffer;
	U16 *dst = (U16 *)dmd_high_buffer;
	U16 mask, tmp;
	U16 count = 0;
	mask = 0x5555;
	while (src < dmd_low_buffer + DMD_PAGE_SIZE - 4)
	{
		tmp = src[0];
		src[0] &= mask;
		dst[0] = tmp & ~mask;
		src++;
		dst++;

		if (++count == 8)
		{
			mask = ~mask;
			count = 0;
		}
	}
}


void dmd_dither (void)
{
	dmd_show_low ();
	task_sleep (TIME_66MS);
	dmd_show2 ();
	task_sleep (TIME_166MS);
	dmd_show_high ();
	task_sleep (TIME_66MS);
	dmd_show2 ();
	task_sleep (TIME_166MS);
	dmd_flip_low_high ();
}


void deff_explode_out (void)
{
	dmd_dup_partial ();
}

void clock_update_deff (void)
{
	extern U8 clock_hour, clock_min;

#if 0
	dmd_map_overlay ();
	dmd_clean_page_low ();
#endif
	dmd_alloc_pair ();
	dmd_clean_page_low ();
	font_render_string_center (&font_mono5, 64, 5, "THE TIME IS");
	sprintf ("%d:%02d", clock_hour ? clock_hour : 12, clock_min);
	font_render_string_center (&font_fixed10, 64, 16, sprintf_buffer);
	dmd_dither_prepare ();
	dmd_show2 ();
	task_sleep_sec (2);
	deff_exit ();
}

void skill_shot_running_deff (void)
{
}

