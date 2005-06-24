
#ifndef _SYS_BITARRAY_H
#define _SYS_BITARRAY_H

typedef uint8_t *bitset;

#define WPC_BITOP(bs, index, bitop)				\
do {														\
	__asm__ volatile (								\
		"\tst%0 " STR(WPC_SHIFTADDR) "\n"		\
		"\tst%1 " STR(WPC_SHIFTBIT) "\n"			\
		"\tld%0 " STR(WPC_SHIFTADDR) "\n"		\
		"\tld%1 " STR(WPC_SHIFTBIT) "\n"			\
		bitop												\
		: "=a" (bs), "=d" (index)					\
		: "a" (bs), "d" (index)						\
	);														\
} while (0)


#define __setbit(bs, index) \
	WPC_BITOP (bs, index, "or%1 ,%0\n\tst%1, %0\n")

#define __clearbit(bs, index) \
	WPC_BITOP (bs, index, "com%1\n\tand%1 ,%0\n\tst%1, %0\n")

#define __togglebit(bs, index) \
	WPC_BITOP (bs, index, "eor%1 ,%0\n\tst%1, %0\n")

#define __testbit(bs, index) \
	WPC_BITOP (bs, index, "and%1 ,%0\n")

#endif /* _SYS_BITARRAY_H */
