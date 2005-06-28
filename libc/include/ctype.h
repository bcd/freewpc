/*
 *	ISO C99 Standard 7.4: Character handling	<ctype.h>
 */

#ifndef _CTYPE_H
#define _CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif


extern int isalnum(unsigned char c);
extern int isalpha(unsigned char c);
extern int iscntrl(unsigned char c);
extern int isdigit(unsigned char c);
extern int isgraph(unsigned char c);
extern int islower(unsigned char c);
extern int isprint(unsigned char c);
extern int ispunct(unsigned char c);
extern int isspace(unsigned char c);
extern int isupper(unsigned char c);
extern int isxdigit(unsigned char c);
extern int tolower(unsigned char c);
extern int toupper(unsigned char c);


#ifdef __cplusplus
}
#endif

#endif /* !defined _CTYPE_H */
