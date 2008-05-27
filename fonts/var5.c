
#include <freewpc.h>

static char font_var5x5_alpha_cap[][7] = {
	{ 4, 5, 6, 9, 15, 9, 9 },
	{ 4, 5, 7, 9, 7, 9, 7 },
	{ 3, 5, 6, 1, 1, 1, 6 },
	{ 4, 5, 7, 9, 9, 9, 7 },
	{ 3, 5, 7, 1, 7, 1, 7 },

	{ 3, 5, 7, 1, 7, 1, 1 },
	{ 4, 5, 14, 1, 13, 9, 6 },
	{ 4, 5, 9, 9, 15, 9, 9 },
	{ 3, 5, 7, 2, 2, 2, 7 },
	{ 4, 5, 14, 4, 4, 5, 2 },

	{ 4, 5, 9, 5, 3, 5, 9 },
	{ 3, 5, 1, 1, 1, 1, 7 },
	{ 5, 5, 17, 27, 21, 17, 17 },
	{ 4, 5, 9, 11, 13, 9, 9 },
	{ 4, 5, 6, 9, 9, 9, 6 },
	
	{ 4, 5, 7, 9, 7, 1, 1 },
	{ 5, 5, 14, 17, 21, 25, 30 },
	{ 4, 5, 7, 9, 7, 5, 9 },
	{ 4, 5, 6, 1, 6, 8, 6 },
	{ 3, 5, 7, 2, 2, 2, 2 },

	{ 4, 5, 9, 9, 9, 9, 6 },
	{ 3, 5, 5, 5, 5, 2, 2 },
	{ 5, 5, 17, 17, 21, 21, 14 },
	{ 3, 5, 5, 5, 2, 5, 5 },
	{ 3, 5, 5, 5, 2, 2, 2 },
	{ 4, 5, 15, 8, 4, 2, 15 },
};

static char font_var5x5_digit[][7] = {
	{ 3, 5, 2, 5, 5, 5, 2 },
	{ 3, 5, 2, 3, 2, 2, 7 },
	{ 4, 5, 6, 9, 6, 1, 15 },
	{ 3, 5, 3, 4, 3, 4, 3 },
	{ 4, 5, 5, 5, 15, 4, 4 },
	{ 4, 5, 15, 1, 7, 8, 7 },
	{ 4, 5, 6, 1, 7, 9, 6 },
	{ 4, 5, 15, 8, 4, 2, 1 },
	{ 4, 5, 6, 9, 6, 9, 6 },
	{ 4, 5, 6, 9, 14, 8, 6 },
};

static char font_var5x5_sep[][7] = {
	/* period */ { 2, 5, 0, 0, 0, 0, 2 },
	/* comma */ { 2, 5, 0, 0, 0, 2, 1 },
	/* slash */ { 5, 5, 16, 8, 4, 2, 1 },
	/* colon */ { 1, 5, 0, 1, 0, 1, 0 },
	/* hyphen */ { 3, 5, 0, 0, 7, 0, 0 },
	/* percent */ { 5, 5, 18, 8, 4, 2, 9 },
	/* equal */ { 3, 5, 0, 7, 0, 7, 0 },
	/* lparen */ { 2, 5, 2, 1, 1, 1, 2 },
	/* rparen */ { 2, 5, 1, 2, 2, 2, 1 },
};

static char *var5_glyphs[] = {
   ['A'] = font_var5x5_alpha_cap[0],
   ['B'] = font_var5x5_alpha_cap[1],
   ['C'] = font_var5x5_alpha_cap[2],
   ['D'] = font_var5x5_alpha_cap[3],
   ['E'] = font_var5x5_alpha_cap[4],
   ['F'] = font_var5x5_alpha_cap[5],
   ['G'] = font_var5x5_alpha_cap[6],
   ['H'] = font_var5x5_alpha_cap[7],
   ['I'] = font_var5x5_alpha_cap[8],
   ['J'] = font_var5x5_alpha_cap[9],
   ['K'] = font_var5x5_alpha_cap[10],
   ['L'] = font_var5x5_alpha_cap[11],
   ['M'] = font_var5x5_alpha_cap[12],
   ['N'] = font_var5x5_alpha_cap[13],
   ['O'] = font_var5x5_alpha_cap[14],
   ['P'] = font_var5x5_alpha_cap[15],
   ['Q'] = font_var5x5_alpha_cap[16],
   ['R'] = font_var5x5_alpha_cap[17],
   ['S'] = font_var5x5_alpha_cap[18],
   ['T'] = font_var5x5_alpha_cap[19],
   ['U'] = font_var5x5_alpha_cap[20],
   ['V'] = font_var5x5_alpha_cap[21],
   ['W'] = font_var5x5_alpha_cap[22],
   ['X'] = font_var5x5_alpha_cap[23],
   ['Y'] = font_var5x5_alpha_cap[24],
   ['Z'] = font_var5x5_alpha_cap[25],
   ['0'] = font_var5x5_digit[0],
   ['1'] = font_var5x5_digit[1],
   ['2'] = font_var5x5_digit[2],
   ['3'] = font_var5x5_digit[3],
   ['4'] = font_var5x5_digit[4],
   ['5'] = font_var5x5_digit[5],
   ['6'] = font_var5x5_digit[6],
   ['7'] = font_var5x5_digit[7],
   ['8'] = font_var5x5_digit[8],
   ['9'] = font_var5x5_digit[9],
   ['.'] = font_var5x5_sep[0],
   [','] = font_var5x5_sep[1],
   ['/'] = font_var5x5_sep[2],
   [':'] = font_var5x5_sep[3],
   ['-'] = font_var5x5_sep[4],
   ['%'] = font_var5x5_sep[5],
	['='] = font_var5x5_sep[6],
	['('] = font_var5x5_sep[7],
	[')'] = font_var5x5_sep[8],
};

const font_t font_var5 = {
	.spacing = 1,
	.height = 5, 
	.glyphs = var5_glyphs,
};


