
#include <freewpc.h>

static uint8_t font_space[32] = { 0, };


fontargs_t font_args;


extern inline uint8_t lsrqi3 (uint8_t data, uint8_t count)
{
	switch (count)
	{
		default:
		case 7:
			data >>= 1;
		case 6:
			data >>= 1;
		case 5:
			data >>= 1;
		case 4:
			data >>= 1;
		case 3:
			data >>= 1;
		case 2:
			data >>= 1;
		case 1:
			data >>= 1;
		case 0:
			break;
	}
	return (data);
}


extern inline uint8_t aslqi3 (uint8_t data, uint8_t count)
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


uint8_t *font_lookup (const font_t *font, char c)
{
	uint8_t *entry;
	uint8_t index;

	if ((c >= 'A') && (c <= 'Z'))
	{
		entry = (uint8_t *)font->chars;
		index = c - 'A';
	}
	else if ((c >= '0') && (c <= '9'))
	{
		entry = (uint8_t *)font->digits;
		index = c - '0';
	}
	else if (c == ' ')
	{
		entry = font_space;
		index = 0;
	}
	else
	{
		entry = NULL;
		index = 0;
		fatal (ERR_UNPRINTABLE_CHAR);
	}
	return entry + index * font->height;
}


void fontargs_render_string (const fontargs_t *args)
{
	static uint8_t *dmd_base;
	const char *s = args->s;
	U8 x = args->x;
	const font_t *font = args->font;

#if 0
	db_puts ("font render y=");
	db_put2x (args->y);
	db_putc ('\n');
#endif

	dmd_base = ((uint8_t *)dmd_low_buffer) + args->y * DMD_BYTE_WIDTH;

	while (*s != '\0')
	{
		static uint8_t *data;
		static uint8_t i;
		static uint8_t xb;
		static uint8_t xr;

		if (*s == '.')
		{
			dmd_base[(font->height-1) * DMD_BYTE_WIDTH + x/8 - 1] |= 0x40;
			s++;
			continue;
		}
		else if (*s == ',')
		{
			dmd_base[(font->height-3) * DMD_BYTE_WIDTH + x/8 - 1] |= 0x40;
			dmd_base[(font->height-2) * DMD_BYTE_WIDTH + x/8 - 1] |= 0x40;
			dmd_base[(font->height-1) * DMD_BYTE_WIDTH + x/8 - 1] |= 0x80;
			s++;
			continue;
		}

		/* TODO : drawing to positions that are not 8-bit aligned
		 * does not work, so force to proper alignment for now. */
#if 11111
	x &= ~7;
#endif

#ifdef DB_FONT
		db_puts ("--- Rendering character "); db_putc (*s); db_puts ("---\n");
#endif

		data = font_lookup (font, *s);
		xb = x / 8;
		xr = x % 8;

		if (xr == 0)
		{
			for (i=0; i <font->height; i++)
				dmd_base[i * DMD_BYTE_WIDTH + xb] = *data++;
		}
		else
		{
			for (i=0; i <font->height; i++)
			{
#if 0
				db_put2x (xr);
				db_putc (' ');
				db_put2x (*data);
				db_putc (' ');
				db_put2x (*data << xr);
				db_putc (' ');
				db_put2x (*data >> (8 - xr));
				db_putc ('\n');
#endif
				dmd_base[i * DMD_BYTE_WIDTH + xb] = (*data << xr);
				// dmd_base[i * DMD_BYTE_WIDTH + xb] = aslqi3 (*data, xr);
				dmd_base[i * DMD_BYTE_WIDTH + xb + 1] = *data >> (8 - xr);
				// dmd_base[i * DMD_BYTE_WIDTH + xb + 1] = lsrqi3 (*data, (8 - xr));
				data++;
			}
		}

		x += font->width + font->spacing; /* advance by 1 char ... font->width */
		s++;
	}
}


uint8_t font_get_string_width (const font_t *font, const char *s)
{
	uint8_t width = 0;
	while (*s++ != '\0')
		width += (font->width + font->spacing);
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



