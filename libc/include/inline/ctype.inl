#ifndef _INLINE_CTYPE_INL
#define _INLINE_CTYPE_INL

#define __ISCNTRL	0x01
#define __ISDIGIT	0x02
#define __ISGRAPH	0x04
#define __ISLOWER	0x08
#define __ISPRINT	0x10
#define __ISSPACE	0x20
#define __ISUPPER	0x40
#define __ISXDIGIT	0x80

extern unsigned char __ctype_flags[];
extern unsigned char __ctype_toupper[];
extern unsigned char __ctype_tolower[];

/* any character for which isalpha() or isdigit() is true */
#define __isalnum(c)  (__ctype_flags[c] & (__ISUPPER | __ISLOWER | __ISDIGIT))

/* any character for which isupper() or islower() is true */
#define __isalpha(c)  (__ctype_flags[c] & (__ISUPPER | __ISLOWER))

#define __iscntrl(c)  (__ctype_flags[c] & __ISCNTRL)
#define __isdigit(c)  (__ctype_flags[c] & __ISDIGIT)
#define __isgraph(c)  (__ctype_flags[c] & __ISGRAPH)
#define __islower(c)  (__ctype_flags[c] & __ISLOWER)
#define __isprint(c)  (__ctype_flags[c] & __ISPRINT)

/* all printing characters except a space and characters for which isalnum() is true */
#define __ispunct(c)  ((__ctype_flags[c] & __ISPRINT) && !(__ctype_flags[c] & (__ISSPACE | __ISUPPER | __ISLOWER | __ISDIGIT)))

#define __isspace(c)  (__ctype_flags[c] & __ISSPACE)
#define __isupper(c)  (__ctype_flags[c] & __ISUPPER)
#define __isxdigit(c) (__ctype_flags[c] & __ISXDIGIT)

#define __tolower(c) (__ctype_tolower[c])
#define __toupper(c) (__ctype_toupper[c])

#endif /* !defined _INLINE_CTYPE_INL */
