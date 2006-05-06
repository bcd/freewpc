
#include <freewpc.h>

static const uint8_t font_9x6_alpha_cap[][11] = {
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


static const uint8_t font_9x6_digit[][11] = {
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


static const uint8_t font_9x6_sep[][11] = {
	{ 6, 9, 0, 0, 0, 0, 0, 0, 0, 0, 2 },
	{ 6, 9, 0, 0, 0, 0, 0, 0, 2, 2, 1 },
	{ 6, 9, 16, 16, 8, 8, 4, 4, 2, 2, 0 },
};


static unsigned char *mono9_glyphs[] = {
   ['A'] = (unsigned char *)font_9x6_alpha_cap[0],
   ['B'] = (unsigned char *)font_9x6_alpha_cap[1],
   ['C'] = (unsigned char *)font_9x6_alpha_cap[2],
   ['D'] = (unsigned char *)font_9x6_alpha_cap[3],
   ['E'] = (unsigned char *)font_9x6_alpha_cap[4],
   ['F'] = (unsigned char *)font_9x6_alpha_cap[5],
   ['G'] = (unsigned char *)font_9x6_alpha_cap[6],
   ['H'] = (unsigned char *)font_9x6_alpha_cap[7],
   ['I'] = (unsigned char *)font_9x6_alpha_cap[8],
   ['J'] = (unsigned char *)font_9x6_alpha_cap[9],
   ['K'] = (unsigned char *)font_9x6_alpha_cap[10],
   ['L'] = (unsigned char *)font_9x6_alpha_cap[11],
   ['M'] = (unsigned char *)font_9x6_alpha_cap[12],
   ['N'] = (unsigned char *)font_9x6_alpha_cap[13],
   ['O'] = (unsigned char *)font_9x6_alpha_cap[14],
   ['P'] = (unsigned char *)font_9x6_alpha_cap[15],
   ['Q'] = (unsigned char *)font_9x6_alpha_cap[16],
   ['R'] = (unsigned char *)font_9x6_alpha_cap[17],
   ['S'] = (unsigned char *)font_9x6_alpha_cap[18],
   ['T'] = (unsigned char *)font_9x6_alpha_cap[19],
   ['U'] = (unsigned char *)font_9x6_alpha_cap[20],
   ['V'] = (unsigned char *)font_9x6_alpha_cap[21],
   ['W'] = (unsigned char *)font_9x6_alpha_cap[22],
   ['X'] = (unsigned char *)font_9x6_alpha_cap[23],
   ['Y'] = (unsigned char *)font_9x6_alpha_cap[24],
   ['Z'] = (unsigned char *)font_9x6_alpha_cap[25],
   ['0'] = (unsigned char *)font_9x6_digit[0],
   ['1'] = (unsigned char *)font_9x6_digit[1],
   ['2'] = (unsigned char *)font_9x6_digit[2],
   ['3'] = (unsigned char *)font_9x6_digit[3],
   ['4'] = (unsigned char *)font_9x6_digit[4],
   ['5'] = (unsigned char *)font_9x6_digit[5],
   ['6'] = (unsigned char *)font_9x6_digit[6],
   ['7'] = (unsigned char *)font_9x6_digit[7],
   ['8'] = (unsigned char *)font_9x6_digit[8],
   ['9'] = (unsigned char *)font_9x6_digit[9],
   ['.'] = (unsigned char *)font_9x6_sep[0],
   [','] = (unsigned char *)font_9x6_sep[1],
   ['/'] = (unsigned char *)font_9x6_sep[2],
};

const font_t font_mono9 = {
	.spacing = 2,
	.height = 9, 
	.glyphs = mono9_glyphs,
};


