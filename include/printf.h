
#ifndef _PRINTF_H
#define _PRINTF_H


typedef U16 *va_list;

#define PRINTF_BUFFER_SIZE		48


#define va_start(va, fmt) \
do { \
	va = (va_list)((unsigned char *)&fmt + sizeof (fmt)); \
} while (0) \


#define va_arg(va, type)	*((type *)va)++

#define va_end(va)

extern char sprintf_buffer[PRINTF_BUFFER_SIZE];
int printf (const char *format, ...);
#define sprintf printf

#ifdef DEBUGGER
#define dbprintf(format, rest...) \
	do { \
		printf (format, ## rest ); \
		db_puts (sprintf_buffer); \
	} while (0)
#else
#define dbprintf(format, rest...)
#endif /* DEBUGGER */

#endif /* _PRINTF_H */
