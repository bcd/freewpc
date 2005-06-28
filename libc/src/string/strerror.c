/* Copyright (C) 2000 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <string.h>
#include <errno.h>


char *strerror(int errnum)
{
#ifdef __HAVE_DIVISION
  static char ebuf[40];		/* 64-bit number + slop */
  char *cp;
  int v=10000, lz=0;

  if (errnum >= 0 && errnum < __sys_nerr)
    return __sys_errlist[errnum];

  strcpy(ebuf, "Unknown error: ");
  cp = ebuf + strlen(ebuf);
  if (errnum < 0)
  {
    *cp++ = '-';
    errnum = -errnum;
  }
  while (v)
  {
    int d = errnum / v;
    if (d || lz || (v == 1))
    {
      *cp++ = d+'0';
      lz = 1;
    }
    errnum %= v;
    v /= 10;
  }
#endif
  return ebuf;
}
