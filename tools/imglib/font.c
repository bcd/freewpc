
#include "imglib.h"


void bitmap_write_glyph (struct buffer *bmp, const struct glyph *g, unsigned int x, unsigned int y)
{
	unsigned int row, col;
	U8 width = g->width;
	U8 height = g->height;
	U8 *bits = g->bits;
	U8 val;
	U8 bitno;

	bitmap_set_color (bmp, 1);

	unsigned int byte_width = (width + 7) / 8;
	for (row = 0; row < height; row++)
	{
		for (col = 0; col < byte_width; col++)
		{
			val = *bits++;
			for (bitno = 0; bits && bitno < 8; bitno++)
			{
				if (val & 1)
					bitmap_draw_pixel (bmp, x+col*8+bitno, y+row);
				val >>= 1;
			}
		}
	}
}


struct glyph *font_lookup_glyph (const font_t *font, char c)
{
	struct glyph *g = (struct glyph *)font->glyphs[c - font->basechar];
	return g;
}


struct coord font_get_string_size (const font_t *font, const char *text)
{
	struct coord coord;

	coord.y = font->height;
	coord.x = 0;
	while (*text != '\0')
	{
		if (*text != ' ')
		{
			struct glyph *g = font_lookup_glyph (font, *text);
			coord.x += g->width + 1;
		}
		else
		{
			coord.x += 5;
		}
		text++;
	}
	return coord;
}


void bitmap_write_text (struct buffer *bmp, const font_t *font,
	unsigned int x, unsigned int y,
	const char *text)
{
	while (*text != '\0')
	{
		char c = *text++;
		if (c != ' ')
		{
			struct glyph *g = font_lookup_glyph (font, c);
			bitmap_write_glyph (bmp, g, x, y);
			x += g->width + 1;
		}
		else
		{
			x += 5;
		}
	}
}


void bitmap_write_text_center (struct buffer *bmp, const font_t *font,
	unsigned int x, unsigned int y,
	const char *text)
{
	struct coord size = font_get_string_size (font, text);
	bitmap_write_text (bmp, font, x - size.x / 2, y - size.y / 2, text);
}


