
#include <freewpc.h>


uint8_t errcode;

__fastram__ uint8_t irq_count;
__fastram__ uint8_t irq_shift_count;
__fastram__ uint8_t tick_count;

uint8_t sys_init_complete;


/* Macro for toggling the runtime LED */
#define wpc_led_toggle() \
do {											\
	asm (										\
		"lda " STR(WPC_LEDS) "\n"		\
		"eora #0x80\n"						\
		"sta " STR(WPC_LEDS) "\n"		\
		:										\
		: 										\
		: "a" 								\
	);											\
} while (0)


void fatal (errcode_t error_code) __noreturn__
{
}


void nonfatal (errcode_t error_code)
{
}


void irq_init (void)
{
	irq_shift_count = 1;
	irq_count = 0;
}


#pragma interrupt
void do_irq (void)
{
	*(uint8_t *)WPC_ZEROCROSS_IRQ_CLEAR = 0x96;

	/* Execute rtts every 1ms */
	wpc_led_toggle ();
	asm ("jsr switch_rtt");
	lamp_rtt ();
	sol_rtt ();
	irq_count++;

	irq_shift_count <<= 1;
	if (irq_shift_count == 0)
	{
		irq_shift_count = 1;

		/* Execute rtts every 8ms */
		tick_count++;
	}
}


#pragma interrupt
void do_firq (void)
{
	if (*(int8_t *)WPC_PERIPHERAL_TIMER_FIRQ_CLEAR < 0)
	{
		/* Timer interrupt */
	}
	else
	{
		/* DMD interrupt */
		dmd_rtt ();
	}

	*(uint8_t *)WPC_PERIPHERAL_TIMER_FIRQ_CLEAR  = 0;
}


#pragma interrupt
void do_nmi (void)
{
	fatal (ERR_NMI);
}


#pragma interrupt
void do_swi (void)
{
	fatal (ERR_SWI);
}


#pragma interrupt
void do_swi2 (void)
{
	fatal (ERR_SWI2);
}


#pragma interrupt
void do_swi3 (void)
{
	fatal (ERR_SWI3);
}


void idle (void)
{
}


void lamp_c_demo (void) __taskentry__
{
	lampset_set_apply_delay (0);
	lampset_apply_on (LAMPSET_ALL);

	lampset_set_apply_delay (TIME_33MS);
	for (;;)
	{
		lampset_apply_toggle (LAMPSET_ALL);
	}
}


void init (void) __noreturn__
{
	extern void lamp_demo ();
	extern void test_init (void);

	sys_init_complete = 0;
	
	wpc_led_toggle ();
	sol_init ();
	dmd_init ();
	switch_init ();
	sound_init ();

	wpc_led_toggle ();
	irq_init ();
	asm ("jsr heap_init");
	task_init ();
	deff_init ();

	*(uint8_t *)WPC_ZEROCROSS_IRQ_CLEAR = 0x06;

	sys_init_complete++;

	wpc_led_toggle ();

	//task_create_gid (0, lamp_demo, 0);
	task_create_gid (0, lamp_c_demo, 0);

	test_init ();
	task_exit ();
}


