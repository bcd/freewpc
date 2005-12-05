
#include <freewpc.h>

static U8 font_space[32] = { 0, };


fontargs_t font_args;


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


U8 *font_lookup (const font_t *font, char c)
{
	U8 *entry;
	U8 index;

	if ((c >= 'A') && (c <= 'Z'))
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
		dbprintf ("Unprintable character: %i\n", c);
		fatal (ERR_UNPRINTABLE_CHAR);
	}

	return entry + index * font->height;
}

#pragma long_branch
void fontargs_render_string (const fontargs_t *args)
{
	static U8 *dmd_base;
	static const char *s;
	U8 x;

	dmd_base = ((U8 *)dmd_low_buffer) + args->y * DMD_BYTE_WIDTH;
   s = args->s;
  	x = args->x;

	wpc_push_page (FONT_PAGE);

	while (*s != '\0')
	{
		static U8 *data;
		static U8 i;
		static U8 xb;
		static U8 xr;

		xb = x / 8;

#ifdef DB_FONT
		db_puts ("--- Rendering character "); db_putc (*s); db_puts ("---\n");
#endif

		data = font_lookup (args->font, *s);
		xb = x / 8;
		xr = x % 8;

		if (xr == 0)
		{
			for (i=0; i <args->font->height; i++)
				dmd_base[i * DMD_BYTE_WIDTH + xb] = *data++;
		}
		else
		{
			for (i=0; i <args->font->height; i++)
			{
				dmd_base[i * DMD_BYTE_WIDTH + xb] |= aslqi3 (*data, xr);
				dmd_base[i * DMD_BYTE_WIDTH + xb + 1] |= lsrqi3 (*data, (8 - xr));
				data++;
			}
		}

		/* advance by 1 char ... args->font->width */
		if ((*s == '.') || (*s == ','))
			x += 4;
		else
			x += args->font->width + args->font->spacing; 
		s++;
	}

	wpc_pop_page ();
}
#pragma short_branch


U8 font_get_string_width (const font_t *font, const char *s)
{
	U8 oldpage = wpc_get_rom_page ();
	wpc_set_rom_page (FONT_PAGE);

	U8 width = 0;
	while (*s++ != '\0')
		width += (font->width + font->spacing);

	wpc_set_rom_page (oldpage);

	return (width);
}

void fontargs_render_string_center (const fontargs_t *args)
{
	font_args.x = args->x - (font_get_string_width (args->font, args->s) / 2);
	if (args != &font_args)
	{
		font_args.y = args->y;
		font_args.font = args->font;
		font_args.s = args->s;
	}
	fontargs_render_string (&font_args);
}


void fontargs_render_string_right (const fontargs_t *args)
{
	font_args.x = args->x - font_get_string_width (args->font, args->s);
	if (args != &font_args)
	{
		font_args.y = args->y;
		font_args.font = args->font;
		font_args.s = args->s;
	}
	fontargs_render_string (&font_args);
}



