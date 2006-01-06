
#include <freewpc.h>

static U8 font_space[32] = { 0, };


fontargs_t font_args;

U8 font_width;
U8 font_byte_width;
U8 font_height;

U8 font_string_width;
U8 font_string_height;


extern inline U8 lsrqi3 (U8 data, U8 count)
{
	register U8 d = data;
	switch (count)
	{
		default:
		case 7:
			d >>= 1;
		case 6:
			d >>= 1;
		case 5:
			d >>= 1;
		case 4:
			d >>= 1;
		case 3:
			d >>= 1;
		case 2:
			d >>= 1;
		case 1:
			d >>= 1;
		case 0:
			break;
	}
	return (d);
}


extern inline U8 aslqi3 (U8 data, U8 count)
{
	switch (count)
	{
		default:
		case 7:
			data <<= 1;
		case 6:
			data <<= 1;
		case 5:
			data <<= 1;
		case 4:
			data <<= 1;
		case 3:
			data <<= 1;
		case 2:
			data <<= 1;
		case 1:
			data <<= 1;
		case 0:
			break;
	}
	return (data);
}


#pragma long_branch
U8 *font_lookup (const font_t *font, char c)
{
	U8 *entry;
	U8 index;

	if (font->glyphs)
	{
		if (c == ' ')
		{
			U8 *data = font->glyphs[(U8)'I'];
			font_width = *data++;
			font_byte_width = (font_width + 7) >> 3;
			font_height = *data++;
			return (font_space);
		}
		else
		{
			U8 *data = font->glyphs[(U8)c];
			font_width = *data++;
			font_byte_width = (font_width + 7) >> 3;
			font_height = *data++;
			return (data);
		}
	}
	else if ((c >= 'A') && (c <= 'Z'))
	{
		entry = (U8 *)font->chars;
		index = c - 'A';
	}
	else if ((c >= '0') && (c <= '9'))
	{
		entry = (U8 *)font->digits;
		index = c - '0';
	}
	else if (c == '.')
	{
		entry = (U8 *)font->seps;
		index = 0;
	}
	else if (c == ',')
	{
		entry = (U8 *)font->seps;
		index = 1;
	}
	else if (c == '/')
	{
		entry = (U8 *)font->seps;
		index = 2;
	}
	else if (c == ' ')
	{
		entry = font_space;
		index = 0;
	}
	else if ((c >= 'a') && (c <= 'z'))
	{
		entry = (U8 *)font->chars;
		index = c - 'a';
	}
	else
	{
		entry = NULL;
		index = 0;
		nonfatal (ERR_UNPRINTABLE_CHAR);
	}

	font_width = font->width;
	font_byte_width = font->bytewidth;
	font_height = font->height;
	return entry + index * font->height;
}

#pragma long_branch
static void fontargs_render_string (void)
{
	static U8 *dmd_base;
	static const char *s;
	U8 x;
	char c;
	const fontargs_t *args = &font_args;

	dmd_base = ((U8 *)dmd_low_buffer) + args->y * DMD_BYTE_WIDTH;
	s = sprintf_buffer;
  	x = args->x;

	wpc_push_page (FONT_PAGE);

	while ((c = *s) != '\0')
	{
		static U8 *data;
		static U8 i;
		static U8 xb;
		static U8 xr;
		unsigned long int j;

		data = font_lookup (args->font, c);

		xb = x / 8;
		xr = x % 8;

		if (xr == 0)
		{
			for (i=0; i < font_height; i++)
			{
				for (j=0; j < font_byte_width; j++)
				{
					dmd_base[i * DMD_BYTE_WIDTH + xb + j] = *data++;
				}
			}
		}
		else
		{
			for (i=0; i < font_height; i++)
			{
				for (j=0; j < font_byte_width; j++)
				{
					dmd_base[i * DMD_BYTE_WIDTH + xb + j] |= 
						aslqi3 (*data, xr);
					dmd_base[i * DMD_BYTE_WIDTH + xb + j + 1] |= 
						lsrqi3 (*data, (8 - xr));
					data++;
				}
			}
		}

		/* advance by 1 char ... args->font->width */
		if ((c == '.') || (c == ','))
			x += 4;
		else
			x += font_width + args->font->spacing; 
		s++;
	}
	wpc_pop_page ();
}
#pragma short_branch


void font_get_string_area (const font_t *font, const char *s)
{
	U8 c;

	/* Copy the string to be rendered into the RAM buffer,
	 * if it is not already there.  This allows us to be
	 * more efficient later on, as page flipping between
	 * static strings and the font data is not required.
	 */
	if (s != sprintf_buffer)
	{
		U8 *ram = sprintf_buffer;
		while (*s != '\0')
			*ram++ = *s++;
		*ram = '\0';
		s = sprintf_buffer;
	}

	wpc_push_page (FONT_PAGE);

	if (font->glyphs)
	{
		font_string_width = 0;
		font_string_height = 0;

		while ((c = *s++) != '\0')
		{
			(void)font_lookup (font, c);
			font_string_width += font_width + font->spacing;
			if (font_height > font_string_height)
				font_string_height = font_height;
		}
	}
	else
	{
		U8 font_width, font_spacing;

		font_width = font->width;
		font_spacing = font->spacing;
		font_string_height = font->height;

		font_string_width = 0;
		while ((c = *s++) != '\0')
		{
			font_string_width += font_width + font_spacing;
		}
	}

	wpc_pop_page ();
	task_dispatching_ok = TRUE;
}

void fontargs_render_string_left (const fontargs_t *args)
{
	font_get_string_area (args->font, args->s);
	if (args != &font_args)
	{
		font_args.x = args->x;
		font_args.y = args->y;
		font_args.font = args->font;
		font_args.s = args->s;
	}
	fontargs_render_string ();
}


void fontargs_render_string_center (const fontargs_t *args)
{
	font_get_string_area (args->font, args->s);
	font_args.x = args->x - (font_string_width / 2);
	font_args.y = args->y - (font_string_height / 2);
	if (args != &font_args)
	{
		font_args.font = args->font;
		font_args.s = args->s;
	}
	fontargs_render_string ();
}


void fontargs_render_string_right (const fontargs_t *args)
{
	font_get_string_area (args->font, args->s);
	font_args.x = args->x - font_string_width;
	if (args != &font_args)
	{
		font_args.y = args->y;
		font_args.font = args->font;
		font_args.s = args->s;
	}
	fontargs_render_string ();
}

