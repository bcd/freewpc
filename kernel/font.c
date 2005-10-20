
#include <freewpc.h>

static uint8_t font_space[32] = { 0, };


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


void font_render_string (const font_t *font, uint8_t x, uint8_t y, const char *s)
{
	static uint8_t *dmd_base;

	dmd_base = ((uint8_t *)dmd_low_buffer) + y * DMD_BYTE_WIDTH;

	while (*s != '\0')
	{
		static uint8_t *data;
		static uint8_t i;
		static uint8_t xb;
		static uint8_t xr;

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

void font_render_string_center (const font_t *font, uint8_t x, uint8_t y, const char *s)
{
	x -= (font_get_string_width (font, s) / 2);
	font_render_string (font, x, y, s);
}


void font_render_string_right (const font_t *font, uint8_t x, uint8_t y, const char *s)
{
	x -= font_get_string_width (font, s);
	font_render_string (font, x, y, s);
}



