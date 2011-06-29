
#include <freewpc.h>

#ifdef CONFIG_MONO9_ALPHA
static char font_9x6_alpha_cap[][11] = {
	{ 6, 9, 4, 4, 10, 10, 31, 31, 17, 17, 17 },
	{ 6, 9, 7, 7, 9, 9, 7, 9, 9, 7, 7 },
	{ 6, 9, 14, 14, 1, 1, 1, 1, 1, 14, 14 },
	{ 6, 9, 7, 9, 17, 9, 7 },
	{ 6, 9, 31, 31, 1, 31, 31, 1, 1, 31, 31 },

	{ 6, 9, 31, 31, 1, 1, 31, 31, 1, 1, 1 },
	{ 6, 9, 14, 1, 25, 9, 15 },
	{ 6, 9, 17, 17, 17, 31, 31, 17, 17, 17, 17 },
	{ 6, 9, 31, 31, 4, 4, 4, 4, 4, 31, 31 },
	{ 6, 9, 15, 4, 4, 5, 2 },

	{ 6, 9, 17, 9, 7, 9, 17 },
	{ 6, 9, 1, 1, 1, 1, 1, 1, 1, 31, 31 },
	{ 6, 9, 17, 27, 21, 17, 17 },
	{ 6, 9, 17, 19, 21, 25, 17 },
	{ 6, 9, 14, 14, 17, 17, 17, 17, 17, 14, 14 },
	
	{ 6, 9, 15, 15, 17, 17, 15, 15, 1, 1, 1 },
	{ 6, 9, 14, 17, 21, 25, 30 },
	{ 6, 9, 15, 17, 15, 9, 17 },
	{ 6, 9, 14, 14, 1, 1, 14, 16, 16, 14, 14 },
	{ 6, 9, 31, 31, 4, 4, 4, 4, 4, 4, 4 },

	{ 6, 9, 17, 17, 17, 17, 17, 17, 17, 14, 14 },
	{ 6, 9, 17, 17, 17, 17, 10, 10, 10, 4, 4 },
	{ 6, 9, 17, 17, 21, 21, 14 },
	{ 6, 9, 17, 17, 10, 10, 4, 10, 10, 17, 17 },
	{ 6, 9, 17, 17, 17, 10, 10, 4, 4, 4, 4 },
	{ 6, 9, 31, 31, 8, 8, 4, 2, 2, 31, 31 },
};
#endif

static char font_9x6_digit[][11] = {
	{ 6, 9, 12, 30, 51, 51, 51, 51, 51, 30, 12 },
	{ 6, 9, 12, 12, 14, 14, 12, 12, 12, 30, 30 },
	{ 6, 9, 30, 63, 48, 24, 12, 6, 3, 63, 63 },
	{ 6, 9, 30, 62, 48, 30, 62, 48, 48, 62, 30 },
	{ 6, 9, 27, 27, 27, 63, 63, 24, 24, 24, 24 },
	{ 6, 9, 63, 63, 3, 15, 31, 48, 48, 31, 15 },
	{ 6, 9, 12, 14, 3, 31, 63, 51, 51, 30, 30 },
	{ 6, 9, 63, 63, 48, 24, 12, 6, 3, 3, 3 },
	{ 6, 9, 12, 30, 51, 51, 30, 51, 51, 30, 12 },
	{ 6, 9, 12, 30, 51, 51, 62, 48, 48, 30, 12 },
};


static char font_9x6_sep[][11] = {
	/* period */ { 3, 9, 0, 0, 0, 0, 0, 0, 0, 0, 2 },
	/* comma */ { 3, 9, 0, 0, 0, 0, 0, 0, 2, 2, 1 },
	/* slash */ { 6, 9, 16, 16, 8, 8, 4, 4, 2, 2, 0 },
};

#define BASECHAR ' '

static char *mono9_glyphs[] = {
#ifdef CONFIG_MONO9_ALPHA
   ['A' - BASECHAR] = font_9x6_alpha_cap[0],
   ['B' - BASECHAR] = font_9x6_alpha_cap[1],
   ['C' - BASECHAR] = font_9x6_alpha_cap[2],
   ['D' - BASECHAR] = font_9x6_alpha_cap[3],
   ['E' - BASECHAR] = font_9x6_alpha_cap[4],
   ['F' - BASECHAR] = font_9x6_alpha_cap[5],
   ['G' - BASECHAR] = font_9x6_alpha_cap[6],
   ['H' - BASECHAR] = font_9x6_alpha_cap[7],
   ['I' - BASECHAR] = font_9x6_alpha_cap[8],
   ['J' - BASECHAR] = font_9x6_alpha_cap[9],
   ['K' - BASECHAR] = font_9x6_alpha_cap[10],
   ['L' - BASECHAR] = font_9x6_alpha_cap[11],
   ['M' - BASECHAR] = font_9x6_alpha_cap[12],
   ['N' - BASECHAR] = font_9x6_alpha_cap[13],
   ['O' - BASECHAR] = font_9x6_alpha_cap[14],
   ['P' - BASECHAR] = font_9x6_alpha_cap[15],
   ['Q' - BASECHAR] = font_9x6_alpha_cap[16],
   ['R' - BASECHAR] = font_9x6_alpha_cap[17],
   ['S' - BASECHAR] = font_9x6_alpha_cap[18],
   ['T' - BASECHAR] = font_9x6_alpha_cap[19],
   ['U' - BASECHAR] = font_9x6_alpha_cap[20],
   ['V' - BASECHAR] = font_9x6_alpha_cap[21],
   ['W' - BASECHAR] = font_9x6_alpha_cap[22],
   ['X' - BASECHAR] = font_9x6_alpha_cap[23],
   ['Y' - BASECHAR] = font_9x6_alpha_cap[24],
   ['Z' - BASECHAR] = font_9x6_alpha_cap[25],
#endif
   ['0' - BASECHAR] = font_9x6_digit[0],
   ['1' - BASECHAR] = font_9x6_digit[1],
   ['2' - BASECHAR] = font_9x6_digit[2],
   ['3' - BASECHAR] = font_9x6_digit[3],
   ['4' - BASECHAR] = font_9x6_digit[4],
   ['5' - BASECHAR] = font_9x6_digit[5],
   ['6' - BASECHAR] = font_9x6_digit[6],
   ['7' - BASECHAR] = font_9x6_digit[7],
   ['8' - BASECHAR] = font_9x6_digit[8],
   ['9' - BASECHAR] = font_9x6_digit[9],
   ['.' - BASECHAR] = font_9x6_sep[0],
   [',' - BASECHAR] = font_9x6_sep[1],
   ['/' - BASECHAR] = font_9x6_sep[2],
};

const font_t font_mono9 = {
	.spacing = 2,
	.height = 9, 
	.glyphs = mono9_glyphs,
	.basechar = BASECHAR,
};


