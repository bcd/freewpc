
#ifndef _PRINTF_H
#define _PRINTF_H


typedef uint16_t *va_list;

#define PRINTF_BUFFER_SIZE		48


#define va_start(va, fmt) \
do { \
	va = (va_list)((unsigned char *)&fmt + sizeof (fmt)); \
} while (0) \



#define va_arg(va, type)	((type)*va++)

#define va_end(va)

extern char sprintf_buffer[PRINTF_BUFFER_SIZE];
void sprintf (const char *format, ...);

#define dbprintf(format, rest...) \
	do { \
		sprintf (format, ## rest ); \
		db_puts (sprintf_buffer); \
	} while (0)


#endif /* _PRINTF_H */
