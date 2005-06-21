

#include <wpc.h>
#include <sys/types.h>
#include <sys/irq.h>

#define SWITCH_BITS_SIZE	(NUM_SWITCHES / BITS_PER_BYTE)

#define AR_RAW			0
#define AR_CHANGED 	1
#define AR_PENDING 	2
#define AR_QUEUED 	3
#define NUM_ARRAYS 	4

uint8_t switch_bits[NUM_ARRAYS][SWITCH_BITS_SIZE];


typedef struct
{
	void (*service_fn) (void);
	uint8_t flags;
} switch_info_t;


switch_info_t switch_info[NUM_SWITCHES] = {
};

void switch_init (void)
{
	memset ((uint8_t *)switch_bits, 0, SWITCH_BITS_SIZE * NUM_ARRAYS);
}


void switch_sched (uint16_t sw)
{
}


void switch_idle_task (void)
{
	uint8_t rawbits, pendbits;
	uint8_t col;

	for (col = 0; col < 8; col++)
	{
		disable_irq ();
		rawbits = switch_bits[AR_RAW][col];
		pendbits = switch_bits[AR_PENDING][col] & rawbits;
		switch_bits[AR_PENDING][col] = 0;
		enable_irq ();
		if (pendbits)
		{
			uint8_t sw = col << 3;
			uint8_t row;
			for (row=0; row < 8; row++, sw++, pendbits >>= 1)
			{
				if (pendbits & 1)
				{
					task_create (switch_sched, sw);
				}
			}
		}
	}
	task_exit ();
}

