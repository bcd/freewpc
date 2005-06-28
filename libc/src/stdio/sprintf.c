#include <stdarg.h>
#include <stdio.h>

int sprintf(char *s, const char *format, ...)
{
   FILE file;
   int ret;
   va_list ap;

   file.flag = _IOSTRG;
   file.ptr = s;

   va_start(ap, format);
   ret = _doprnt(format, ap, &file);
   va_end(ap);

   *file.ptr = '\0';

   return ret;
}
