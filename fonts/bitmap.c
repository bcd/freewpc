#include <freewpc.h>

/* For each bitmap image, declare a character array with the width, height,
and data.  Bits appear in row-major order. */

static char bitmap_common_box3[] = { 
	3, 3,
	0x7, 0x5, 0x7
};

/* A table of all bitmaps grouped together in this 'font'.  The index
is used to retrieve the bitmap later. */
static char *bitmap_common_glyphs[] = {
   [1] = bitmap_common_box3,
};

/* The font descriptor */
const font_t font_bitmap_common = {
	.spacing = 1,
	.height = 3,
	.glyphs = bitmap_common_glyphs,
};

