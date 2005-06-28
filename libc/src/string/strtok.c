/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdlib.h>

char *strtok(char *s1, const char *s2)
{
  static char *last;
  const char *spanp;
  char *tok;
  int c, sc;

  if (s1 == NULL && (s1 = last) == NULL)
    return NULL;

  /*
   * Skip (span) leading delimiters (s1 += strspn(s1, s2), sort of).
   */
 cont:
  c = *s1++;
  for (spanp = s2; (sc = *spanp++) != 0;) {
    if (c == sc)
      goto cont;
  }

  if (c == 0) {			/* no non-delimiter characters */
    last = NULL;
    return NULL;
  }
  tok = s1 - 1;

  /*
   * Scan token (scan for delimiters: s1 += strcspn(s1, s2), sort of).
   * Note that s2 must have one NUL; we stop if we see that, too.
   */
  for (;;) {
    c = *s1++;
    spanp = s2;
    do {
      if ((sc = *spanp++) == c) {
	if (c == 0)
	  s1 = NULL;
	else
	  s1[-1] = 0;
	last = s1;
	return (tok);
      }
    } while (sc != 0);
  }
  /* not reached */
}
