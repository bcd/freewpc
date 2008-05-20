
#include <freewpc.h>


U8 printer_line_width;

U8 printer_page_length;

U8 printer_lineno;

U8 printer_colno;

U8 printer_pageno;


#define print_nl() print_char ('\n')


void printer_reconfig (void)
{
	printer_line_width = 72;
	printer_page_length = 66;
	printer_lineno = 0;
	printer_colno = 0;
	printer_pageno = 1;
}


void print_char (U8 c)
{
	wpc_parport_write (c);
	if (c == '\f')
	{
		printer_colno = 0;
		printer_lineno = 0;
	}
	else if (c == '\n')
	{
		printer_colno = 0;
		printer_lineno++;
	}
	else
	{
		printer_colno++;
	}
}

void print_string (const char *text)
{
	while (*text)
	{
		print_char (*text);
		text++;
	}
}


void print_string_center (const char *text)
{
}


void print_string_right (const char *text)
{
}


void print_repeated_char (U8 c, U8 count)
{
	while (count)
	{
		print_char (c);
		count--;
	}
}


void print_report (void)
{
	printer_reconfig ();
	print_string ("FREEWPC STATUS REPORT\n");
}


CALLSET_ENTRY (printer, init)
{
	printer_reconfig ();
}
