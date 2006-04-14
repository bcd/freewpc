
#include <freewpc.h>

#pragma section ("page61")

static const uint8_t font_5x5_alpha_cap[][7] = {
	{ 5, 5, 4, 10, 31, 17, 17 },
	{ 5, 5, 7, 9, 7, 9, 7 },
	{ 5, 5, 14, 1, 1, 1, 14 },
	{ 5, 5, 7, 9, 17, 9, 7 },
	{ 5, 5, 31, 1, 31, 1, 31 },

	{ 5, 5, 31, 1, 31, 1, 1 },
	{ 5, 5, 14, 1, 29, 9, 14 },
	{ 5, 5, 17, 17, 31, 17, 17 },
	{ 5, 5, 31, 4, 4, 4, 31 },
	{ 5, 5, 31, 4, 4, 5, 2 },

	{ 5, 5, 17, 9, 7, 9, 17 },
	{ 5, 5, 1, 1, 1, 1, 31 },
	{ 5, 5, 17, 27, 21, 17, 17 },
	{ 5, 5, 17, 19, 21, 25, 17 },
	{ 5, 5, 14, 17, 17, 17, 14 },
	
	{ 5, 5, 15, 17, 15, 1, 1 },
	{ 5, 5, 14, 17, 21, 25, 30 },
	{ 5, 5, 15, 17, 15, 9, 17 },
	{ 5, 5, 14, 1, 14, 16, 14 },
	{ 5, 5, 31, 4, 4, 4, 4 },

	{ 5, 5, 17, 17, 17, 17, 14 },
	{ 5, 5, 17, 17, 10, 10, 4 },
	{ 5, 5, 17, 17, 21, 21, 14 },
	{ 5, 5, 17, 10, 4, 10, 17 },
	{ 5, 5, 17, 10, 4, 4, 4 },
	{ 5, 5, 31, 8, 4, 2, 31 },
};

static const uint8_t font_5x5_digit[][7] = {
	{ 5, 5, 14, 17, 17, 17, 14 },
	{ 5, 5, 4, 6, 4, 4, 14 },
	{ 5, 5, 14, 17, 14, 1, 31 },
	{ 5, 5, 14, 16, 14, 16, 14 },
	{ 5, 5, 5, 5, 31, 4, 4 },
	{ 5, 5, 31, 1, 15, 16, 15 },
	{ 5, 5, 14, 1, 15, 17, 14 },
	{ 5, 5, 31, 8, 4, 2, 1 },
	{ 5, 5, 14, 17, 14, 17, 14 },
	{ 5, 5, 14, 17, 30, 16, 14 },
};

static const uint8_t font_5x5_sep[][7] = {
	/* period */ { 3, 5, 0, 0, 0, 0, 2 },
	/* comma */ { 3, 5, 0, 0, 0, 2, 1 },
	/* slash */ { 5, 5, 16, 8, 4, 2, 1 },
	/* colon */ { 3, 5, 0, 2, 0, 2, 0 },
	/* hyphen */ { 3, 5, 0, 0, 7, 0, 0 },
};

static unsigned char *mono5_glyphs[] = {
   ['A'] = (unsigned char *)font_5x5_alpha_cap[0],
   ['B'] = (unsigned char *)font_5x5_alpha_cap[1],
   ['C'] = (unsigned char *)font_5x5_alpha_cap[2],
   ['D'] = (unsigned char *)font_5x5_alpha_cap[3],
   ['E'] = (unsigned char *)font_5x5_alpha_cap[4],
   ['F'] = (unsigned char *)font_5x5_alpha_cap[5],
   ['G'] = (unsigned char *)font_5x5_alpha_cap[6],
   ['H'] = (unsigned char *)font_5x5_alpha_cap[7],
   ['I'] = (unsigned char *)font_5x5_alpha_cap[8],
   ['J'] = (unsigned char *)font_5x5_alpha_cap[9],
   ['K'] = (unsigned char *)font_5x5_alpha_cap[10],
   ['L'] = (unsigned char *)font_5x5_alpha_cap[11],
   ['M'] = (unsigned char *)font_5x5_alpha_cap[12],
   ['N'] = (unsigned char *)font_5x5_alpha_cap[13],
   ['O'] = (unsigned char *)font_5x5_alpha_cap[14],
   ['P'] = (unsigned char *)font_5x5_alpha_cap[15],
   ['Q'] = (unsigned char *)font_5x5_alpha_cap[16],
   ['R'] = (unsigned char *)font_5x5_alpha_cap[17],
   ['S'] = (unsigned char *)font_5x5_alpha_cap[18],
   ['T'] = (unsigned char *)font_5x5_alpha_cap[19],
   ['U'] = (unsigned char *)font_5x5_alpha_cap[20],
   ['V'] = (unsigned char *)font_5x5_alpha_cap[21],
   ['W'] = (unsigned char *)font_5x5_alpha_cap[22],
   ['X'] = (unsigned char *)font_5x5_alpha_cap[23],
   ['Y'] = (unsigned char *)font_5x5_alpha_cap[24],
   ['Z'] = (unsigned char *)font_5x5_alpha_cap[25],
   ['0'] = (unsigned char *)font_5x5_digit[0],
   ['1'] = (unsigned char *)font_5x5_digit[1],
   ['2'] = (unsigned char *)font_5x5_digit[2],
   ['3'] = (unsigned char *)font_5x5_digit[3],
   ['4'] = (unsigned char *)font_5x5_digit[4],
   ['5'] = (unsigned char *)font_5x5_digit[5],
   ['6'] = (unsigned char *)font_5x5_digit[6],
   ['7'] = (unsigned char *)font_5x5_digit[7],
   ['8'] = (unsigned char *)font_5x5_digit[8],
   ['9'] = (unsigned char *)font_5x5_digit[9],
   ['.'] = (unsigned char *)font_5x5_sep[0],
   [','] = (unsigned char *)font_5x5_sep[1],
   ['/'] = (unsigned char *)font_5x5_sep[2],
   [':'] = (unsigned char *)font_5x5_sep[3],
   ['-'] = (unsigned char *)font_5x5_sep[4],
};

const font_t font_mono5 = {
	.spacing = 2,
	.height = 5, 
	.glyphs = mono5_glyphs,
};


