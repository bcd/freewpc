
#include <freewpc.h>
#include <m6809_math.h>

char sprintf_buffer[PRINTF_BUFFER_SIZE];

U8 sprintf_width;

bool sprintf_leading_zeroes;

U8 leading_zero_count;

S8 number_length;

U8 min_width;


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

	DIV10 (b, quot, rem);

	*buf++ = quot + '0';
	*buf++ = rem + '0';
	return buf;
}


char *do_sprintf_hex_byte (char *buf, uint8_t b)
{
	*buf++ = digit2char (b >> 4);
	*buf++ = digit2char (b & 0x0F);
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


#pragma long_branch
void sprintf (const char *format, ...)
{
	static va_list va;
	static uint8_t *buf;
	static uint8_t *endbuf;
	
	buf = sprintf_buffer;

	va_start (va, format);
	while (*format)
	{
		if (*format == '%')
		{
			sprintf_width = 0;
			sprintf_leading_zeroes = FALSE;
			min_width = 1;

do_format_chars:
			format++;
			switch (*format)
			{
				case '0':
					sprintf_leading_zeroes = TRUE;
					goto do_format_chars;

				case '1': case '2': case '3':
				case '4': case '5': case '6':
				case '7': case '8': case '9':
					sprintf_width = *format - '0';
					goto do_format_chars;

				case 'd':
				case 'i':
				{
					register U8 b = va_arg (va, U8);
					endbuf = do_sprintf_decimal (buf, b);
fixup_number:
					leading_zero_count = 0;
					while ((buf[leading_zero_count] == '0') &&
						(buf + leading_zero_count < endbuf))
					{
						leading_zero_count++;
					}
					number_length = endbuf - buf;

					if (sprintf_leading_zeroes)
					{
						/* OK to display leading zeroes */
						buf = endbuf;
					}
					else
					{
						/* Not OK to display leading zeroes */
						/* memmove (buf, 
						 * 	buf+leading_zero_count, 
						 * 	number_length-leading_zero_count) */
#if 0
						*endbuf = '\0';
						db_puts ("Leading zeroes: ");
						db_puti (leading_zero_count);
						db_puts ("   Number length: ");
						db_puti (number_length);
						db_puts ("   Buffer: ");
						db_puts (buf);
						db_putc ('\n');
#endif

						if (number_length == leading_zero_count)
						{
							number_length = min_width;
							buf += min_width;
						}
						else
						{
							U8 *buf2 = buf;
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

				case 'x':
				{
					register U8 b = va_arg (va, U8);
					endbuf = do_sprintf_hex_byte (buf, b);
					goto fixup_number;
					break;
				}

				case 'b':
				{
					register bcd_t *bcd  = va_arg (va, bcd_t *);
					endbuf = buf;
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
					while (*s)
						*_buf++ = *s++;
					buf = _buf;
					break;
				}

				case 'p':
				{
					register void *p = va_arg (va, void *);
					p = p;
					break;
				}
			}
		}
		else
		{
			*buf++ = *format;
		}
		format++;
	}
	va_end (va);

	*buf = '\0';
}
#pragma short_branch

