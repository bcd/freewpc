/*
 * Copyright 2008 by Brian Dominy <brian@oddchange.com>
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
#include <test.h>
#include <format.h>

U8 printer_lineno;

U8 printer_colno;

U8 printer_pageno;

U8 printer_string_page;

bool print_header_needed;

const char *printout_name;

extern struct audit main_audits[];
extern struct audit earnings_audits[];
extern struct audit standard_audits[];

#define print_nl() print_char ('\n')
#define print_ff() print_char ('\f')
#define print_from_page(n) { printer_string_page = (n); }
#define print_from_this_page()  print_from_page (0)

void print_header (void);


void printer_reconfig (void)
{
	printer_lineno = 0;
	printer_colno = 0;
	printer_pageno = 0;
	print_header_needed = TRUE;
	printer_string_page = 0;
	print_from_this_page ();
}


void print_char (U8 c)
{
	if (print_header_needed)
	{
		print_header ();
	}

	if (c == '~')
	{
		wpc_parport_write (printer_pageno + '0');
	}
	else
	{
		wpc_parport_write (c);
	}

	if (c == '\n')
	{
		printer_colno = 0;
		printer_lineno++;
		task_sleep (TIME_16MS);
	}
	else
	{
		printer_colno++;
	}

	if ((c == '\f') || (printer_lineno == printer_config.lines_per_page))
	{
		printer_colno = 0;
		printer_lineno = 0;
		print_header_needed = TRUE;
	}
}

void print_string (const char *text)
{
	if (printer_string_page)
	{
		U8 c;
		while ((c = far_read8 (text, printer_string_page)))
		{
			print_char (c);
			text++;
		}
	}
	else
	{
		while (*text)
		{
			print_char (*text);
			text++;
		}
	}
}


void print_repeated_char (U8 c, U8 count)
{
	while (count)
	{
		print_char (c);
		count--;
	}
}


void printer_moveto (U8 colno)
{
	if (colno < printer_colno)
	{
		print_nl ();
	}
	print_repeated_char (' ', colno - printer_colno);
}


void print_line_center (const char *text)
{
	printer_moveto ((printer_config.column_width - strlen (text)) / 2);
	print_string (text);
	print_nl ();
}


void print_line_right (const char *text)
{
	printer_moveto (printer_config.column_width - strlen (text));
	print_string (text);
	print_nl ();
}


void print_bar (void)
{
	print_repeated_char ('-', printer_config.column_width);
	print_nl ();
}

void print_header (void)
{
	if (print_header_needed == FALSE)
		return;

	print_header_needed = FALSE;
	printer_pageno++;

	if (printer_config.pause_every_page == YES)
	{
		dmd_alloc_low_clean ();
		font_render_string_center (&font_mono5, 64, 11, "PRESS ENTER");
		font_render_string_center (&font_mono5, 64, 21, "FOR NEXT PAGE");
		dmd_show_low ();

		while (!switch_poll (SW_ENTER))
			task_sleep (TIME_66MS);
		while (switch_poll (SW_ENTER))
			task_sleep (TIME_66MS);
	}

	/* Print the actual page header.
	 * Note that we do not use sprintf() here, as that has been
	 * seen to cause stack overflows.  We opt for a much simpler
	 * approach whereby the tilde character is substituted with
	 * the real page number.  This limits reports to 9 pages.
	 * The proper solution would be to use fork a separate
	 * task to work around the stack size limitation.
	 */
	print_string (printout_name);
	print_line_right ("PAGE ~");
}


void print_audit_list (const char *title, struct audit *aud)
{
	U8 auditno = 1;
	audit_t *aptr;
	U8 format;

	print_string (title);
	print_nl ();
	print_bar ();

	while (far_read_pointer (&aud->name, TEST_PAGE) != NULL)
	{
		sprintf ("%02d", auditno);
		print_string (sprintf_buffer);

		printer_moveto (5);
		print_from_page (TEST_PAGE);
		print_string ((char *)far_read_pointer (&aud->name, TEST_PAGE));
		print_from_this_page ();

		aptr = far_read_pointer (&aud->nvram, TEST_PAGE);
		if (aptr)
		{
			format = far_read8 (&aud->format, TEST_PAGE);
			printer_moveto (30);
			render_audit (*aptr, format);
			print_string (sprintf_buffer);
		}

		print_nl ();
		aud++;
		auditno++;
	}
}


/* This function should be invoked from a separate task context,
because it may sleep. */
void print_all_audits (void)
{
	printer_reconfig ();

	printout_name = "AUDIT REPORT";
	print_audit_list ("MAIN AUDITS", main_audits);
	print_nl ();
	print_audit_list ("EARNINGS AUDITS", earnings_audits);
	print_nl ();
	print_audit_list ("STANDARD AUDITS", standard_audits);
	print_ff ();
}


CALLSET_ENTRY (printer, init)
{
	printer_reconfig ();
}

