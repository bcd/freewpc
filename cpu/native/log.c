#include <stdio.h>
#include <stdarg.h>

void print_log (const char *format, ...)
{
	va_list ap;
	char buf[256];

	va_start (ap, format);
	vsprintf (buf, format, ap);
	va_end (ap);

	fprintf (stdout, "%s", buf);
	fflush (stdout);
}

