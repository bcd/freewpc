/*
 * Copyright 2006 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <freewpc.h>
#ifdef __m6809__
#include <m6809/math.h>
#endif

#ifdef __mint16__
#define PROMOTED_U8 U16
#else
#define PROMOTED_U8 U8
#endif


/**
 * \file
 * \brief An implementation of the C printf() routine (non-standard).
 */

char sprintf_buffer[PRINTF_BUFFER_SIZE];

U8 sprintf_width;

bool sprintf_leading_zeroes;

U8 leading_zero_count;

S16 number_length;

U8 min_width;

U8 comma_positions;

U8 commas_written;


char digit2char (uint8_t digit)
{
	if (digit <= 9)
		return digit + '0';
	else
		return digit - 10 + 'A';
}


char *do_sprintf_decimal (char *buf, U8 b)
{
	uint8_t quot;
	uint8_t rem;

	if (b >= 200)
	{
		*buf++ = '2';
		b -= 200;
	}
	else if (b >= 100)
	{
		*buf++ = '1';
		b -= 100;
	}

	DIV10 (b, quot, rem);

	*buf++ = quot + '0';
	*buf++ = rem + '0';
	return buf;
}



char *do_sprintf_long_decimal (char *buf, U16 w)
{
	static const U16 powers_of_ten[] = { 10000, 1000, 100, 10, 1, 0 };
	const U16 *current_power_of_ten = powers_of_ten;

	if (w == 0)
		*buf++ = '0';
	else while (*current_power_of_ten != 0)
	{
		U8 digit = 0;
		while (w >= *current_power_of_ten)
		{
			digit++;
			w -= *current_power_of_ten;
		}
		*buf++ = digit + '0';
		current_power_of_ten++;
	}
	return (buf);
}


char *do_sprintf_hex_byte (char *buf, uint8_t b)
{
	*buf++ = digit2char (b >> 4);
	if (comma_positions & 0x1)
	{
		*buf++ = ',';
		commas_written++;
	}
	comma_positions >>= 1;

	*buf++ = digit2char (b & 0x0F);
	if (comma_positions & 0x1)
	{
		*buf++ = ',';
		commas_written++;
	}
	comma_positions >>= 1;

	return buf;
}


#define HIGHBYTE(w)	(((U8 *)&w)[0])
#define LOWBYTE(w)	(((U8 *)&w)[1])

char *do_sprintf_hex (char *buf, uint16_t w)
{
	buf = do_sprintf_hex_byte (buf, HIGHBYTE(w));
	buf = do_sprintf_hex_byte (buf, LOWBYTE (w));
	return buf;
}


U8 sprintf (const char *format, ...)
{
	static va_list va;
	static char *buf;
	static char *endbuf;
	
	buf = sprintf_buffer;

	va_start (va, format);
	while (*format)
	{
		if (*format == '%')
		{
			if (format[1] == '%')
			{
				format++;
				goto output_char;
			}

			sprintf_width = 0;
			sprintf_leading_zeroes = FALSE;
			min_width = 1;
			comma_positions = 0;
			commas_written = 0;

do_format_chars:
			format++;
			switch (*format)
			{
				case '0':
					if (sprintf_leading_zeroes == FALSE)
					{
						sprintf_leading_zeroes = TRUE;
						goto do_format_chars;
					}
					/* FALLTHRU on purpose */

				case '1': case '2': case '3':
				case '4': case '5': case '6':
				case '7': case '8': case '9':
					sprintf_width <<= 4;
					sprintf_width += *format - '0';
					goto do_format_chars;

				case 'd':
				case 'i':
				{
					register U8 b = va_arg (va, PROMOTED_U8);
					endbuf = do_sprintf_decimal (buf, b);
fixup_number:
					leading_zero_count = 0;
					while (((buf[leading_zero_count] == '0') ||
						(buf[leading_zero_count] == ',')) &&
						(buf + leading_zero_count < endbuf))
					{
						leading_zero_count++;
					}

					if (sprintf_leading_zeroes)
					{
						/* OK to display leading zeroes */
						buf = endbuf;
					}
					else
					{
						number_length = endbuf - buf;

						/* Not OK to display leading zeroes */
						/* memmove (buf, 
						 * 	buf+leading_zero_count, 
						 * 	number_length-leading_zero_count) */
						if (number_length == leading_zero_count)
						{
							number_length = min_width;
							buf += min_width;
						}
						else
						{
							char *buf2 = buf;
							number_length -= leading_zero_count;

							while (number_length > 0)
							{
								buf2[0] = buf2[leading_zero_count];
								buf2++;
								number_length--;
							}

							buf = endbuf - leading_zero_count;
						}
					}
					break;
				}

				case 'x': case 'X':
				{
					register U8 b = va_arg (va, PROMOTED_U8);
					endbuf = do_sprintf_hex_byte (buf, b);
					goto fixup_number;
					break;
				}

				case 'l':
				{
					++format;
					switch (*format)
					{
						case 'x': case 'X':
						{
do_long_hex_integer:
							do {
								register U8 b = va_arg (va, U8);
								endbuf = do_sprintf_hex_byte (buf, b);
								b = va_arg (va, U8);
								endbuf = do_sprintf_hex_byte (endbuf, b);
								goto fixup_number;
							} while (0);
							break;
						}

						case 'd':
						{
							do {
								register U16 w = va_arg (va, U16);
								endbuf = do_sprintf_long_decimal (buf, w);
								goto fixup_number;
							} while (0);
							break;
						}
					}
					break;
				}

				case 'b':
				{
					/* TODO : this used to be a 'register' variable, but
					 * with the most recent gcc, that causes incorrect
					 * values to be displayed.  'static' works though... */
					static bcd_t *bcd;
					bcd = va_arg (va, bcd_t *);
					endbuf = buf;
					comma_positions = 0x12;
					while (sprintf_width != 0)
					{
						endbuf = do_sprintf_hex_byte (endbuf, *bcd++);
						sprintf_width -= 2;
					}
					min_width = 2;
					goto fixup_number;
					break;
				}

				case 's':
				{
					register const char *s = va_arg (va, const char *);
					register char *_buf = buf;
					if (sprintf_width == 0)
						while (*s)
							*_buf++ = *s++;
					else
						while (sprintf_width--)
							*_buf++ = *s++;

					buf = _buf;
					break;
				}

				case 'c':
				{
					register const char c = va_arg (va, PROMOTED_U8);
					*buf++ = c;
					break;
				}

				case 'p':
				{
// #ifdef CONFIG_PLATFORM_LINUX
/* TODO - handle 32-bit pointers */
// #else
					sprintf_leading_zeroes = TRUE;
					sprintf_width = 4;
					goto do_long_hex_integer;
// #endif
				}
			}
		}
		else
		{
output_char:
			*buf++ = *format;
		}
		format++;
	}
	va_end (va);

	*buf = '\0';
	return 0;
}


void
sprintf_far_string (const char **srcp)
{
	char *dst = sprintf_buffer;
	const char *src;
	
	wpc_push_page (MD_PAGE);
	
	src = *srcp;
	if (src != NULL)
	{
		while (*src != '\0')
		{
			*dst++ = *src++;
		}
	}

	*dst++ = '\0';
	wpc_pop_page ();
}

