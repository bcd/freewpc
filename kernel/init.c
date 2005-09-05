
#include <freewpc.h>
#include <asm-6809.h>

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


void do_fatal (uint16_t pc, errcode_t error_code) __noreturn__
{
	dmd_alloc_low_clean ();
	seg_write_uint8 (SEG_ADDR(0,0,0), error_code);
	seg_write_hex16 (SEG_ADDR(0,1,0), pc);
	dmd_show_low ();
	for (;;);
}


void do_nonfatal (uint16_t pc, errcode_t error_code)
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
	db_rtt ();
	asm ("jsr switch_rtt");
	lamp_rtt ();
	sol_rtt ();
	triac_rtt ();
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


#pragma naked
void do_reset (void) __noreturn__
{
	register uint8_t *ramptr asm ("x");

	extern void lamp_demo ();
	extern void test_init (void);

	set_direct_page_pointer (0);

	set_stack_pointer (STACK_BASE);

	ramptr = (uint8_t *)USER_RAM_SIZE;
	do
	{
		*ramptr-- = 0;
	} while (ramptr != 0);

	*(volatile uint8_t *)WPC_RAM_LOCK = RAM_UNLOCKED;
	*(uint8_t *)WPC_RAM_LOCKSIZE = RAM_LOCK_512;
	*(volatile uint8_t *)WPC_RAM_LOCK = RAM_LOCK_512;

	sys_init_complete = 0;
	
	wpc_led_toggle ();
	db_init ();
	sol_init ();
	triac_init ();
	dmd_init ();
	switch_init ();
	sound_init ();

	device_init ();
	trough_init ();

	wpc_led_toggle ();
	irq_init ();
	task_init ();
	deff_init ();

	*(uint8_t *)WPC_ZEROCROSS_IRQ_CLEAR = 0x06;

	sys_init_complete++;

	wpc_led_toggle ();

	task_create_gid (0, lamp_c_demo);
	task_create_gid (0, device_probe);

	test_init ();
	task_exit ();
}


