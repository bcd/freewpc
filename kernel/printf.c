/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

/* When building with -mint16, 8-bit values are converted to 16-bits
before they are passed as arguments.  */
#ifdef __mint16__
#define PROMOTED_U8 U16
#else
#define PROMOTED_U8 U8
#endif


/**
 * \file
 * \brief An implementation of the C sprintf() routine (non-standard).  The
 * destination buffer is implicit; data is always written to 'sprintf_buffer'.
 * The buffer should immediately be written to the display or debugger port
 * without sleeping, to avoid reentrancy problems.
 *
 * The following format specifiers are supported:
 * %d - 8-bit decimal
 * %ld - 16-bit long decimal
 * %x - 8-bit hex
 * %lx - 16-bit long hex
 * %s - string
 * %c - character
 * %b - BCD string (with commas)
 * %p - pointer (same as 16-bit hex on the 6809)
 *
 * A number preceding any specifier indicates the desired length.  A '0'
 * prefix to that will always keep any leading zeroes; normally, they are
 * removed.
 *
 * This function is used even when running a native build.  The system's
 * 'sprintf' is never used.
 */


/** The destination buffer for all printing. */
char sprintf_buffer[PRINTF_BUFFER_SIZE];

U8 sprintf_width;

/** Nonzero if leading zeroes are permitted.  When zero, sprintf will
 * remove any leading zeroes from numbers. */
bool sprintf_leading_zeroes;

/** The number of leading zeroes in the last rendered number */
U8 leading_zero_count;

S16 number_length;

U8 min_width;

U8 comma_positions;

U8 commas_written;

char separator_char;


/** Convert a hex digit to its character representation */
char digit2char (U8 digit)
{
	if (digit <= 9)
		return digit + '0';
	else
		return digit - 10 + 'A';
}


/** Write an 8-bit decimal value 'b' to the buffer 'buf'.
 * The hundreds digit, if any, is checked first and then removed
 * to leave a value in the range of 0-99.  The DIV10() macro is
 * then called to divide the value by 10, producing the tens and
 * the ones digit.  The division is slightly more efficient than
 * the method using below for 16-bit values. */
char *do_sprintf_decimal (char *buf, U8 b)
{
	U8 quot;
	U8 rem;

	/* Print the hundreds digit and remove it from 'b'. */
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

	/* Calculate (b / 10, b % 10). */
	DIV10 (b, quot, rem);

	/* Output the tens and ones digits. */
	*buf++ = quot + '0';
	*buf++ = rem + '0';
	return buf;
}


/** Write a 16-bit decimal value 'w' to the buffer 'buf'.
 * Since there is no division algorithm implemented currently,
 * this function is fairly slow. */
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


/** Write an 8-bit hexadecimal value 'b' to the buffer 'buf'. */
char *do_sprintf_hex_byte (char *buf, U8 b)
{
	*buf++ = digit2char (b >> 4);
	if (comma_positions & 0x1)
	{
		*buf++ = separator_char;
		commas_written++;
	}
	comma_positions >>= 1;

	*buf++ = digit2char (b & 0x0F);
	if (comma_positions & 0x1)
	{
		*buf++ = separator_char;
		commas_written++;
	}
	comma_positions >>= 1;

	return buf;
}


#define HIGHBYTE(w)	(((U8 *)&w)[0])
#define LOWBYTE(w)	(((U8 *)&w)[1])


/** Write a 16-bit hexadecimal value 'w' to the buffer 'buf'. */
char *do_sprintf_hex (char *buf, U16 w)
{
	buf = do_sprintf_hex_byte (buf, HIGHBYTE(w));
	buf = do_sprintf_hex_byte (buf, LOWBYTE (w));
	return buf;
}


/** Generated formatted data based on the format string 'format'
 * into the buffer 'sprintf_buffer'.  Note that unlike the
 * real sprintf, this function doesn't return a value. */
