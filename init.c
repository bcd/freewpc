
#include <wpc.h>

#include <sys/sol.h>
#include <sys/dmd.h>
#include <sys/sound.h>

#include <sys/task.h>
#include <sys/errno.h>

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
}


#pragma interrupt
void do_firq (void)
{
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


void init (void) __noreturn__
{
	extern void lamp_demo ();
	extern void test_init (void);

	sys_init_complete = 0;
	
	wpc_led_toggle ();
	sol_init ();
	dmd_init ();
	asm ("jsr switch_init");
	sound_init ();

	wpc_led_toggle ();
	asm ("jsr trace_init");
	irq_init ();
	asm ("jsr heap_init");
	task_init ();
	asm ("jsr deff_init");

	*(uint8_t *)WPC_ZEROCROSS_IRQ_CLEAR = 0x06;

	sys_init_complete++;

	wpc_led_toggle ();

	task_create_gid (0, lamp_demo, 0);

	test_init ();
	task_exit ();
}


