
#ifndef _SYS_DEBUG_H
#define _SYS_DEBUG_H

#ifndef __SASM__

/* Include ASIC address definitions */
#include <wpc.h>

#define db_status_reg	*(uint8_t *)WPC_DEBUG_CONTROL_PORT
#define db_data_reg		*(uint8_t *)WPC_DEBUG_DATA_PORT
#define db_write_ready	(db_status_reg & 0x1)
#define db_read_ready	(db_status_reg & 0x2)

#define db_inb db_data_reg

#define db_putc(b) \
{ \
	db_data_reg = b; \
}

void db_puts (const char *s);
void db_puti (uint8_t v);
void db_put2x (uint8_t v);
void db_put4x (uint16_t v);

#define db_putp(p)	db_put4x ((uint16_t)p)

void db_init (void);
void db_dump_switches (void);
void db_rtt (void);

#endif /* !__SASM__ */
#endif /* _SYS_DEBUG_H */
