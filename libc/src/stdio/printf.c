#include <stdarg.h>
#include <stdio.h>

int printf(const char *format, ...)
{
   int ret;
   va_list ap;

   va_start(ap, format);
   ret = _doprnt(format, ap, stdout);
   va_end(ap);

   return ret;
}
