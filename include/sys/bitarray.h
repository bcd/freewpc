
#ifndef _SYS_BITARRAY_H
#define _SYS_BITARRAY_H

typedef uint8_t *bitset;

#define WPC_BITOP(bs, index, bitop)				\
do {														\
	__asm__ volatile (								\
		"st%0\t" STR(WPC_SHIFTADDR) "\n"			\
		"\tst%1\t" STR(WPC_SHIFTBIT) "\n"		\
		"\tld%0\t" STR(WPC_SHIFTADDR) "\n"		\
		"\tld%1\t" STR(WPC_SHIFTBIT) "\n"		\
		bitop												\
		: "=a" (bs), "=q" (index)					\
		: "a" (bs), "q" (index)						\
	);														\
} while (0)


#define __setbit(bs, index) \
	WPC_BITOP (bs, index, "\tor%1\t,%0\n\tst%1\t,%0\n")

#define __clearbit(bs, index) \
	WPC_BITOP (bs, index, "\tcom%1\n\tand%1\t,%0\n\tst%1\t,%0\n")

#define __togglebit(bs, index) \
	WPC_BITOP (bs, index, "\teor%1\t,%0\n\tst%1\t,%0\n")

#define __testbit(bs, index) \
	WPC_BITOP (bs, index, "\tand%1\t,%0\n")

#endif /* _SYS_BITARRAY_H */
