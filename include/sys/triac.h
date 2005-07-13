#ifndef _SYS_TRIAC_H
#define _SYS_TRIAC_H

#define TRIAC_GI0					0x01
#define TRIAC_GI1					0x02
#define TRIAC_GI2					0x04
#define TRIAC_GI3					0x08
#define TRIAC_GI4					0x10
#define TRIAC_COIN_INTERLOCK	0x20
#define TRIAC_FLIPPER_ENABLE	0x80

#ifndef __SASM__
typedef uint8_t triacbits_t;

void triac_rtt (void);
void triac_on (triacbits_t bits);
void triac_off (triacbits_t bits);
void triac_init (void);

#endif

#endif /* _SYS_TRIAC_H */
