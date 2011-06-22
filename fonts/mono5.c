
#include <freewpc.h>

static char font_5x5_alpha_cap[][7] = {
	{ 5, 5, 4, 10, 31, 17, 17 },
	{ 5, 5, 7, 9, 7, 9, 7 },
	{ 5, 5, 14, 1, 1, 1, 14 },
	{ 5, 5, 15, 17, 17, 17, 15 },
	{ 5, 5, 31, 1, 31, 1, 31 },

	{ 5, 5, 31, 1, 31, 1, 1 },
	{ 5, 5, 14, 1, 13, 9, 14 },
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

static char font_5x5_digit[][7] = {
	{ 5, 5, 14, 17, 17, 17, 14 },
	{ 5, 5, 4, 6, 4, 4, 14 },
	{ 5, 5, 14, 17, 14, 1, 31 },
	{ 4, 5, 7, 8, 7, 8, 7 },
	{ 5, 5, 5, 5, 31, 4, 4 },
	{ 5, 5, 31, 1, 15, 16, 15 },
	{ 5, 5, 14, 1, 15, 17, 14 },
	{ 5, 5, 31, 8, 4, 2, 1 },
	{ 5, 5, 14, 17, 14, 17, 14 },
	{ 5, 5, 14, 17, 30, 16, 14 },
};

 
static char font_5x5_sep[][7] = {
	/* period */ { 2, 5, 0, 0, 0, 0, 2 },
	/* comma */ { 2, 5, 0, 0, 0, 2, 1 },
	/* slash */ { 5, 5, 16, 8, 4, 2, 1 },
	/* colon */ { 1, 5, 0, 1, 0, 1, 0 },
	/* hyphen */ { 3, 5, 0, 0, 7, 0, 0 },
	/* percent */ { 5, 5, 18, 8, 4, 2, 9 },
	/* dollar sign */ { 5, 5, 14, 5, 14, 20, 14 },
	/* pound sign */ { 5, 5, 14, 18, 7, 2, 31 },
	/* Euro sign */ { 5, 5, 28, 2, 7, 2, 28 },
};

#define BASECHAR ' '

static char *mono5_glyphs[] = {
   ['A' - BASECHAR] = font_5x5_alpha_cap[0],
   ['B' - BASECHAR] = font_5x5_alpha_cap[1],
   ['C' - BASECHAR] = font_5x5_alpha_cap[2],
   ['D' - BASECHAR] = font_5x5_alpha_cap[3],
   ['E' - BASECHAR] = font_5x5_alpha_cap[4],
   ['F' - BASECHAR] = font_5x5_alpha_cap[5],
   ['G' - BASECHAR] = font_5x5_alpha_cap[6],
   ['H' - BASECHAR] = font_5x5_alpha_cap[7],
   ['I' - BASECHAR] = font_5x5_alpha_cap[8],
   ['J' - BASECHAR] = font_5x5_alpha_cap[9],
   ['K' - BASECHAR] = font_5x5_alpha_cap[10],
   ['L' - BASECHAR] = font_5x5_alpha_cap[11],
   ['M' - BASECHAR] = font_5x5_alpha_cap[12],
   ['N' - BASECHAR] = font_5x5_alpha_cap[13],
   ['O' - BASECHAR] = font_5x5_alpha_cap[14],
   ['P' - BASECHAR] = font_5x5_alpha_cap[15],
   ['Q' - BASECHAR] = font_5x5_alpha_cap[16],
   ['R' - BASECHAR] = font_5x5_alpha_cap[17],
   ['S' - BASECHAR] = font_5x5_alpha_cap[18],
   ['T' - BASECHAR] = font_5x5_alpha_cap[19],
   ['U' - BASECHAR] = font_5x5_alpha_cap[20],
   ['V' - BASECHAR] = font_5x5_alpha_cap[21],
   ['W' - BASECHAR] = font_5x5_alpha_cap[22],
   ['X' - BASECHAR] = font_5x5_alpha_cap[23],
   ['Y' - BASECHAR] = font_5x5_alpha_cap[24],
   ['Z' - BASECHAR] = font_5x5_alpha_cap[25],
   ['0' - BASECHAR] = font_5x5_digit[0],
   ['1' - BASECHAR] = font_5x5_digit[1],
   ['2' - BASECHAR] = font_5x5_digit[2],
   ['3' - BASECHAR] = font_5x5_digit[3],
   ['4' - BASECHAR] = font_5x5_digit[4],
   ['5' - BASECHAR] = font_5x5_digit[5],
   ['6' - BASECHAR] = font_5x5_digit[6],
   ['7' - BASECHAR] = font_5x5_digit[7],
   ['8' - BASECHAR] = font_5x5_digit[8],
   ['9' - BASECHAR] = font_5x5_digit[9],
   ['.' - BASECHAR] = font_5x5_sep[0],
   [',' - BASECHAR] = font_5x5_sep[1],
   ['/' - BASECHAR] = font_5x5_sep[2],
   [':' - BASECHAR] = font_5x5_sep[3],
   ['-' - BASECHAR] = font_5x5_sep[4],
   ['%' - BASECHAR] = font_5x5_sep[5],
   ['$' - BASECHAR] = font_5x5_sep[6],
   ['[' - BASECHAR] = font_5x5_sep[7],
   ['<' - BASECHAR] = font_5x5_sep[8],
};

const font_t font_mono5 = {
	.spacing = 2,
	.height = 5, 
	.glyphs = mono5_glyphs,
	.basechar = BASECHAR,
};


