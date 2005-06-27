
#include <freewpc.h>
#include <sys/irq.h>


uint8_t switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];


typedef struct
{
	void (*service_fn) (void);
	uint8_t flags;
} switch_info_t;


switch_info_t switch_info[NUM_SWITCHES] = {
};


void switch_init (void)
{
	memset ((uint8_t *)&switch_bits[0][0], 0, sizeof (switch_bits));
	memcpy ((uint8_t *)&switch_bits[0][0], mach_opto_mask, SWITCH_BITS_SIZE);
}


void switch_rtt (void)
{
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

