/*
 *	ISO C99 Standard: 7.21 String handling	<string.h>
 */

#ifndef _STRING_H
#define _STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>


extern void * memchr(const void *s, unsigned char c, size_t n);
extern int    memcmp(const void *s1, const void *s2, size_t n);
extern void * memcpy(void *s1, const void *s2, size_t n);
extern void * memmove(void *s1, const void *s2, size_t n);
extern void * memset(void *s, int c, size_t n);
extern char * strcat(char *s1, const char *s2);
extern char * strchr(const char *s, int c);
extern int    strcmp(const char *s1, const char *s2);
extern int    strcoll(const char *s1, const char *s2);
extern char * strcpy(char *s1, const char *s2);
extern size_t strcspn(const char *s1, const char *s2);
extern char * strerror(int errnum);
extern size_t strlen(const char *s);
extern char * strncat(char *s1, const char *s2, size_t n);
extern int    strncmp(const char *s1, const char *s2, size_t n);
extern char * strncpy(char *s1, const char *s2, size_t n);
extern char * strpbrk(const char *s1, const char *s2);
extern char * strrchr(const char *s, int c);
extern size_t strspn(const char *s1, const char *s2);
extern char * strstr(const char *s1, const char *s2);
extern char * strtok(char *s1, const char *s2);
extern size_t strxfrm(char *s1, const char *s2, size_t n);


#ifdef __cplusplus
}
#endif

#endif /* !defined _STRING_H */
