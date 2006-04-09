#ifndef _SYS_FONT_H
#define _SYS_FONT_H


typedef struct
{
	U8 width;
	U8 bytewidth;
	U8 spacing;
	U8 height;
	U8 **chars;
	U8 **digits;
	U8 **seps;
	U8 **glyphs;
} font_t;

extern const font_t font_mono5;
extern const font_t font_mono9;
extern const font_t font_fixed10;
extern const font_t font_fixed6;
extern const font_t font_lucida9;
extern const font_t font_cu17;
extern const font_t font_term6;
extern const font_t font_times8;
extern const font_t font_times10;
extern const font_t font_helv8;

extern const font_t font_legacy5x5;
extern const font_t font_legacy9x6;

#define font_5x5 font_mono5
#define font_9x6 font_mono9

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

void font_get_string_area (const font_t *font, const char *s);
void fontargs_render_string_center (const fontargs_t *args);
void fontargs_render_string_right (const fontargs_t *args);
void fontargs_render_string_left (const fontargs_t *args);


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


#define font_render_string_left(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_left (&font_args); \
}

#define font_render_string font_render_string_left


#define font_render_string_center(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_center (&font_args); \
}

#define font_render_string_right(f,x,y,s) \
{ \
	DECL_FONTARGS(f,x,y,s); \
	fontargs_render_string_right (&font_args); \
}


#endif /* _SYS_FONT_H */
