
#include <freewpc.h>

static char font_tinynum_digit[][7] = {
	{ 3, 5, 2, 5, 5, 5, 2 },
	{ 1, 5, 1, 1, 1, 1, 1 },
	{ 2, 5, 3, 2, 3, 1, 3 },
	{ 2, 5, 3, 2, 3, 2, 3 },
	{ 3, 5, 5, 5, 7, 4, 4 },
	{ 2, 5, 3, 1, 3, 2, 3 },
	{ 3, 5, 3, 1, 7, 5, 7 },
	{ 2, 5, 3, 2, 2, 2, 2 },
	{ 3, 5, 7, 5, 7, 5, 7 },
	{ 3, 5, 7, 5, 7, 4, 4 },
};

static volatile char font_tinynum_hexdigit[][7] = {
	{ 3, 5, 2, 5, 7, 5, 5 },
	{ 3, 5, 3, 5, 3, 5, 3 },
	{ 3, 5, 6, 1, 1, 1, 6 },
	{ 3, 5, 3, 5, 5, 5, 3 },
	{ 3, 5, 7, 1, 3, 1, 7 },
	{ 3, 5, 7, 1, 3, 1, 1 },
};

static char font_tinynum_sep[][7] = {
	/* period */ { 1, 5, 0, 0, 0, 0, 1 },
	/* comma */ { 1, 5, 0, 0, 0, 1, 1 },
};

#define BASECHAR ','

static char *tinynum_glyphs[] = {
   ['0' - BASECHAR] = font_tinynum_digit[0],
   ['1' - BASECHAR] = font_tinynum_digit[1],
   ['2' - BASECHAR] = font_tinynum_digit[2],
   ['3' - BASECHAR] = font_tinynum_digit[3],
   ['4' - BASECHAR] = font_tinynum_digit[4],
   ['5' - BASECHAR] = font_tinynum_digit[5],
   ['6' - BASECHAR] = font_tinynum_digit[6],
   ['7' - BASECHAR] = font_tinynum_digit[7],
   ['8' - BASECHAR] = font_tinynum_digit[8],
   ['9' - BASECHAR] = font_tinynum_digit[9],
   ['.' - BASECHAR] = font_tinynum_sep[0],
   [',' - BASECHAR] = font_tinynum_sep[1],
   ['A' - BASECHAR] = NULL,
};

const font_t font_tinynum = {
	.spacing = 1,
	.height = 5,
	.glyphs = tinynum_glyphs,
	.basechar = BASECHAR,
};


