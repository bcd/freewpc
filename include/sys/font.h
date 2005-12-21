#ifndef _SYS_FONT_H
#define _SYS_FONT_H

typedef struct
{
	uint8_t width;
	uint8_t spacing;
	uint8_t height;
	uint8_t **chars;
	uint8_t **digits;
	uint8_t **seps;
} font_t;

extern const font_t font_5x5;
extern const font_t font_9x6;

typedef struct
{
	const font_t *font;
	union {
		struct {
			U8 x;
			U8 y;
		};
		U16 xy;
	};
	const char *s;
} fontargs_t;

extern fontargs_t font_args;

void fontargs_render_string (const fontargs_t *args);
uint8_t font_get_string_width (const font_t *font, const char *s);
void fontargs_render_string_center (const fontargs_t *args);
void fontargs_render_string_right (const fontargs_t *args);


#define DECL_FONTARGS_CONST(_f,_x,_y,_s) \
	static fontargs_t args = { \
		.font = _f, \
		.x = _x, \
		.y = _y, \
		.s = _s, \
	};


#define DECL_FONTARGS(_f,_x,_y,_s) \
	font_args.font = _f; \
	font_args.xy = ((((U16)(_x)) << 8) | (_y)); \
	font_args.s = _s;


#define font_render_string(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string (&font_args); \
}

#define font_render_string_center(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_center (&font_args); \
}

#define font_render_string_right(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_center (&font_args); \
}


#endif /* _SYS_FONT_H */
