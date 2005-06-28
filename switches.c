
#include <freewpc.h>
#include <sys/irq.h>

#define switch_raw_bits			_switch_bits
#define switch_changed_bits	_switch_bits + (1 * SWITCH_BITS_SIZE)
#define switch_pending_bits	_switch_bits + (2 * SWITCH_BITS_SIZE)
#define switch_queued_bits		_switch_bits + (3 * SWITCH_BITS_SIZE)

uint8_t switch_bits[NUM_SWITCH_ARRAYS][SWITCH_BITS_SIZE];


typedef struct
{
	void (*service_fn) (void);
	uint8_t flags;
	uint8_t pad;
} switch_info_t;

extern void sw_left_coin (void);
extern void sw_right_coin (void);
extern void sw_center_coin (void);
extern void sw_fourth_coin (void);
extern void sw_escape_button (void);
extern void sw_down_button (void);
extern void sw_up_button (void);
extern void sw_enter_button (void);

static const switch_info_t switch_info[NUM_SWITCHES] = {
	[SW_LEFT_COIN] = { sw_left_coin, },
	[SW_CENTER_COIN] = { sw_center_coin, },
	[SW_RIGHT_COIN] = { sw_right_coin, },
	[SW_FOURTH_COIN] = { sw_fourth_coin, },
	[SW_ESCAPE] = { sw_escape_button, },
	[SW_DOWN] = { sw_down_button, },
	[SW_UP] = { sw_up_button, },
	[SW_ENTER] = { sw_enter_button, },
};


void switch_init (void)
{
	memset ((uint8_t *)&switch_bits[0][0], 0, sizeof (switch_bits));
	memcpy ((uint8_t *)&switch_bits[0][0], mach_opto_mask, SWITCH_BITS_SIZE);
}


extern inline void switch_rowpoll(const uint8_t col)
{
#if 0
	asm ("\tlda " STR(WPC_SW_ROW_INPUT) "\n");
	asm ("\tst%0 WPC_SW_COL_STROBE\n" :: "q" (1 << col));
	asm ("\tldb "	STR(switch_raw_bits) "+ %0\n" :: "q" (col));
	asm ("\tsta "	STR(switch_raw_bits) "+ %0\n" :: "q" (col));
	asm ("\teorb " STR(switch_raw_bits) "+ %0\n" :: "q" (col));
	asm ("\tstb "	STR(switch_changed_bits) "+%0\n" :: "q" (col));
	asm ("\torb "	STR(switch_pending_bits) "+%0\n" :: "q" (col));
	asm ("\tstb "	STR(switch_pending_bits) "+%0\n" :: "q" (col));
#endif
}

void switch_rtt (void)
{
	switch_rowpoll (0);
	switch_rowpoll (1);
	switch_rowpoll (2);
	switch_rowpoll (3);
	switch_rowpoll (4);
	switch_rowpoll (5);
	switch_rowpoll (6);
	switch_rowpoll (7);
}


void switch_sched (uint8_t sw)
{
	/* const switch_info_t *swinfo = &switch_info[sw];
	task_create_gid (0, swinfo->service_fn, sw); */
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

