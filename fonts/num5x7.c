
#include <freewpc.h>

static char font_5x7_digit[][9] = {
	/* 0 */ { 5, 7,    14, 17, 17, 17, 17, 17, 14 },
	/* 1 */ { 5, 7,    4, 6, 4, 4, 4, 4, 14 },
	/* 2 */ { 5, 7,    14, 17, 16, 14, 1, 1, 31 },
	/* 3 */ { 4, 7,    7, 8, 8, 7, 8, 8, 7 },
	/* 4 */ { 5, 7,    9, 9, 9, 31, 8, 8, 8 },
	/* 5 */ { 5, 7,    31, 1, 1, 15, 16, 16, 15 },
	/* 6 */ { 5, 7,    14, 1, 1, 15, 17, 17, 14 },
	/* 7 */ { 5, 7,    31, 8, 4, 4, 4, 4, 4 },
	/* 8 */ { 5, 7,    14, 17, 17, 14, 17, 17, 14 },
	/* 9 */ { 5, 7,    14, 17, 17, 30, 16, 17, 14 },
};

static char font_5x7_sep[][9] = {
	/* period */      { 2, 7, 0, 0, 0, 0, 0, 0, 2 },
	/* comma */       { 2, 7, 0, 0, 0, 0, 0, 2, 1 },
	/* slash */       { 3, 7, 4, 4, 2, 2, 2, 1, 1 },
	/* colon */       { 1, 7, 0, 0, 1, 0, 1, 0, 0 },
	/* hyphen */      { 3, 7, 0, 0, 0, 7, 0, 0, 0 },
	/* percent */     { 5, 7, 18, 8, 8, 4, 2, 2, 9 },
	/* dollar sign */ { 5, 7, 4, 14, 5, 14, 20, 14, 4 },
};

#define BASECHAR ' '

static char *num5x7_glyphs[] = {
   ['0' - BASECHAR] = font_5x7_digit[0],
   ['1' - BASECHAR] = font_5x7_digit[1],
   ['2' - BASECHAR] = font_5x7_digit[2],
   ['3' - BASECHAR] = font_5x7_digit[3],
   ['4' - BASECHAR] = font_5x7_digit[4],
   ['5' - BASECHAR] = font_5x7_digit[5],
   ['6' - BASECHAR] = font_5x7_digit[6],
   ['7' - BASECHAR] = font_5x7_digit[7],
   ['8' - BASECHAR] = font_5x7_digit[8],
   ['9' - BASECHAR] = font_5x7_digit[9],
   ['.' - BASECHAR] = font_5x7_sep[0],
   [',' - BASECHAR] = font_5x7_sep[1],
   ['/' - BASECHAR] = font_5x7_sep[2],
   [':' - BASECHAR] = font_5x7_sep[3],
   ['-' - BASECHAR] = font_5x7_sep[4],
   ['%' - BASECHAR] = font_5x7_sep[5],
   ['$' - BASECHAR] = font_5x7_sep[6],
   ['A' - BASECHAR] = NULL,
};

const font_t font_num5x7 = {
	.spacing = 2,
	.height = 7,
	.glyphs = num5x7_glyphs,
	.basechar = BASECHAR,
};


