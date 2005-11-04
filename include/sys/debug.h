
#ifndef _SYS_DEBUG_H
#define _SYS_DEBUG_H

/* Include ASIC address definitions */
#include <wpc.h>

#define db_status_reg	*(uint8_t *)WPC_DEBUG_CONTROL_PORT
#define db_data_reg		*(uint8_t *)WPC_DEBUG_DATA_PORT
#define db_write_ready	(db_status_reg & 0x1)
#define db_read_ready	(db_status_reg & 0x2)

#define db_inb db_data_reg

#ifdef DEBUGGER

#define db_putc(b) \
{ \
	db_data_reg = b; \
}

void db_puts (const char *s);
void db_puti (uint8_t v);
void db_put2x (uint8_t v);
void db_put4x (uint16_t v);
#else
#define db_puts(s)
#define db_puti(i)
#define db_put2x(v)
#define db_put4x(v)
#define db_putc(b)
#endif

#define db_putp(p)	db_put4x ((uint16_t)p)

void db_init (void);
void db_dump_switches (void);
void db_rtt (void);

#endif /* _SYS_DEBUG_H */
