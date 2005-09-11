#ifndef _SYS_FONT_H
#define _SYS_FONT_H

typedef struct
{
	uint8_t width;
	uint8_t spacing;
	uint8_t height;
	uint8_t **chars;
	uint8_t **digits;
} font_t;

extern const font_t font_5x5;

void font_render_string (const font_t *font, uint8_t x, uint8_t y, const char *s);
uint8_t font_get_string_width (const font_t *font, const char *s);
void font_render_string_center (const font_t *font, uint8_t x, uint8_t y, const char *s);
void font_render_string_right (const font_t *font, uint8_t x, uint8_t y, const char *s);

#endif /* _SYS_FONT_H */
