#include <freewpc.h>

/* For each bitmap image, declare a character array with the width, height,
and data.  Bits appear in row-major order. */

static char bitmap_common_box3[] = { 3, 3, 0x7, 0x5, 0x7 };
static char bitmap_common_x3[] = { 3, 3, 0x5, 0x2, 0x5 };
static char bitmap_common_plus3[] = { 3, 3, 0x2, 0x7, 0x2 };

static char bitmap_common_box5[] = { 5, 5, 0x1f, 0x11, 0x11, 0x11, 0x1f };
static char bitmap_common_x5[] = { 5, 5, 0x1f, 0x1b, 0x15, 0x1b, 0x1f };

/* A table of all bitmaps grouped together in this 'font'.  The index
is used to retrieve the bitmap later. */
static char *bitmap_common_glyphs[] = {
   [BM_BOX3] = bitmap_common_box3,
   [BM_X3] = bitmap_common_x3,
   [BM_PLUS3] = bitmap_common_plus3,
   [BM_BOX5] = bitmap_common_box5,
   [BM_X5] = bitmap_common_x5,
};

/* The font descriptor */
const font_t font_bitmap_common = {
	.spacing = 0,
	.height = 0,
	.glyphs = bitmap_common_glyphs,
};

