
#include "wpc.h"
#include <sys/task.h>

short c1 (short x) { return x-1; }
char c2 (char x) { return x-2; }
int c3 (int x) { return x-3; }
long c4 (long x) { return x-4; }

inline void wpc_shift (void **addr, uint8_t *bit)
{
	*(uint16_t *)WPC_SHIFTADDR = (uint16_t)*addr;
	*(uint8_t *)WPC_SHIFTBIT = *bit;
	(uint16_t)(*addr) = *(uint16_t *)WPC_SHIFTADDR;
	*bit = *(uint8_t *)WPC_SHIFTBIT;
}

void 
test_shifter (void)
{
	void *addr = (void *)0x200;
	uint8_t bit = 16;
	wpc_shift (&addr, &bit);
}




void
test_task (uint16_t arg)
{
	int i;
	for (i=0; i < 10; i++)
	{
		*(uint8_t *)WPCS_DATA = 0x81;
		task_sleep (TIME_1S * 2);
	}
}


void
c_try_init (void)
{
}