void sprintf (const char *format, ...)
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
				/* '%E' is a nonstandard form that means to preserve
				the previous buffer and move to the end of it for
				writing additional characters.  It only makes sense to
				put this at the beginning of a format string. */
				case 'E':
					while (*buf != '\0')
						buf++;
					break;

				/* Handle format char '*' to dynamically set
				the width from a parameter */
				case '*':
					sprintf_width = va_arg (va, PROMOTED_U8);
					goto do_format_chars;

				case '0':
					if (sprintf_width == 0)
					{
						sprintf_leading_zeroes = TRUE;
						sprintf_width = 1;
						goto do_format_chars;
					}
					/* FALLTHRU on purpose */

				case '1': case '2': case '3':
				case '4': case '5': case '6':
				case '7': case '8': case '9':
					sprintf_width = (sprintf_width * 10) +
						*format - '0';
					goto do_format_chars;

				case 'd':
				case 'i':
				{
					register U8 b = va_arg (va, PROMOTED_U8);
					endbuf = do_sprintf_decimal (buf, b);
fixup_number:
					leading_zero_count = 0;
					while (((buf[leading_zero_count] == '0') ||
						(buf[leading_zero_count] == separator_char)) &&
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
							buf[min_width-1] = '0';
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

				case 'w':
				{
#ifdef CONFIG_NATIVE
do_32bit_hex_integer:
#endif
					do {
						S8 n;
						U32 w32 = va_arg (va, U32);
						U8 *wp32 = (U8 *)&w32;
#ifdef CONFIG_LITTLE_ENDIAN
						for (n = 3; n >= 0; n--)
#else /* CONFIG_BIG_ENDIAN */
						for (n = 0; n < 4; n++)
#endif
							buf = do_sprintf_hex_byte (buf, wp32[n]);
						endbuf = buf;
					} while (0);
					break;
				}

				case 'l':
				{
					++format;
					switch (*format)
					{
						case 'x': case 'X':
						{
#ifndef CONFIG_NATIVE
do_long_hex_integer:
#endif
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

					/* Initialize 'comma_positions' based on the length
					of the number.  When the least significant bit is
					set, it means that a comma should be printed AFTER
					the next digit is output.  As digits are printed,
					this variable is right-shifted. */
					switch (sprintf_width)
					{
						default:
							comma_positions = 0;
							break;

						case 8:
							comma_positions = 0x2 | 0x10;
							break;

						case 10:
							comma_positions = 0x1 | 0x8 | 0x40;
							break;
					}

					do
					{
						endbuf = do_sprintf_hex_byte (endbuf, *bcd++);
						sprintf_width -= 2;
					} while (sprintf_width);
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
						do {
							*_buf++ = *s++;
						} while (--sprintf_width);

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
					sprintf_leading_zeroes = TRUE;
#ifdef CONFIG_NATIVE /* handle 32-bit pointers */
					sprintf_width = 8;
					goto do_32bit_hex_integer;
#else
					sprintf_width = 4;
					goto do_long_hex_integer;
#endif
				}
			}
		}
		else
		{
output_char:
			*buf++ = *format;
		}
		format++;

		/* Detect when close to buffer overflow here and break out */
		if (buf > sprintf_buffer + PRINTF_BUFFER_SIZE - 2)
			break;
	}
	va_end (va);
	*buf = '\0';
}


/** Output a constant string in the far string page to
 * 'sprintf_buffer'.  srcp is a pointer to the string. */
void
sprintf_far_string (const char **srcp)
{
	char *dst = sprintf_buffer;
	const char *src;
	
	page_push (MD_PAGE);
	
	src = *srcp;
	if (src != NULL)
	{
		while (*src != '\0')
		{
			*dst++ = *src++;
		}
	}

	*dst++ = '\0';
	page_pop ();
}


/** Output a BCD-encoded score */
void
sprintf_score (const U8 *score)
{
#if (MACHINE_SCORE_DIGITS == 8)
	sprintf ("%8b", score);
#elif (MACHINE_SCORE_DIGITS == 10)
	sprintf ("%10b", score);
#elif (MACHINE_SCORE_DIGITS == 12)
	sprintf ("%12b", score);
#else
#error "invalid number of score digits"
#endif
}


/** Output the contents of the sprintf buffer to the debugger port. */
#ifdef DEBUGGER
void
dbprintf1 (void)
{
	puts_handler (sprintf_buffer);
}
#endif


/** Initialize the digit separation every time attract mode is
entered, in case the operator changes it in test mode. */
CALLSET_ENTRY (printf, amode_start)
{
	if (system_config.euro_digit_sep)
		separator_char = '.';
	else
		separator_char = ',';
}


/** At initialization, don't trust the adjustments and default
 * to US style. */
CALLSET_ENTRY (printf, init)
{
	separator_char = '.';
}


