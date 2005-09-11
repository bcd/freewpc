
#include <freewpc.h>
#include <m6809_math.h>

char sprintf_buffer[24];

uint8_t sprintf_width;

bool sprintf_leading_zeroes;


char digit2char (uint8_t digit)
{
	if (digit <= 9)
		return digit + '0';
	else
		return digit - 10 + 'A';
}


void sprintf (const char *format, ...)
{
	va_list va;
	uint8_t *buf = sprintf_buffer;

	va_start (va, format);
	while (*format)
	{
		if (*format == '%')
		{
do_format_chars:
			format++;
			
			sprintf_width = 0;
			sprintf_leading_zeroes = FALSE;

			switch (*format)
			{
				case '0':
					sprintf_leading_zeroes = TRUE;
					goto do_format_chars;

				case '1':
				case '2':
				case '3':
				case '4':
					sprintf_width = *format - '0';
					goto do_format_chars;

				case 'd':
				case 'i':
				{
					uint16_t w = va_arg (va, uint16_t);
					uint8_t quot;
					uint8_t rem;
					uint16_t quot_rem = div10 (w & 0xFF);

					asm ("sta %0" :: "m" (quot));
					asm ("stb %0" :: "m" (rem));

					*buf++ = quot + '0';
					*buf++ = rem + '0';
					break;
				}

#if 0
				case 'x':
				{
					uint16_t w = va_arg (va, uint16_t);
					*buf++ = digit2char (w & 0xFF);
					*buf++ = digit2char (w >> 8);
					break;
				}
#endif

				case 's':
				{
					const char *s = va_arg (va, const char *);
					while (*s)
						*buf++ = *s++;
					break;
				}

				case 'p':
				{
					void *p = va_arg (va, void *);
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
	db_puts ("sprintf: ");
	db_puts (sprintf_buffer);
	db_putc ('\n');
}


