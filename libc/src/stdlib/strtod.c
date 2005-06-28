/* Copyright (C) 1998 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1997 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <errno.h>
#include <ctype.h>
#include <libc/unconst.h>

double
strtod(const char *s, char **sret)
{
  long double r;		/* result */
  int e;			/* exponent */
  long double d;		/* scale */
  int sign;			/* +- 1.0 */
  int esign;
  int i;
  int flags=0;

  r = 0.0;
  sign = 1;
  e = 0;
  esign = 1;

  if (sret)
    *sret = unconst(s, char *);

  while (isspace((unsigned char) *s))
    s++;

  if (*s == '+')
    s++;
  else if (*s == '-')
  {
    sign = -1;
    s++;
  }

  while ((*s >= '0') && (*s <= '9'))
  {
    flags |= 1;
    r *= 10.0;
    r += *s - '0';
    s++;
  }

  if (*s == '.')
  {
    d = 0.1L;
    s++;
    while ((*s >= '0') && (*s <= '9'))
    {
      flags |= 2;
      r += d * (*s - '0');
      s++;
      d *= 0.1L;
    }
  }

  if (flags == 0)
    return 0;

  if (sret)
    *sret = unconst(s, char *);

  if ((*s == 'e') || (*s == 'E'))
  {
    s++;
    if (*s == '+')
      s++;
    else if (*s == '-')
    {
      s++;
      esign = -1;
    }
    if ((*s < '0') || (*s > '9'))
      return r * sign;

    while ((*s >= '0') && (*s <= '9'))
    {
      e *= 10;
      e += *s - '0';
      s++;
    }
  }

  /* Detect overflow.  */
  if (e < 0)
  {
    errno = ERANGE;
    r = HUGE_VAL;
  }
  else if (esign < 0)
    for (i = 1; i <= e; i++)
    {
      r *= 0.1L;
      /* Detect underflow below 2^-1075, which is half
         the smallest representable double. */
      if (r < 2.47032822920623272088e-324L)
      {
	errno = ERANGE;
	r = 0.0;
	break;
      }
    }
  else
    for (i = 1; i <= e; i++)
    {
      r *= 10.0;
      if (r > DBL_MAX)	/* detect overflow */
      {
	errno = ERANGE;
	r = HUGE_VAL;
	break;
      }
    }

  if (sret)
    *sret = unconst(s, char *);
  return r * sign;
}
